#include <FastLED.h>

#define DATA_PIN D4
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

byte hue = 0;

byte mode = 0;

void initLeds()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    // FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
    // FastLED.setBrightness(50);
    FastLED.setMaxRefreshRate(120);
}

void refreshLeds()
{
    FastLED.show();
    // FastLED.delay(1000 / 120); //120fps
}

//====================================================
// Patterns
//====================================================

//==================================================== Static Colors
//==================================================== Breathing Static Colors

void rainbow()
{
    fill_rainbow(leds, NUM_LEDS, hue, 255 / NUM_LEDS);
    EVERY_N_MILLISECONDS(1) { hue++; }
}

void rainbowBig()
{
    fill_rainbow(leds, NUM_LEDS, hue, 4);
    EVERY_N_MILLISECONDS(50) { hue++; }
}

void rainbowGlitter()
{
    fill_rainbow(leds, NUM_LEDS, hue, 255 / NUM_LEDS);
    byte chances = 20;
    if (random8() < chances)
        leds[random8(NUM_LEDS)] = CRGB::White;
    EVERY_N_MILLISECONDS(1) { hue++; }
    // EVERY_N_MILLISECONDS(1) { leds[random8(NUM_LEDS)]=CRGB::White; }
}

void rainbowBreath()
{
    rainbow();
    fadeLightBy(leds, NUM_LEDS, beatsin8(12));
}

void staticColor()
{
    fill_solid(leds, NUM_LEDS, CRGB::White);
}

void staticColorBreath()
{
    fill_solid(leds, NUM_LEDS, CRGB::White);
    fadeLightBy(leds, NUM_LEDS, beatsin8(12));
}

void changeColorSlow()
{
    fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
    EVERY_N_MILLISECONDS(100) { hue++; };
}

void changeColorFast()
{
    fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
    EVERY_N_MILLISECONDS(10) { hue++; };
}

void changeColorSlowBreath()
{
    changeColorSlow();
    fadeLightBy(leds, NUM_LEDS, beatsin8(12));
}

void changeColorFastBreath()
{
    changeColorFast();
    fadeLightBy(leds, NUM_LEDS, beatsin8(12));
}

void randomColorBreath()
{
    int brightness = beatsin8(12);
    fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
    fadeLightBy(leds, NUM_LEDS, 255 - brightness);

    if (brightness <= 1)
    {
        hue = random8();
    }
}

int randomColorChaserIndex = 0;
void randomColorChaser()
{
    EVERY_N_MILLISECONDS(20)
    {
        leds[randomColorChaserIndex] = CHSV(hue, 255, 255);

        randomColorChaserIndex++;
        if (randomColorChaserIndex >= NUM_LEDS)
        {
            randomColorChaserIndex = 0;
            hue += 10;
        }
    };

    // EVERY_N_MILLISECONDS(1)
    // {
    // };
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].fadeLightBy(20);
    }
}

//====================================================
//====================================================

typedef void (*Effects[])();
Effects effects = {staticColor, staticColorBreath, changeColorSlow, changeColorFast, changeColorSlowBreath, changeColorFastBreath, randomColorBreath, rainbow, rainbowBig, rainbowGlitter, rainbowBreath, randomColorChaser};

void nextEffect(){
  mode++;
  if (mode >= sizeof(effects) / sizeof(effects[0]))
    mode = 0;
}