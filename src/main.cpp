#include <Arduino.h>

#include "hardware/gpio.h"
#include "network/web_server.h"

void setup()
{
    // Serial Monitor
    Serial.begin(115200);

    // Give Serial time to initialize
    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println("       ESP32 4-BIT ALU");
    Serial.println("================================");

    // Initialize physical ALU GPIO
    gpioInit();

    Serial.println("GPIO initialized.");

    // Start Wi-Fi + LittleFS + Web Server
    webServerInit();
}

void loop()
{
    // Process incoming browser requests
    webServerHandle();
}