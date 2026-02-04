#include <Arduino.h>

#include <TaskScheduler.h>
#include <CommandDispatcher.h>
#include <DtmfDecoder.h>
#include <SoundListener.h>
#include <WiFi_Connector.h>
#include <RTDBConnector.h>
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
RTDBConnector rtdbConnector;
CodeWorker codeWorker;
DoorController* doorController[2];

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

void timerDurationWorker(){
  static int lastDurationMs = -1;
  int durationMs = 0;

  if(!rtdbConnector.getInt("/timerDuration", durationMs)) return;
  if(durationMs <= 0) return;
  if(durationMs == lastDurationMs) return;

  lastDurationMs = durationMs;
  doorController[0]->setOpenDuration(durationMs);
  doorController[1]->setOpenDuration(durationMs);
  Serial.printf("Updated timerDuration: %d ms\n", durationMs);
}

// ==========================================
// COMMANDS (DTMF Actions)
// ==========================================

void open(String args){
  Serial.printf("OPENED %s\n", args.c_str());
  if(args.c_str()[0]){
    int doorId = args.c_str()[0] - '0';
    if(doorId >=0 && doorId < 2){
      doorController[doorId]->OpenDoor();
    }
  }
}

// ==========================================
// SETUP & LOOP
// ==========================================

void setup() {
  Serial.begin(115200);

  LedController::getInstance().setup();

  doorController[0] = new DoorController(0, 13, 12, 35, 30000);
  doorController[1] = new DoorController(1, 27, 25, 34, 30000);
  
  // 1. Setup Workers
  rtdbConnector.setWiFiConnector(&wifiConnector);

  codeWorker.setDecoder(&dtmfSystem);
  codeWorker.setRTDBConnector(&rtdbConnector);
  workerSystem.addWorker(&listener, 20);
  workerSystem.addWorker(dtmfWorker, 25);
  workerSystem.addWorker(timerDurationWorker, 1000);
  workerSystem.addWorker(&wifiConnector, 500);
  workerSystem.addWorker(&rtdbConnector, 50);
  workerSystem.addWorker(&codeWorker, 50);
  workerSystem.addWorker(doorController[0], 50);
  workerSystem.addWorker(doorController[1], 50);


  // 2. Setup Commands
  commandSystem.registerCommand("*0F39#", open, 1);

  Serial.println("System Ready.");
}

void loop() {
  workerSystem.update();
}