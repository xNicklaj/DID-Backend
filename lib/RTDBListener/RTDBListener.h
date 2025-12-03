#ifndef RTDBLISTENER_H
#define RTDBLISTENER_H

#include <Arduino.h>
#include <Worker.h>
#include <FirebaseESP32.h>
#include <WiFiClientSecure.h>
#include <LedController.h>
#include <WiFi_Connector.h>

#define DATABASE_URL "did-lab7-37dfa-default-rtdb.europe-west1.firebasedatabase.app/"

#ifndef DATABASE_API_KEY
#define DATABASE_API_KEY ""
#endif

class RTDBListener : public Worker {
public:
    RTDBListener();
    void setup() override;
    void update() override;
    void setWiFiConnector(WiFi_Connector* wifiConnector);
private:
    bool initialized = false;
    WiFi_Connector* wifi;
    void tryInit();
    FirebaseData data;
    FirebaseAuth auth;
    FirebaseConfig config;
};

#endif
