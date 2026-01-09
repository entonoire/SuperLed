#include <Adafruit_SSD1306.h>
void updateOled();
void setupOLED();
extern Adafruit_SSD1306 oled;

namespace OLED {
    void toggle();
    bool isEnabled();
}