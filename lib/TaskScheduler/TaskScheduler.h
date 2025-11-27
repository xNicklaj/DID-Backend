#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <Arduino.h>

// Function pointer type for worker tasks
typedef void (*TaskCallback)();

// Use the project's Worker base class so TaskScheduler matches existing
// worker implementations. Worker.h defines `setup()` and `execute()`.
#include <Worker.h>

struct TaskNode {
    enum Type { FUNC, CLASS } type;
    unsigned long interval;
    unsigned long lastRun;
    TaskNode* next;
    union {
        TaskCallback func;
        Worker* worker;
    } handler;
};

class TaskScheduler {
private:
    TaskNode* head;

public:
    TaskScheduler();

    // API: Add a function to be called every 'interval' milliseconds
    void addWorker(TaskCallback func, unsigned long intervalMs);

    // API: Add a Worker-derived object. `setup()` will be called immediately
    // when the worker is registered; `execute()` will be called each interval.
    void addWorker(Worker* worker, unsigned long intervalMs);

    // API: Call this in the main loop to keep workers running
    void update();
};

#endif