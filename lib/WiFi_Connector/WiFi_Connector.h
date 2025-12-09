#ifndef WIFICONNECTOR_H
#define WIFICONNECTOR_H

#include <Worker.h>
#include <WiFi.h>
#ifdef WIFI_USE_PEAP
#include <esp_wpa2.h>
#endif

class WiFi_Connector : public Worker{
private:
    enum State { IDLE, CONNECTING, CONNECTED, FAILED } state = IDLE;
    unsigned long connectStart = 0;
    unsigned long connectTimeoutMs = 15000; // default timeout

public:
    void setup() override;
    void update() override;

    // Optional: adjust connect timeout (milliseconds)
    void setConnectTimeout(unsigned long ms) { connectTimeoutMs = ms; }
    bool isConnected() const { return state == CONNECTED && WiFi.status() == WL_CONNECTED; }
    State getState() const { return state; }
};

#endif