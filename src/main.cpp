#include <Arduino.h>

#include <TaskScheduler.h>
#include <CommandDispatcher.h>
#include <DtmfDecoder.h>
#include <SoundListener.h>
#include <WiFi_Connector.h>
#include <RTDBListener.h>
#include <CodeWorker.h>
#include <ServoController.h>
#include <DoorController.h>
#include <DistanceReader.h>
#include <VUMeter.h>

// --- Global System Objects ---
TaskScheduler workerSystem;
CommandDispatcher commandSystem;
DTMFDecoder dtmfSystem;
SoundListener listener;
WiFi_Connector wifiConnector;
RTDBListener rtdbListener;
CodeWorker codeWorker;
DoorController doorController;
DistanceReader distanceReaderObj(26, 35);
VUMeter vuMeter;

// --- Hardware/State Variables ---

// ==========================================
// VIRTUAL WORKERS (Periodic Tasks)
// ==========================================

void dtmfWorker(){
  // Return if character is empty or sequence is empty
  if(dtmfSystem.detectDTMF(listener.getBuffer(), listener.getBufferSize())) return;
  if(dtmfSystem.getSequence().length() == 0) return;

  if(!commandSystem.execute(dtmfSystem.getSequence(), "")) return;
  dtmfSystem.clearSequence();
}

void doorWorker(){
  doorController.OpenDoor();
}

void distanceReader(){
  long distance = distanceReaderObj.read();
  Serial.printf("Distance: %ld cm\n", distance);
}

// ==========================================
// COMMANDS (DTMF Actions)
// ==========================================

void pair(String args){
  Serial.println("PAIRED");
}

void open(String args){
  Serial.printf("OPENED %s\n", args.c_str());
}

// ==========================================
// SETUP & LOOP
// ==========================================

void setup() {
  Serial.begin(115200);

  LedController::getInstance().setup();
  doorController.init(12, 4000); // Pin 12, 4 seconds open time

  vuMeter.setListener(&listener);
  
  // 1. Setup Workers
  //rtdbListener.setWiFiConnector(&wifiConnector);
  codeWorker.setDecoder(&dtmfSystem);
  codeWorker.setRTDBListener(&rtdbListener);

  workerSystem.addWorker(&listener, 20);
  workerSystem.addWorker(dtmfWorker, 25);
  //workerSystem.addWorker(&wifiConnector, 500);
  //workerSystem.addWorker(&rtdbListener, 50);
  workerSystem.addWorker(&codeWorker, 50);
  workerSystem.addWorker(&vuMeter, 200);
  //workerSystem.addWorker(&doorController, 500);
  //workerSystem.addWorker(doorWorker, 20000);
  //workerSystem.addWorker(distanceReader, 1000);

  // 2. Setup Commands
  commandSystem.registerCommand("*123#", pair);
  commandSystem.registerCommand("*341#", open, 1);

  Serial.println("System Ready.");
}

void loop() {
  workerSystem.update();
}