#include <Arduino.h>
#include <LittleFS.h>
#include "header/led.h"
#include "header/oled.h"
#include "header/scenery.h"
#include "header/main.h"
#include "header/pinRegistry.h"
#include "header/file.h"

unsigned long buttonDelay = 0;
unsigned long updateResetDelay = 0;
unsigned long pressStart = 0;
unsigned long ledOffDelay = 0;

bool isPressed = false;
bool webUpdate = false;
bool offDelay = false;

namespace BTN {
    void ledRGB(uint8_t r, uint8_t g, uint8_t b) {
        analogWrite(BUTTON_LED_RED_PIN,   255 - r);
        analogWrite(BUTTON_LED_GREEN_PIN, 255 - g);
        analogWrite(BUTTON_LED_BLUE_PIN,  255 - b);
    }

    void ledOff() {
        ledRGB(0, 0, 0);
    }

    void sendUpdate() {
        ledRGB(LED::pwmRed, LED::pwmGreen, LED::pwmBlue);
        webUpdate = true;
        updateResetDelay = Time::currentMillis;
    }

    void tempLED() {
        ledOffDelay = Time::currentMillis;
        offDelay = true;
    }
}

void setupButton() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUTTON_LED_RED_PIN, OUTPUT);
    pinMode(BUTTON_LED_GREEN_PIN, OUTPUT);
    pinMode(BUTTON_LED_BLUE_PIN, OUTPUT);
    buttonDelay = 0;
    updateResetDelay = 0;
    pressStart = 0;
    ledOffDelay = 0;
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
                BTN::tempLED(); 
                break;
            case 2:
                SAVE::applyLastScene();
                BTN::ledRGB(LED::pwmRed, LED::pwmGreen, LED::pwmBlue);
                BTN::tempLED();
                break;
        }
        pressCount = 0;
    }

    if (offDelay && now - ledOffDelay >= 5000) {
        BTN::ledOff();
        ledOffDelay = 0;
        offDelay = false;
    }
}
