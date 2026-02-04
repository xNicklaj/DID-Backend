#include <DistanceReader.h>

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
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    const float duration = pulseIn(echoPin, HIGH);
    const float distance = (duration * 0.0343f) / 2.0f;

    return distance;
}

