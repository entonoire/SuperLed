void LED_Pattern_Fade();
void loopScenery();

enum Scenery {
    RGB,
    FADE,
    OFF,
    BLINK
};
void setScenery(Scenery scenery);
int getSceneryIndex();