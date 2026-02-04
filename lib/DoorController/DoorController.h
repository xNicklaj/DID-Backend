#ifndef DOOR_CONTROLLER_H
#define DOOR_CONTROLLER_H

#include <ServoController.h>
#include <DistanceReader.h>

#define CLOSED_ANGLE 0
#define OPEN_ANGLE 180
#define DEFAULT_DISTANCE_THRESHOLD_CM 60

enum class DoorState{
    DOOR_CLOSED,
    DOOR_OPEN,
    DOOR_TRANSITIONING
};

enum class DistanceState{
    NOT_DETECTED,
    DETECTED
};

class DoorController : public Worker {
    private:
        ServoController servoController;
        DistanceReader distanceReader;
        /// @brief Time the door remains open
        int openTime = 1000; // milliseconds
        int timer = 0;
        int lastUpdateTime = 0;
    public:
        /// @brief Construct a DoorController and initialize its pins.
        /// @param doorPin Servo pin used for the door.
        /// @param triggerPin Trigger pin for the distance sensor.
        /// @param echoPin Echo pin for the distance sensor.
        /// @param openDuration Duration in milliseconds to keep the door open.
        DoorController(int doorPin, int triggerPin, int echoPin, int openDuration = 1000);

        /// @brief Initialize or re-initialize the DoorController pins and open duration.
        /// @param pin Servo pin used for the door.
        /// @param triggerPin Trigger pin for the distance sensor.
        /// @param echoPin Echo pin for the distance sensor.
        /// @param openDuration Duration in milliseconds to keep the door open.
        void init(int pin, int triggerPin, int echoPin, int openDuration = 1000);

        /// @brief Opens the door for the configured duration. After said duration, closes the door.
        void OpenDoor();

        /// @brief Closes the door.
        void CloseDoor();

        /// @brief Gets the current state of the door.
        /// @return Current door state.
        DoorState getDoorState();

        /// @brief Gets whether an object was detected within the threshold distance or not.
        /// @return Detection state.
        DistanceState getDistanceState();

        /// @brief Called by the scheduler to initialize the worker state.
        void setup() override;
        /// @brief Called by the scheduler to update the worker state.
        void update() override;
};

#endif