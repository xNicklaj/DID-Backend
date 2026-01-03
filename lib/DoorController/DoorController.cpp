#include <DoorController.h>

DoorController::DoorController(int pin, int openDuration){
    init(pin, openDuration);
}

void DoorController::init(int pin, int openDuration){
    servoController.setup(pin);
    openTime = openDuration;
    hasBeenInitialized = true;
}

void DoorController::OpenDoor(){
    Serial.printf("Standby. Opening the door on pin %d...\n", servoController.getPin());
    servoController.setAngle(OPEN_ANGLE, 45); // Open position
    timer = 0;
    lastUpdateTime = millis();
}

void DoorController::CloseDoor(){
    servoController.setAngle(CLOSED_ANGLE, 45); // Closed position
}

void DoorController::setup(){
    if(!hasBeenInitialized) 
        init();
    CloseDoor();
}

void DoorController::update(){
    if(servoController.getAngle() != OPEN_ANGLE) return;
    
    int currentTime = millis();
    timer += (currentTime - lastUpdateTime);
    lastUpdateTime = currentTime;

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