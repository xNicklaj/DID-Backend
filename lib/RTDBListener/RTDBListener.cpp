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
    
    if(Firebase.getString(data, "/led/color")){
        String color = data.stringData();
        Serial.println(color);
    }
}