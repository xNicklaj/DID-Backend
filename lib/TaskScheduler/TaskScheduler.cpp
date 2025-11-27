#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() : head(nullptr) {}

void TaskScheduler::addWorker(TaskCallback func, unsigned long intervalMs) {
    TaskNode* newNode = new TaskNode;
    newNode->callback = func;
    newNode->interval = intervalMs;
    newNode->lastRun = 0;
    newNode->next = head;
    head = newNode;
}

void TaskScheduler::update() {
    unsigned long currentMillis = millis();
    TaskNode* current = head;
    
    while (current != nullptr) {
        if (currentMillis - current->lastRun >= current->interval) {
            current->lastRun = currentMillis;
            current->callback(); 
        }
        current = current->next;
    }
}
