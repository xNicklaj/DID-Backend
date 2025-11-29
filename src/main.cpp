#include <Arduino.h>

#include <TaskScheduler.h>
#include <CommandDispatcher.h>
#include <DtmfDecoder.h>
#include <SoundListener.h>
#include <VUMeter.h>
#include <EQMeter.h>

// --- Global System Objects ---
TaskScheduler workerSystem;
CommandDispatcher commandSystem;
DTMFDecoder dtmfSystem;
SoundListener listener;
VUMeter vuMeter;
EQMeter eqMeter;

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
  
  // 1. Setup Workers
  listener.setup();
  vuMeter.setup();
  eqMeter.setup();
  vuMeter.setListener(&listener);
  eqMeter.setListener(&listener);

  workerSystem.addWorker(&listener, 20);
  workerSystem.addWorker(dtmfWorker, 25);
  //workerSystem.addWorker(&vuMeter, 50);
  workerSystem.addWorker(&eqMeter, 50);


  // 2. Setup Commands
  commandSystem.registerCommand("*123#", pair);
  commandSystem.registerCommand("*341#", open, 1);

  Serial.println("System Ready.");
}

void loop() {
  workerSystem.update();
}