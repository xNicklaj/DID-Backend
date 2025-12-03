#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

const uint16_t nPixels = 8;
const uint8_t  nPin    = 13; // CHECK YOUR CONNECTIONS !!!

class LedController {
public:
	static LedController& getInstance();

	void begin();
	void setPixelColor(uint16_t idx, uint32_t color);
	void clear();
	void show();

	uint32_t Color(uint8_t r, uint8_t g, uint8_t b) { return strip.Color(r, g, b); }
	uint16_t numPixels() const { return nPixels; }

private:
	LedController();
	LedController(const LedController&) = delete;
	LedController& operator=(const LedController&) = delete;

	Adafruit_NeoPixel strip;
};

#endif