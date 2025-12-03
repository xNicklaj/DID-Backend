#include "VUMeter.h"
#include <Arduino.h>

void VUMeter::setup(){
    
}

void VUMeter::update(){
    // Nothing to do here for now
    if(listener == nullptr){
        Serial.println("VUMeter: No SoundListener assigned!");
        return;
    }

    int32_t* buffer = listener->getBuffer();
    size_t bytesRead = listener->getBytesRead();

    int samples = bytesRead / 4;

    if (samples <= 0) {
        // nothing to do
        return;
    }

    // Calcolo livello del segnale
    int64_t sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += abs(buffer[i]);
    }

    float level = float(sum) / float(samples);

    float rms = sqrt(level);

    int ledLvl = constrain(rms / SOUND_LEVEL_CAP * LedController::getInstance().numPixels(), 0, LedController::getInstance().numPixels());
    for (uint16_t i = 0; i < LedController::getInstance().numPixels(); i++){
        if(ledLvl == 0)
            LedController::getInstance().setPixelColor(i, 0);
        else if(ledLvl > 5)
            LedController::getInstance().setPixelColor(i, LedController::getInstance().Color(150, 0, 0));
        else if(ledLvl > 3)
            LedController::getInstance().setPixelColor(i, LedController::getInstance().Color(150, 150, 0));
    }
    LedController::getInstance().show();
}
