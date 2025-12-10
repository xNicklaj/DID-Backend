#include "RTDBListener.h"

RTDBListener::RTDBListener() {
}

void RTDBListener::setup() {
    // Defer full Firebase initialization until WiFi connector is provided and connected.
    initialized = false;
    tryInit();
}

void RTDBListener::update() {
    // If not yet initialized, try to init (non-blocking) and skip update until ready
    if (!initialized) {
        tryInit();
        return;
    }

    if(Firebase.getBool(data, "/led/enabled")){
        bool enabled = data.boolData();
        if(!enabled){
            LedController::getInstance().clear();
            return;
        }
    }

    if(Firebase.getString(data, "/led/color")){
        String color = data.stringData();
        color.trim();

        // Accept formats: "#RRGGBB", "RRGGBB", "0xRRGGBB"
        String s = color;
        if (s.startsWith("#")) s = s.substring(1);
        if (s.startsWith("0x") || s.startsWith("0X")) s = s.substring(2);

        if (s.length() == 6) {
            // parse hex
            long val = strtol(s.c_str(), NULL, 16);
            uint8_t r = (val >> 16) & 0xFF;
            uint8_t g = (val >> 8) & 0xFF;
            uint8_t b = val & 0xFF;
            LedController::getInstance().setAllPixelsColor(LedController::getInstance().Color(r, g, b));
        } else {
            // fallback to named colors (case-insensitive)
            String lc = color;
            lc.toLowerCase();
            if(lc == "red") LedController::getInstance().setAllPixelsColor(LedController::getInstance().Color(255, 0, 0));
            else if(lc == "green") LedController::getInstance().setAllPixelsColor(LedController::getInstance().Color(0, 255, 0));
            else if(lc == "blue") LedController::getInstance().setAllPixelsColor(LedController::getInstance().Color(0, 0, 255));
            else LedController::getInstance().setAllPixelsColor(0);
        }
    }

    if(Firebase.getFloat(data, "/led/brightness")){
        float brightness = data.floatData();
        brightness = constrain(brightness, 0.0, 1.0);
        LedController::getInstance().setBrightness(brightness);
    }
}

void RTDBListener::setWiFiConnector(WiFi_Connector* wifiConnector) {
    wifi = wifiConnector;
    // Try initializing immediately when the connector is assigned
    tryInit();
}

void RTDBListener::tryInit() {
    if (initialized) return;
    if (wifi == nullptr) {
        // waiting for wifi connector to be provided
        return;
    }
    if (!wifi->isConnected()) {
        // waiting for wifi connection
        return;
    }

    // perform Firebase initialization now that WiFi is ready
    init();
    config.host = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_API_KEY;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("Connesso a Firebase RTDB");
    initialized = true;
}

void RTDBListener::pushString(const String& path, const String& value) {
    if (!initialized) return;
    Firebase.pushString(data, path.c_str(), value);
}