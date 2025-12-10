#include <ServoController.h>

void ServoController::setAngle(int angle){
    if(angle < 0) angle = 0;
    if(angle > 180) angle = 180;
    currentPos = angle;
    servo.write(angle);
    Serial.printf("Setting servo to %d degrees\n", angle);
    delay(15); // allow time to move  

}

void ServoController::setup(){
    ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
    this->servo.setPeriodHertz(50);
    this->servo.attach(SERVO_PIN);
    this->setAngle(0);
}

void ServoController::update(){
    this->setAngle((currentPos+5)%180);
}