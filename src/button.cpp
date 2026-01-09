#include <Arduino.h>
#include <LittleFS.h>
#include "header/led.h"
#include "header/oled.h"
#include "header/scenery.h"
#include "header/main.h"
#include "header/pinRegistry.h"

unsigned long buttonDelay = 0;
unsigned long updateResetDelay = 0;
unsigned long pressStart = 0;

bool isPressed = false;
bool webUpdate = false;

namespace BTN {
    inline void ledRGB(uint8_t r, uint8_t g, uint8_t b) {
        analogWrite(BUTTON_LED_RED_PIN,   255 - r);
        analogWrite(BUTTON_LED_GREEN_PIN, 255 - g);
        analogWrite(BUTTON_LED_BLUE_PIN,  255 - b);
    }

    inline void ledOff() {
        ledRGB(0, 0, 0);
    }

    void sendUpdate() {
        ledRGB(LED::pwmRed, LED::pwmGreen, LED::pwmBlue);
        webUpdate = true;
        updateResetDelay = Time::currentMillis;
    }
}

void setupButton() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUTTON_LED_RED_PIN, OUTPUT);
    pinMode(BUTTON_LED_GREEN_PIN, OUTPUT);
    pinMode(BUTTON_LED_BLUE_PIN, OUTPUT);
    BTN::ledOff();
}

uint8_t pressCount = 0;

void readButton() {
    unsigned long now = Time::currentMillis;

    if (webUpdate && now - updateResetDelay >= 1000) {
        webUpdate = false;
        BTN::ledOff();
    }

    bool pressed = digitalRead(BUTTON_PIN) == LOW;

    if (pressed && !isPressed) {
        isPressed = true;
        pressStart = now;
        return;
    }

    // set led while it's pressed
    if (pressed) {
        unsigned long held = now - pressStart;

        if (held >= 10000) {
            BTN::ledRGB(255, 255, 255);
        }
        else if (held >= 5000) {
            BTN::ledRGB(255, 165, 0);
        }
        return;
    }


    // run the logic once
    if (isPressed) {
        unsigned long held = now - pressStart;
        isPressed = false;
        BTN::ledOff();

        if (held >= 10000) {
            ESP.restart();
            return;
        }

        if (held >= 5000) {
            LittleFS.remove("/save.txt");
            return;
        }

        pressCount++;
        buttonDelay = now;
    }

    if (pressCount && now - buttonDelay >= 500) {
        switch (pressCount) {
            case 1:
                setScenery(OFF);
                BTN::ledRGB(255, 0, 0);
                break;
            case 2:
                LED_ColorRGB(0, 0, 0);
                setScenery(FADE);
                BTN::ledRGB(0, 255, 0);
                break;
            case 3:
                LED_ColorRGB(0, 100, 0);
                setScenery(BLINK);
                BTN::ledRGB(0, 0, 255);
                break;
        }
        pressCount = 0;
    }
}
