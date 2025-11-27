#ifndef SOUND_LISTENER_H
#define SOUND_LISTENER_H

#include <Arduino.h>
#include <driver/i2s.h>

#define SOUND_BUFFER_SIZE 256
#define SAMPLING_RATE     8000

class SoundListener {
private:
    int32_t soundBuffer[SOUND_BUFFER_SIZE];
    size_t bytesRead;

public:
    void setup();
    void execute();

    int getBufferSize();
    int32_t* getBuffer();
    size_t getBytesRead();
};

#endif