#include <Arduino.h>
#include "header/led.h"
#include "header/oled.h"
#include "header/button.h"
#include "header/scenery.h"
#include "EEPROM.h"
#include "header/web.h"
#include <ESP8266WiFi.h>

namespace Time {
    unsigned long currentMillis = millis();
}

void setup() {
    Time::currentMillis = millis();
    delay(1000);
    setupButton();
    setupWeb();
    setupLED();
    // setupOLED();
}

void loop() {
    Time::currentMillis = millis();
    loopScenery();
    while (WiFi.status() != WL_CONNECTED) {
        setupWeb();
    }
    handle();
    readButton();
    // updateOled();
}