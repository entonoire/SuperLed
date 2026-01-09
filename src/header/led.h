void setupLED();
void LED_ColorRGB(uint8_t r, uint8_t g, uint8_t b);
void LED_ColorRGB();

namespace LED {
    extern String mode;
    extern uint8_t pwmRed;
    extern uint8_t pwmGreen;
    extern uint8_t pwmBlue;
    extern uint8_t prevR;
    extern uint8_t prevG;
    extern uint8_t prevB;
}