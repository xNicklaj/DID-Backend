#include <DistanceReader.h>

DistanceReader::DistanceReader() : triggerPin(-1), echoPin(-1) {}

DistanceReader::DistanceReader(int triggerPin, int echoPin) {
    setup(triggerPin, echoPin);
}

void DistanceReader::setup(int triggerPin, int echoPin) {
    this->triggerPin = triggerPin;
    this->echoPin = echoPin;

    pinMode(this->triggerPin, OUTPUT);
    pinMode(this->echoPin, INPUT);
}

long DistanceReader::read() {
    if (triggerPin < 0 || echoPin < 0) {
        Serial.println("ERROR: DistanceReader not initialized. Call setup() first.");
        return 0;
    }

    // Send trigger pulse
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    // Wait for echo with timeout
    unsigned long duration = pulseIn(echoPin, HIGH, PULSIN_TIMEOUT_US);
    
    if (duration == 0) {
        Serial.printf("ERROR: No echo received on pin %d (timeout). Check wiring!\n", echoPin);
        return 0;
    }

    // Convert duration to distance: distance = (duration * speed_of_sound) / 2
    // Speed of sound = 343 m/s = 0.0343 cm/us
    long distance = (duration * 0.0343f) / 2.0f;
    
    //Serial.printf("Distance: %ld cm (duration: %lu us) on pin %d\n", distance, duration, echoPin);
    return distance;
}

