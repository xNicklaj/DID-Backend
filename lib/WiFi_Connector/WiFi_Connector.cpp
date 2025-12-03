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
    WiFi.begin(WIFI_SSID, WIFI_PW);

    Serial.printf("Connecting to WiFi '%s' with password '%s' ..", WIFI_SSID, WIFI_PW);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    } 
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI());
}

void WiFi_Connector::update(){

}