#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "header/led.h"
#include "header/main.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 32
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

unsigned long screenDelay;
bool _isEnabled = true;
void setupOLED() {
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0)) {
        Serial.println(F("SSD1306 allocation failed"));
        while (true);
    }
    screenDelay = 0;
    delay(2000);
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.display();
}

void updateOled() {
    if (!_isEnabled) return;
    if ((Time::currentMillis-screenDelay) < 500) return;
    if (LED::prevR == LED::pwmRed && LED::prevG == LED::pwmGreen && LED::prevB == LED::pwmBlue) return;

    screenDelay = Time::currentMillis;
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("R: ");
    oled.fillRect(18, 1, map(LED::pwmRed, 0, 255, 0, 110), 4, WHITE);

    oled.println("G: ");
    oled.fillRect(18, 9, map(LED::pwmGreen, 0, 255, 0, 110), 4, WHITE);

    oled.println("B: ");
    oled.fillRect(18, 17, map(LED::pwmBlue, 0, 255, 0, 110), 4, WHITE);
    
    oled.print("Mode: ");
    oled.println(LED::mode);
    oled.display();
}

namespace OLED {
    bool isEnabled() {
        return _isEnabled;
    }

    void toggle() {
        _isEnabled = !_isEnabled;
        oled.clearDisplay();
        oled.display();
    }
}