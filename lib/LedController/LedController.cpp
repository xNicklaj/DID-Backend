#include "LedController.h"

LedController& LedController::getInstance() {
	static LedController instance;
	return instance;
}

LedController::LedController()
	: strip(nPixels, nPin, NEO_GRB + NEO_KHZ800)
{
}

void LedController::begin() {
	strip.begin();
	strip.show();
}

void LedController::setPixelColor(uint16_t idx, uint32_t color) {
	if (idx < nPixels) strip.setPixelColor(idx, color);
}

void LedController::clear() {
	for (uint16_t i = 0; i < nPixels; ++i) strip.setPixelColor(i, 0);
}

void LedController::show() {
	strip.show();
}