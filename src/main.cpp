#include <Arduino.h>

#include <TaskScheduler.h>
#include <CommandDispatcher.h>
#include <DtmfDecoder.h>
#include <SoundListener.h>
#include <VUMeter.h>
#include <TestWorker.h>

// --- Global System Objects ---
TaskScheduler workerSystem;
CommandDispatcher commandSystem;
DTMFDecoder dtmfSystem;
SoundListener listener;
VUMeter vuMeter;
TestWorker testWorker;

// --- Hardware/State Variables ---

// ==========================================
// VIRTUAL WORKERS (Periodic Tasks)
// ==========================================

void dtmfWorker(){
  // Return if character is empty or sequence is empty
  if(dtmfSystem.detectDTMF(listener.getBuffer(), listener.getBufferSize())) return;
  if(dtmfSystem.getSequence().length() == 0) return;
  
  Serial.printf("Current Sequence: %s\n", dtmfSystem.getSequence());

  if(!commandSystem.execute(dtmfSystem.getSequence(), "")) return;
  dtmfSystem.clearSequence();
  
}

void commandRunnerWorker(){
  String command = dtmfSystem.getSequence();
  if(command.length() == 0) return;
  commandSystem.execute(command, "");
}

// ==========================================
// COMMANDS (User Actions)
// ==========================================

void pair(String args){
  Serial.println("PAIRED");
}

// ==========================================
// SETUP & LOOP
// ==========================================

void setup() {
  Serial.begin(115200);
  
  // 1. Setup Workers
  listener.setup();
  vuMeter.setup();
  vuMeter.setListener(&listener);

  workerSystem.addWorker(&listener, 50);
  workerSystem.addWorker(dtmfWorker, 50);
  //workerSystem.addWorker(&vuMeter, 20);
  workerSystem.addWorker(&testWorker, 500);

  // 2. Setup Commands
  commandSystem.registerCommand("*123#", pair);

  Serial.println("System Ready.");
}

void loop() {
  //Serial.println("Looping...");
  workerSystem.update();
}