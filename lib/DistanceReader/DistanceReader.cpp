#include <DistanceReader.h>

DistanceReader::DistanceReader(int triggerPin, int echoPin){// Max distance 400cm
    this->triggerPin = triggerPin;
    this->echoPin = echoPin;

    pinMode(this->triggerPin, OUTPUT);
    pinMode(this->echoPin, INPUT);
}

/// @brief Read distance in cm
/// @return 
long DistanceReader::read() {
    float duration, distance;  

    digitalWrite(triggerPin, LOW);  
	delayMicroseconds(2);  
	digitalWrite(triggerPin, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(triggerPin, LOW);

    duration = pulseIn(echoPin, HIGH); 
    distance = (duration*.0343)/2;  

    return distance;
}

