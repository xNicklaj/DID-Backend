#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <Arduino.h>

// Function pointer type for worker tasks
typedef void (*TaskCallback)();

struct TaskNode {
    TaskCallback callback;
    unsigned long interval;
    unsigned long lastRun;
    TaskNode* next;
};

class TaskScheduler {
private:
    TaskNode* head;

public:
    TaskScheduler() : head(nullptr) {}

    // API: Add a function to be called every 'interval' milliseconds
    void addWorker(TaskCallback func, unsigned long intervalMs) {
        TaskNode* newNode = new TaskNode;
        newNode->callback = func;
        newNode->interval = intervalMs;
        newNode->lastRun = 0;
        newNode->next = head;
        head = newNode;
    }

    // API: Call this in the main loop to keep workers running
    void update() {
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
};

#endif