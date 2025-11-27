#ifndef TESTWORKER_H
#define TESTWORKER_H

#include <Arduino.h>
#include <Worker.h>
#include <Adafruit_NeoPixel.h>

const uint16_t nPixels = 8;
const uint8_t  nPin    = 13; // CHECK YOUR CONNECTIONS !!!

class TestWorker : public Worker {
private:
    short int ledLvl = 0;
public:
    void setup() override;
    void update() override;
};

#endif