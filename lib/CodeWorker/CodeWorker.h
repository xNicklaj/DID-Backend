#ifndef TESTWORKER_H
#define TESTWORKER_H

#include <Arduino.h>
#include <Worker.h>
#include <DtmfDecoder.h>
#include <RTDBListener.h>

class CodeWorker : public Worker {
private:
    short int ledLvl = 0;
    DTMFDecoder* decoder;
    RTDBListener* rtdb;
public:
    void setup() override;
    void update() override;
    void setDecoder(DTMFDecoder* dec) { decoder = dec; };
    void setRTDBListener(RTDBListener* rtdbListener) { rtdb = rtdbListener; }
};

#endif