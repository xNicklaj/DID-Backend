#ifndef DISTANCE_READER_H
#define DISTANCE_READER_H

#include <Arduino.h>

class DistanceReader {
    private:
        int triggerPin;
        int echoPin;
    public:
        DistanceReader(int triggerPin, int echoPin);
        long read();
};

#endif