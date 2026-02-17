#include "RTDBConnector.h"

RTDBConnector::RTDBConnector() {
}

void RTDBConnector::setup() {
    // Defer full Firebase initialization until WiFi connector is provided and connected.
    initialized = false;
    tryInit();
}

void RTDBConnector::update() {
    // If not yet initialized, try to init (non-blocking) and skip update until ready
    if (!initialized) {
        tryInit();
        return;
    }
}

void RTDBConnector::setWiFiConnector(WiFi_Connector* wifiConnector) {
    wifi = wifiConnector;
    // Try initializing immediately when the connector is assigned
    tryInit();
}

void RTDBConnector::tryInit() {
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

void RTDBConnector::pushString(const String& path, const String& value) {
    if (!initialized) return;
    Firebase.pushString(data, path.c_str(), value);
}

bool RTDBConnector::setBool(const String& path, bool value) {
    if (!initialized) return false;
    return Firebase.setBool(data, path.c_str(), value);
}

bool RTDBConnector::setJSON(const String& path, FirebaseJson& json) {
    if (!initialized) return false;
    return Firebase.setJSON(data, path.c_str(), json);
}

bool RTDBConnector::updateJSON(const String& path, FirebaseJson& json) {
    if (!initialized) return false;
    return Firebase.updateNode(data, path.c_str(), json);
}

bool RTDBConnector::getBool(const String& path, bool& outValue) {
    if (!initialized) return false;
    if (!Firebase.getBool(data, path.c_str())) return false;
    outValue = data.boolData();
    return true;
}

bool RTDBConnector::getInt(const String& path, int& outValue) {
    if (!initialized) return false;
    if (!Firebase.getInt(data, path.c_str())) return false;
    outValue = data.intData();
    return true;
}

bool RTDBConnector::getFloat(const String& path, float& outValue) {
    if (!initialized) return false;
    if (!Firebase.getFloat(data, path.c_str())) return false;
    outValue = data.floatData();
    return true;
}

bool RTDBConnector::isConnected() const {
    return initialized;
}