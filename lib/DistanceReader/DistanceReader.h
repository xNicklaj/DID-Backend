#ifndef DISTANCE_READER_H
#define DISTANCE_READER_H

#include <Arduino.h>

class DistanceReader {
    private:
        int triggerPin;
        int echoPin;
    public:
        /// @brief Construct an uninitialized distance reader.
        DistanceReader();
        /// @brief Construct a distance reader with the provided pins.
        /// @param triggerPin GPIO pin for the trigger signal.
        /// @param echoPin GPIO pin for the echo signal.
        DistanceReader(int triggerPin, int echoPin);
        /// @brief Configure or reconfigure the pins used by the sensor.
        /// @param triggerPin GPIO pin for the trigger signal.
        /// @param echoPin GPIO pin for the echo signal.
        void setup(int triggerPin, int echoPin);
        /// @brief Read distance in centimeters.
        /// @return Distance in cm.
        long read();
};

#endif