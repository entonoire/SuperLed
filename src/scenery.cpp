#include <Arduino.h>
#include "header/led.h"
#include "header/scenery.h"
#include "header/main.h"
// #include "EEPROM.h"
Scenery scenery = OFF;
unsigned long blinkDelay = 0;

void LED_Pattern_Fade() {
    LED::prevR = LED::pwmRed;
    LED::pwmRed += 1;
    LED::pwmRed %= 255;

    LED::prevG = LED::pwmGreen;
    LED::pwmGreen += 1;
    LED::pwmGreen %= 255;

    LED::prevB = LED::pwmBlue;
    LED::pwmBlue += 1;
    LED::pwmBlue %= 255;

    LED_ColorRGB();
    LED::mode = "fade";
}

void LED_OFF() {
    LED_ColorRGB(0, 0, 0);
    LED::mode = "off";
}

void LED_Blink() {
    if ((Time::currentMillis-blinkDelay) >= 1000) {
        blinkDelay = Time::currentMillis;
        if (!(LED::pwmRed > 0 && LED::pwmBlue > 0 && LED::pwmGreen > 0)) {
            LED_ColorRGB(LED::prevR, LED::prevG, LED::prevB);
            return;
        }

        LED_OFF();
        return;
    }

    LED_ColorRGB();
    LED::mode = "blink";
}

void setScenery(Scenery newScenery) {
    scenery = newScenery;
    // EEPROM.write(0, newScenery);
    // EEPROM.write(1, LED::pwmRed);
    // EEPROM.write(2, LED::pwmGreen);
    // EEPROM.write(3, LED::pwmBlue);
}

int getSceneryIndex() {
    return scenery;
}

void loopScenery() {
    switch (scenery)
    {
    case FADE:
        LED_Pattern_Fade();
        break;
    case RGB:
        LED::prevR = LED::pwmRed;
        LED::prevG = LED::pwmGreen;
        LED::prevB = LED::pwmBlue;
        LED_ColorRGB();
        break;
    case BLINK:
        LED_Blink();
        break;
    default:
        LED_OFF();
        break;
    }
}