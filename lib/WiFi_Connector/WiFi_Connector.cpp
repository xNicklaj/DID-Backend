#include "WiFi_Connector.h"

#include <string.h>

// Allow overriding these values via build flags (e.g. -D WIFI_PW=\"pass\" -D WIFI_SSID=\"polito\")
#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif

#ifndef WIFI_PW
#define WIFI_PW ""
#endif

/* #ifndef WIFI_EAP_USERNAME
#define WIFI_EAP_USERNAME "sXXXXXX@studenti.polito.it"
#endif

#ifndef WIFI_EAP_ID
#define WIFI_EAP_ID "sXXXXXX@studenti.polito.it"
#endif */

void WiFi_Connector::setup(){
    WiFi.mode(WIFI_STA);

    // Start a non-blocking connection attempt
    Serial.printf("Connecting to WiFi '%s'\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PW);
    state = CONNECTING;
    connectStart = millis();
}

void WiFi_Connector::update(){
    // Progress connection state without blocking
    if (state == CONNECTING) {
        wl_status_t st = WiFi.status();
        if (st == WL_CONNECTED) {
            state = CONNECTED;
            Serial.println("WiFi connected");
            Serial.println(WiFi.localIP());
            Serial.print("Signal strength: ");
            Serial.println(WiFi.RSSI());
        } else {
            unsigned long now = millis();
            if (now - connectStart >= connectTimeoutMs) {
                state = FAILED;
                Serial.println();
                Serial.println("WiFi connect: timeout");
                // Optionally disconnect to allow a fresh attempt later
                WiFi.disconnect();
            } else {
                // Still trying; print a dot occasionally (every 2s)
                static unsigned long lastDot = 0;
                if (now - lastDot >= 2000) {
                    Serial.print('.');
                    lastDot = now;
                }
            }
        }
    } else if (state == CONNECTED) {
        // keep monitoring connection; react to disconnects
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi lost connection");
            state = IDLE;
        }
    } else if (state == FAILED) {
        // no-op; allow manual retry by calling setup() again or implement auto-retry
    }
}