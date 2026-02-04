#include <DoorController.h>

DoorController::DoorController(int pin, int triggerPin, int echoPin, int openDuration){
    init(pin, triggerPin, echoPin, openDuration);
}

void DoorController::init(int pin, int triggerPin, int echoPin, int openDuration){
    servoController.setup(pin);
    distanceReader.setup(triggerPin, echoPin);

    openTime = openDuration;
}

void DoorController::OpenDoor(){
    Serial.printf("Standby. Opening the door on pin %d...\n", servoController.getPin());
    servoController.setAngle(OPEN_ANGLE, 100); // Open position
    timer = 0;
    lastUpdateTime = millis();
}

void DoorController::CloseDoor(){
    servoController.setAngle(CLOSED_ANGLE, 100); // Closed position
    Serial.printf("Door closed on pin %d.\n", servoController.getPin());
}

DistanceState DoorController::getDistanceState(){
    long distance = distanceReader.read();
    if(distance <= DEFAULT_DISTANCE_THRESHOLD_CM){
        return DistanceState::DETECTED;
    }
    return DistanceState::NOT_DETECTED;
}

void DoorController::setup(){
    CloseDoor();
}

void DoorController::update(){
    if(servoController.getAngle() != OPEN_ANGLE) return;
    
    const unsigned long currentTime = millis();
    timer += static_cast<int>(currentTime - static_cast<unsigned long>(lastUpdateTime));
    lastUpdateTime = static_cast<int>(currentTime);

    if(timer >= openTime){
        CloseDoor();
    }
}

DoorState DoorController::getDoorState(){
    int angle = servoController.getAngle();
    if(angle == CLOSED_ANGLE) return DoorState::DOOR_CLOSED;
    if(angle == OPEN_ANGLE) return DoorState::DOOR_OPEN;
    return DoorState::DOOR_TRANSITIONING;
}