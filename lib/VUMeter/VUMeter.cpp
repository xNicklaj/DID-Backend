#include "VUMeter.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

Adafruit_NeoPixel strip(kPixels, kPin, NEO_GRB + NEO_KHZ800);

void VUMeter::setup(){
    strip.begin();
    strip.show();
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

    int ledLvl = constrain(rms / SOUND_LEVEL_CAP * kPixels, 0, kPixels);
    for (int i = 0; i < kPixels; i++)
        strip.setPixelColor(i, i >= ledLvl ? 0 : (i > 5 ? strip.Color(150, 0, 0) : (i > 3 ? strip.Color(150, 150, 0) : strip.Color(0, 150, 0))));
    strip.show();
}
