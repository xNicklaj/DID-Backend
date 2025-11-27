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
    TaskScheduler();

    // API: Add a function to be called every 'interval' milliseconds
    void addWorker(TaskCallback func, unsigned long intervalMs);

    // API: Call this in the main loop to keep workers running
    void update();
};

#endif