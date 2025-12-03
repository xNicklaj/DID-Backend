#include "LedController.h"

LedController& LedController::getInstance() {
	static LedController instance;
	return instance;
}

LedController::LedController()
	: strip(nPixels, nPin, NEO_GRB + NEO_KHZ800)
{
}

void LedController::setup() {
	strip.begin();
	strip.show();
}

void LedController::setPixelColor(uint16_t idx, uint32_t color, bool show) {
	if (idx < nPixels) 
	{
		// apply brightness modifier
		if (brightness <= 0.0f) {
			strip.setPixelColor(idx, 0);
		} else if (brightness >= 0.9999f) {
			strip.setPixelColor(idx, color);
		} else {
			uint8_t r = (color >> 16) & 0xFF;
			uint8_t g = (color >> 8) & 0xFF;
			uint8_t b = color & 0xFF;
			uint8_t rr = (uint8_t)(r * brightness + 0.5f);
			uint8_t gg = (uint8_t)(g * brightness + 0.5f);
			uint8_t bb = (uint8_t)(b * brightness + 0.5f);
			strip.setPixelColor(idx, strip.Color(rr, gg, bb));
		}
	}
	if (show)
		strip.show();
}

void LedController::setAllPixelsColor(uint32_t color) {
	for (uint16_t i = 0; i < nPixels; ++i) 
		LedController::getInstance().setPixelColor(i, color);
	strip.show();
}

void LedController::clear() {
	for (uint16_t i = 0; i < nPixels; ++i) 
		LedController::getInstance().setPixelColor(i, 0);
	strip.show();
}

void LedController::show() {
	
}

void LedController::setBrightness(float b) {
	if (b < 0.0f) b = 0.0f;
	if (b > 1.0f) b = 1.0f;
	brightness = b;
}

float LedController::getBrightness() const {
	return brightness;
}