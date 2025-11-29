#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() : head(nullptr) {}

// Functional worker
void TaskScheduler::addWorker(TaskCallback func, unsigned long intervalMs) {
    TaskNode* newNode = new TaskNode;
    newNode->type = TaskNode::FUNC;
    newNode->handler.func = func;
    newNode->interval = intervalMs;
    newNode->lastRun = 0;
    newNode->next = head;
    head = newNode;
}

// Class-based worker
void TaskScheduler::addWorker(Worker* worker, unsigned long intervalMs) {
    if (worker == nullptr) return;
    TaskNode* newNode = new TaskNode;
    newNode->type = TaskNode::CLASS;
    newNode->handler.worker = worker;
    newNode->interval = intervalMs;
    newNode->lastRun = 0;
    newNode->next = head;
    head = newNode;

    // Call the worker's setup immediately upon registration
    worker->setup();
}

void TaskScheduler::update() {
    unsigned long currentMillis = millis();
    TaskNode* current = head;
    
    while (current != nullptr) {
        if (currentMillis - current->lastRun >= current->interval) {
            current->lastRun = currentMillis;
            if (current->type == TaskNode::FUNC) {
                if (current->handler.func) current->handler.func();
                // If interval == 0, run as fast as possible but yield
                // to allow other system/background tasks to run.
                if (current->interval == 0) yield();
            } else { // CLASS
                if (current->handler.worker) current->handler.worker->update();
                if (current->interval == 0) yield();
            }
        }
        current = current->next;
    }
}
