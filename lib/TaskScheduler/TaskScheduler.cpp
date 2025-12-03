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
    newNode->startAt = 0;
    newNode->scheduled = false;
    newNode->next = head;
    head = newNode;

    // Call the worker's setup immediately upon registration
    worker->setup();
}

// Add with a non-blocking start delay. `setup()` is called immediately, but
// `update()` calls will not start until after `startDelayMs` milliseconds.
void TaskScheduler::addWorker(Worker* worker, unsigned long intervalMs, unsigned long startDelayMs) {
    if (worker == nullptr) return;
    TaskNode* newNode = new TaskNode;
    newNode->type = TaskNode::CLASS;
    newNode->handler.worker = worker;
    newNode->interval = intervalMs;
    newNode->lastRun = 0;
    newNode->next = head;
    head = newNode;

    // mark scheduling start
    unsigned long now = millis();
    if (startDelayMs == 0) {
        newNode->startAt = 0;
        newNode->scheduled = false;
    } else {
        newNode->startAt = now + startDelayMs;
        newNode->scheduled = true;
    }

    // Call setup immediately regardless of scheduled start
    worker->setup();
}

void TaskScheduler::update() {
    unsigned long currentMillis = millis();
    TaskNode* current = head;
    
    while (current != nullptr) {
        if (current->type == TaskNode::FUNC) {
            if (currentMillis - current->lastRun >= current->interval) {
                current->lastRun = currentMillis;
                if (current->handler.func) current->handler.func();
                if (current->interval == 0) yield();
            }
        } else { // CLASS
            Worker* w = current->handler.worker;
            if (w && w->isEnabled()) {
                // If scheduled to start in the future, wait until startAt
                if (current->scheduled) {
                    if (currentMillis >= current->startAt) {
                        // start now: run first update and clear scheduled flag
                        current->scheduled = false;
                        current->lastRun = currentMillis;
                        w->update();
                    }
                } else {
                    if (currentMillis - current->lastRun >= current->interval) {
                        current->lastRun = currentMillis;
                        w->update();
                        if (current->interval == 0) yield();
                    }
                }
            }
        }
        current = current->next;
    }
}
