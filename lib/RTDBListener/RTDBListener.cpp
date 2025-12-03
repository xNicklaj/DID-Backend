#include "RTDBListener.h"

RTDBListener::RTDBListener() {
}

void RTDBListener::setup() {
    init();

    config.host = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_API_KEY;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("Connesso a Firebase RTDB");
    initialized = true;
}

void RTDBListener::update() {
    if (!initialized) return;
    // Placeholder update loop; real implementation should poll or handle callbacks
    Serial.println("Checking for updates in RTDB...");
    if(Firebase.getString(data, "/led/color")){
        String color = data.stringData();
        Serial.println(color);
        if(color.compareTo("red") == 0) LedController::getInstance().setPixelColor(5, LedController::getInstance().Color(255, 0, 0));
        else if(color.compareTo("green") == 0) LedController::getInstance().setPixelColor(0, LedController::getInstance().Color(0, 255, 0));
        else if(color.compareTo("blue") == 0) LedController::getInstance().setPixelColor(0, LedController::getInstance().Color(0, 0, 255));
        else LedController::getInstance().setPixelColor(0, 0);
    }
}