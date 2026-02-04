#include <DoorController.h>

DoorController::DoorController(int doorId, int pin, int triggerPin, int echoPin, int openDuration){
    init(doorId, pin, triggerPin, echoPin, openDuration);
}

void DoorController::init(int doorId, int pin, int triggerPin, int echoPin, int openDuration){
    id = doorId;
    servoController.setup(pin);
    distanceReader.setup(triggerPin, echoPin);

    openTime = openDuration;
}

void DoorController::OpenDoor(){
    Serial.printf("Standby. Opening the door on pin %d...\n", servoController.getPin());
    servoController.setAngle(OPEN_ANGLE, 100); // Open position
    timer = 0;
    lastUpdateTime = millis();

    setDirty();
}

void DoorController::CloseDoor(){
    servoController.setAngle(CLOSED_ANGLE, 100); // Closed position
    Serial.printf("Door closed on pin %d.\n", servoController.getPin());

    setDirty();
}

DistanceState DoorController::getDistanceState(){
    long distance = distanceReader.read();
    if(distance <= DEFAULT_DISTANCE_THRESHOLD_CM){
        return DistanceState::DETECTED;
    }
    return DistanceState::NOT_DETECTED;
}

void DoorController::setOpenDuration(int openDurationMs){
    if (openDurationMs <= 0) return;
    openTime = openDurationMs;
}

void DoorController::setRtdb(RTDBConnector* connector){
    rtdb = connector;
}

void DoorController::setup(){
    CloseDoor();
}

void DoorController::syncState(){
    char path[50];

    snprintf(path, sizeof(path), "/doors/%d/state", id);
    rtdb->setBool(path, getDoorState() != DoorState::DOOR_CLOSED);

    snprintf(path, sizeof(path), "/doors/%d/hasProduct", id);
    rtdb->setBool(path, getDistanceState() == DistanceState::DETECTED);

    snprintf(path, sizeof(path), "/doors/%d/isTimerRunning", id);
    rtdb->setBool(path, timer > 0);
}

void DoorController::setDirty(){
    char path[50];
    snprintf(path, sizeof(path), "/doors/%d/isDirty", id);
    rtdb->setBool(path, true);
}

void DoorController::update(){
    syncState();

    if(servoController.getAngle() != OPEN_ANGLE) return;
    
    const unsigned long currentTime = millis();
    timer += static_cast<int>(currentTime - static_cast<unsigned long>(lastUpdateTime));
    lastUpdateTime = static_cast<int>(currentTime);

    if(timer >= openTime){
        timer = 0;
        CloseDoor();
    }
}

DoorState DoorController::getDoorState(){
    int angle = servoController.getAngle();
    if(angle == CLOSED_ANGLE) return DoorState::DOOR_CLOSED;
    if(angle == OPEN_ANGLE) return DoorState::DOOR_OPEN;
    return DoorState::DOOR_TRANSITIONING;
}