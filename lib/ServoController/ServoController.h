#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Arduino.h>
#include <Worker.h>
#include <ESP32Servo.h>

#define SERVO_PIN 12

class ServoController : public Worker {
private:
    Servo servo;
    int currentPos = 0;
public:
    void setup() override;
    void update() override;
    void setAngle(int angle);
    int getAngle() const { return currentPos; }
};

#endif