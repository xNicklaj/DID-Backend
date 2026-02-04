#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Arduino.h>
#include <Worker.h>
#include <ESP32Servo.h>

#define SERVO_PIN 12

class ServoController {
private:
    Servo servo;
    int currentPos = 0;
    int pin = SERVO_PIN;
    /// @brief Clamp a requested angle to the valid 0-180 range.
    /// @param angle Requested angle in degrees.
    /// @return Clamped angle in degrees.
    int clampAngle(int angle) const;
public:
    /// @brief Returns the pin number if the servo is attached, -1 otherwise
    /// @return 
    int getPin();
    /// @brief Initialize the servo and attach it to the provided pin.
    /// @param pin GPIO pin to attach the servo to.
    void setup(int pin = SERVO_PIN);
    /// @brief Sets the servo to the specified angle (0-180)
    /// @param angle
    void setAngle(int angle, int degreesPerSecond = 90);
    /// @brief Gets the current angle of the servo
    /// @return Current angle (0-180)
    int getAngle() const { return currentPos; }
};

#endif