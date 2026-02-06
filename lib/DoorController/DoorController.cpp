#include <DoorController.h>
#include <LedController.h>

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
    servoController.setAngle(OPEN_ANGLE, 70); // Open position
    timer = 1;
    lastUpdateTime = millis();

    // Initialize LED display - all 8 LEDs white
    updateLEDDisplay();

    syncState();
    setDirty();
}

void DoorController::CloseDoor(){
    servoController.setAngle(CLOSED_ANGLE, 70); // Closed position
    Serial.printf("Door closed on pin %d.\n", servoController.getPin());

    timer = 0;
    
    // Turn off all LEDs when door closes
    LedController::getInstance().clear();
    
    syncState();
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
    if (!rtdb) {
        Serial.println("RTDB not initialized. Cannot sync door state.");
        // Skip if RTDB not initialized
        return;
    }
    if(!rtdb->isConnected()){
        Serial.println("RTDB not connected. Cannot sync door state.");
        return;
    }
    
    // Batch all updates into a single JSON update to minimize blocking time
    char path[50];
    snprintf(path, sizeof(path), "/doors/%d", id);
    
    FirebaseJson json;
    json.set("isOpen", getDoorState() != DoorState::DOOR_CLOSED);
    json.set("hasProduct", getDistanceState() == DistanceState::DETECTED);
    json.set("isTimerRunning", timer > 0);
    rtdb->updateJSON(path, json);
    //Serial.printf("Synced door %d state to RTDB.\n", id);
}

void DoorController::setDirty(){
    if (!rtdb) return; // Skip if RTDB not initialized
    
    char path[50];
    snprintf(path, sizeof(path), "/doors/%d/isDirty", id);
    Serial.printf("Marking door %d as dirty in RTDB.\n", id);
    rtdb->setBool(path, true);
}

void DoorController::updateLEDDisplay(){
    // Each LED represents exactly 1/8th of the total time
    int timePerLed = openTime / 8;
    int ledsOff = timer / timePerLed;
    int ledsToLight = 8 - ledsOff;
    
    // Ensure we're within bounds
    if(ledsToLight > 8) ledsToLight = 8;
    if(ledsToLight < 0) ledsToLight = 0;
    
    // Update LED strip with 80% white intensity (204 = 255 * 0.8)
    uint32_t white = LedController::getInstance().Color(204, 204, 204);
    for(int i = 0; i < 8; i++){
        if(i < ledsToLight){
            LedController::getInstance().setPixelColor(i, white, false);
        } else {
            LedController::getInstance().setPixelColor(i, 0, false);
        }
    }
    LedController::getInstance().show();
}

void DoorController::update(){
/*     syncCounter++;
    if(syncCounter >= SYNC_INTERVAL_UPDATES){
        syncCounter = 0;
        syncState();
    } */
    if(servoController.getAngle() != OPEN_ANGLE) return;
    
    const unsigned long currentTime = millis();
    timer += static_cast<int>(currentTime - static_cast<unsigned long>(lastUpdateTime));
    lastUpdateTime = static_cast<int>(currentTime);

    // Update LED display to show timer progress
    updateLEDDisplay();

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