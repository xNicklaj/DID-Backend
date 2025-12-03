#ifndef VU_METER_H
#define VU_METER_H

#include <Arduino.h>
#include <Worker.h>
#include <SoundListener.h>
#include <LedController.h>


#define SOUND_LEVEL_CAP 8000

class VUMeter : public Worker {
private:
    SoundListener* listener;
public:
    void setup() override;
    void update() override;

    void setListener(SoundListener* lst) {
        listener = lst;
    }
};

#endif