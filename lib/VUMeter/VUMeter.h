#ifndef VU_METER_H
#define VU_METER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

const uint16_t kPixels = 8;
const uint8_t  kPin    = 13; // CHECK YOUR CONNECTIONS !!!

Adafruit_NeoPixel strip(kPixels, kPin, NEO_GRB + NEO_KHZ800);

class VUMeter {
public:
    void setup(){
        strip.begin();
        strip.show();
    }

    void update(int32_t* buffer, size_t size){
        size_t bytesRead = 0;
        int samples = bytesRead / 4;

        // Calcolo livello del segnale
        int64_t sum = 0;
        for (int i = 0; i < samples; i++) {
            sum += abs(buffer[i] << 8);
        }

        float level = sum / samples;

        float rms = sqrt(level / size);

        Serial.printf("RMS: %f\n", rms);

        int ledLvl = rms / 100;
        for (int i = 0; i < kPixels; i++)
            strip.setPixelColor(i, i >= ledLvl ? 0 : (i > 5 ? strip.Color(150, 0, 0) : (i > 3 ? strip.Color(150, 150, 0) : strip.Color(0, 150, 0))));
        strip.show();
    }
};

#endif