#ifndef DOOR_CONTROLLER_H
#define DOOR_CONTROLLER_H

#include <ServoController.h>

#define CLOSED_ANGLE 0
#define OPEN_ANGLE 90

enum class DoorState{
    DOOR_CLOSED,
    DOOR_OPEN,
    DOOR_TRANSITIONING
};

class DoorController : public Worker {
    private:
        ServoController servoController;
        /// @brief Time the door remains open
        int openTime = 1000; // milliseconds
        bool hasBeenInitialized = false;
        int timer = 0;
        int lastUpdateTime = 0;
    public:
        /// @brief Just calls the init method
        /// @param pin 
        /// @param openDuration 
        DoorController(int pin = SERVO_PIN, int openDuration = 1000);

        /// @brief Initialize the DoorController
        void init(int pin = SERVO_PIN, int openDuration = 1000);

        /// @brief Opens the door for the configured duration. After said duration, closes the door.
        void OpenDoor();

        /// @brief Closes the door
        void CloseDoor();

        /// @brief Gets the current state of the door
        /// @return
        DoorState getDoorState();

        void setup() override;
        void update() override;
};

#endif