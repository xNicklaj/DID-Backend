#include <TaskScheduler.h>
#include <CommandDispatcher.h>
#include <DtmfDecoder.h>
#include <SoundListener.h>
#include <VUMeter.h>

#include <Arduino.h>

#define DTMF_WORKER_BUFF_SIZE 256

// --- Global System Objects ---
TaskScheduler workerSystem;
CommandDispatcher commandSystem;
DTMFDecoder dtmfSystem;
SoundListener listener;
VUMeter vuMeter;

// --- Hardware/State Variables ---


// ==========================================
// VIRTUAL WORKERS (Periodic Tasks)
// ==========================================

void soundListenerWorker(){
  listener.execute();
}

void dtmfWorker(){
  // Return if character is empty or sequence is empty
  if(dtmfSystem.detectDTMF(listener.getBuffer(), listener.getBufferSize())) return;
  if(dtmfSystem.getSequence().length() == 0) return;
  
  Serial.printf("Current Sequence: %s\n", dtmfSystem.getSequence());

  if(!commandSystem.execute(dtmfSystem.getSequence(), "")) return;
  dtmfSystem.clearSequence();
  
}

void vuMeterWorker(){
  Serial.println("Updating VU Meter...");
  vuMeter.update(listener.getBuffer(), listener.getBufferSize());
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
  sleep(200);

  // 1. Setup Workers
  listener.setup();
  vuMeter.setup();
  workerSystem.addWorker(soundListenerWorker, 50);
  workerSystem.addWorker(dtmfWorker, 50);
  workerSystem.addWorker(vuMeterWorker, 100);

  // 2. Setup Commands
  commandSystem.registerCommand("*123#", pair);

  Serial.println("System Ready.");
}

void loop() {
  // The loop remains clean, delegating logic to the managers
  Serial.println("Looping...");
  workerSystem.update();
}