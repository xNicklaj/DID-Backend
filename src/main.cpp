#include <Arduino.h>

#include <TaskScheduler.h>
#include <CommandDispatcher.h>
#include <DtmfDecoder.h>
#include <SoundListener.h>
#include <WiFi_Connector.h>
#include <RTDBListener.h>
#include <LedController.h>

// --- Global System Objects ---
TaskScheduler workerSystem;
CommandDispatcher commandSystem;
DTMFDecoder dtmfSystem;
SoundListener listener;
WiFi_Connector wifiConnector;
RTDBListener rtdbListener;

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
  
  // 1. Setup Workers
  listener.setup();

  workerSystem.addWorker(&listener, 20);
  workerSystem.addWorker(dtmfWorker, 25);
  workerSystem.addWorker(&wifiConnector, 5000);
  workerSystem.addWorker(&rtdbListener, 1000);


  // 2. Setup Commands
  commandSystem.registerCommand("*123#", pair);
  commandSystem.registerCommand("*341#", open, 1);

  Serial.println("System Ready.");
}

void loop() {
  workerSystem.update();
}