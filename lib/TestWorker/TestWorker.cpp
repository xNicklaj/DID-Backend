#include "TestWorker.h"

Adafruit_NeoPixel ledStrip(nPixels, nPin, NEO_GRB + NEO_KHZ800);

void TestWorker::setup(){
    ledStrip.begin();
    ledStrip.show();
}

void TestWorker::update(){
    Serial.println("TestWorker Update Running...");
    ledLvl = (ledLvl + 1) % (nPixels + 1);
    for (int i = 0; i < nPixels; i++)
        ledStrip.setPixelColor(i, i <= ledLvl ? ledStrip.Color(150, 150, 150) : ledStrip.Color(0, 0, 0));
    ledStrip.show();
}