#include <Worker.h>
#include <WiFi.h>

class WiFi_Connector : public Worker{
private:

public:
    void setup() override;
    void update() override;
};