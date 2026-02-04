#ifndef RTDBCONNECTOR_H
#define RTDBCONNECTOR_H

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

class RTDBConnector : public Worker {
public:
    /// @brief Construct the RTDB connector.
    RTDBConnector();
    /// @brief Initialize the connector (called by the scheduler).
    void setup() override;
    /// @brief Update the connector (called by the scheduler).
    void update() override;
    /// @brief Provide the WiFi connector used to establish RTDB access.
    /// @param wifiConnector Pointer to a WiFi connector instance.
    void setWiFiConnector(WiFi_Connector* wifiConnector);
    /// @brief Push a string value to the RTDB at the given path.
    /// @param path RTDB path.
    /// @param value String value to push.
    void pushString(const String& path, const String& value);
    /// @brief Set a boolean value at the given RTDB path.
    /// @param path RTDB path.
    /// @param value Boolean value to set.
    /// @return True if the operation succeeded.
    bool setBool(const String& path, bool value);
    /// @brief Get a boolean value from the given RTDB path.
    /// @param path RTDB path.
    /// @param outValue Output parameter to receive the value.
    /// @return True if the value was read successfully.
    bool getBool(const String& path, bool& outValue);
private:
    bool initialized = false;
    WiFi_Connector* wifi;
    void tryInit();
    FirebaseData data;
    FirebaseAuth auth;
    FirebaseConfig config;
};

#endif