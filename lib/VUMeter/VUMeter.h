#ifndef VU_METER_H
#define VU_METER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

const uint16_t kPixels = 8;
const uint8_t  kPin    = 13; // CHECK YOUR CONNECTIONS !!!

extern Adafruit_NeoPixel strip;

class VUMeter {
public:
    void setup();
    void update(int32_t* buffer, size_t size, size_t bytesRead);
};

#endif