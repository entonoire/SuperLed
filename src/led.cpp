#include <Arduino.h>
#include "header/oled.h"
// #include "EEPROM.h"
#include "header/scenery.h"
#include "header/led.h"
#include "header/pinRegistry.h"

namespace LED {
    String mode = "off";
    uint8_t pwmRed = 0;
    uint8_t pwmGreen = 0;
    uint8_t pwmBlue = 0;
    uint8_t prevR = 1;
    uint8_t prevG = 1;
    uint8_t prevB = 1;
}

void setupLED() {
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_BLUE_PIN, OUTPUT);
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_BLUE_PIN, LOW);
    LED::mode = "off";

    // if (EEPROM.length() > 0) {
    //     uint8_t value = EEPROM.read(0);
    //     LED_ColorRGB(EEPROM.read(1), EEPROM.read(2), EEPROM.read(3));
    //     setScenery(static_cast<Scenery>(value));
    // }
}

void formatRGBMode() {
    if (OLED::isEnabled() && (LED::prevR != LED::pwmRed || LED::prevG != LED::pwmGreen || LED::prevB != LED::pwmBlue)) {
        char buff[32];
        sprintf(buff, "rgb(%d, %d, %d)", LED::pwmRed, LED::pwmGreen , LED::pwmBlue);
        LED::mode = buff;
    }
}

bool inverted = false;
void LED_ColorRGB(uint8_t r, uint8_t g, uint8_t b) {
    if (inverted) {
        analogWrite(LED_RED_PIN, 255 - r);
        analogWrite(LED_GREEN_PIN, 255 - g);
        analogWrite(LED_BLUE_PIN, 255 - b);
    }
    else {
        analogWrite(LED_RED_PIN, r);
        analogWrite(LED_GREEN_PIN, g);
        analogWrite(LED_BLUE_PIN, b);
    }

    LED::prevR = LED::pwmRed;
    LED::prevG = LED::pwmGreen;
    LED::prevB = LED::pwmBlue;
    LED::pwmRed = r;
    LED::pwmGreen = g;
    LED::pwmBlue = b;

    formatRGBMode();
}

void LED_ColorRGB() {
    if (inverted) {
        analogWrite(LED_RED_PIN, 255 - LED::pwmRed);
        analogWrite(LED_GREEN_PIN, 255 - LED::pwmGreen);
        analogWrite(LED_BLUE_PIN, 255 - LED::pwmBlue);
    }
    else {
        analogWrite(LED_RED_PIN, LED::pwmRed);
        analogWrite(LED_GREEN_PIN, LED::pwmGreen);
        analogWrite(LED_BLUE_PIN, LED::pwmBlue);
    }

    formatRGBMode();
}