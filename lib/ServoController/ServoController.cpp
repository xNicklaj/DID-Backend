#include <ServoController.h>

void ServoController::setAngle(int angle, int degreesPerSecond) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    /* if (degreesPerSecond <= 0) {
        // Fallback to immediate move
        setAngle(angle);
        return;
    } */

    int start = currentPos;
    if (start == angle) return;

    int step = (angle > start) ? 1 : -1;
    unsigned long stepDelayMs = 1000UL / (unsigned long)abs(degreesPerSecond);
    if (stepDelayMs == 0) stepDelayMs = 1; // safety floor

    Serial.printf("Slewing servo from %d to %d at %d dps\n", start, angle, degreesPerSecond);

    for (int pos = start; pos != angle; pos += step) {
        servo.write(pos);
        currentPos = pos;
        delay(stepDelayMs);
    }
    servo.write(angle);
    currentPos = angle;
    delay(15); // small settle
}

void ServoController::setup(int pin){
    this->pin = pin;
    ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
    this->servo.setPeriodHertz(50);
    this->servo.attach(pin);
    //this->setAngle(0);
}

int ServoController::getPin(){
    return servo.attached() ? pin : -1;
}