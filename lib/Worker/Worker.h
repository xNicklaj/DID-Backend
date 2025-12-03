#ifndef WORKER_H
#define WORKER_H

class Worker {
public:
    Worker() : enabled(true) {}

    // Called once to initialize the worker
    virtual void setup();

    // Called periodically by the scheduler when enabled
    virtual void update();

    // Enable/disable the worker's update() being invoked by the scheduler
    void setEnabled(bool e) { enabled = e; }
    bool isEnabled() const { return enabled; }

private:
    bool enabled;
};

#endif