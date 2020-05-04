#include <FastLED.h>
#include <EEPROM.h>

#define DATA_PIN D4
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

void changeHueBy(byte v);

byte indexEffect = 0;
byte indexVariant = 0;
byte indexColor = 0; //hue
byte indexAnimation = 0;

//====================================================
// Helpers
//====================================================

void breath()
{
    fadeLightBy(leds, NUM_LEDS, beatsin8(12));
}

byte chaserIndex = 0;
void chaser(byte speed = 4, byte size = 4)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].fadeLightBy(sin8(chaserIndex + i * 255 / NUM_LEDS / 2 * size));
    }

    EVERY_N_MILLISECONDS(1)
    {
        chaserIndex += speed;
    };
}

byte meteorIndex = 0;
int meteorLeds[NUM_LEDS];
void meteor(byte speed = 10, byte fadeSpeed = 1)
{
    EVERY_N_MILLISECONDS(1)
    {
        // Fade In
        if (meteorIndex < NUM_LEDS)
        {
            meteorLeds[meteorIndex] += speed;
            if (meteorLeds[meteorIndex] >= 255)
            {
                meteorLeds[meteorIndex] = 255;
                meteorIndex++;
            }
        }

        // Fade Out
        byte m = 0;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            if (meteorLeds[i] > 0 && i != meteorIndex)
            {
                meteorLeds[i] -= fadeSpeed;
                if (meteorLeds[i] < 0)
                {
                    meteorLeds[i] = 0;
                }
            }

            if (meteorLeds[i] > m)
            {
                m = meteorLeds[i];
            }
        }
        if (m == 0 && meteorIndex == NUM_LEDS)
        {
            meteorIndex = 0;
        }
    }

    for (int i = 0; i < NUM_LEDS; i++)
    {
        // leds[i].fadeLightBy(255 - meteorLeds[i]);
        leds[i].nscale8(meteorLeds[i]);
    }
}

void randomFade(byte speed = 8, byte fadeSpeed = 2)
{
    EVERY_N_MILLISECONDS(1)
    {
        // Fade In
        if (meteorIndex < NUM_LEDS)
        {
            meteorLeds[meteorIndex] += speed;
            if (meteorLeds[meteorIndex] >= 255)
            {
                meteorLeds[meteorIndex] = 255;
                meteorIndex = random8(NUM_LEDS);
            }
        }
        else
        {
            meteorIndex = 0;
        }

        // Fade Out
        for (int i = 0; i < NUM_LEDS; i++)
        {
            if (meteorLeds[i] > 0 && i != meteorIndex)
            {
                meteorLeds[i] -= fadeSpeed;
                if (meteorLeds[i] < 0)
                {
                    meteorLeds[i] = 0;
                }
            }
        }
    }

    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].nscale8(meteorLeds[i]);
    }
}

//====================================================
// Patterns
//====================================================

//==================================================== Static White

void staticWhite()
{
    switch (indexVariant)
    {
    case 0:
        fill_solid(leds, NUM_LEDS, CRGB::White);
        break;
    case 1:
        fill_solid(leds, NUM_LEDS, CRGB(255, 108, 0)); //1500K
        break;
    case 2:
        fill_solid(leds, NUM_LEDS, CRGB(255, 177, 110)); //3000K
        break;
    case 3:
        fill_solid(leds, NUM_LEDS, CRGB::White);
        break;
    case 4:
        fill_solid(leds, NUM_LEDS, CRGB(221, 230, 255)); //8000K
        break;
    case 5:
        fill_solid(leds, NUM_LEDS, CRGB(181, 205, 255)); //15000K
        break;
    default:
        indexVariant = 0;
        break;
    }
}
//==================================================== White Breath

void staticWhiteBreath()
{
    staticWhite();
    breath();
}

//==================================================== White Chaser

void staticWhiteChaser()
{
    staticWhite();
    chaser();
}
//==================================================== White Meteor

void staticWhiteMeteor()
{
    staticWhite();
    meteor();
}

//==================================================== Static Color

void staticColor()
{
    switch (indexVariant)
    {
    case 0:
        fill_solid(leds, NUM_LEDS, CHSV(indexColor, 255, 255));
        break;
    default:
        changeHueBy(8);
        indexVariant = 0;
        break;
    }
}

//==================================================== Static Color Breath

void staticColorBreath()
{
    staticColor();
    breath();
}

//==================================================== Static Color Chaser

void staticColorChaser()
{
    staticColor();
    chaser();
}
//==================================================== Static Color Meteor

void staticColorMeteor()
{
    staticColor();
    meteor();
}

//==================================================== Static Gradient
void staticGradient(byte size = 8)
{
    switch (indexVariant)
    {
    case 0:
        fill_rainbow(leds, NUM_LEDS, indexColor, 255 / NUM_LEDS / size);
        break;
    default:
        changeHueBy(8);
        indexVariant = 0;
        break;
    }
}

//==================================================== Static Gradient Breath
void staticGradientBreath()
{
    staticGradient();
    breath();
}

//==================================================== Static Gradient Chaser
void staticGradientChaser()
{
    staticGradient();
    chaser();
}

//==================================================== Static Gradient Meteor
void staticGradientMeteor()
{
    staticGradient();
    meteor();
}

//==================================================== Animating Gradient
void animatingGradient()
{
    staticGradient();
    EVERY_N_MILLISECONDS(50) { indexColor++; }
}

//==================================================== Animating Gradient Breath
void animatingGradientBreath()
{
    animatingGradient();
    breath();
}

//==================================================== Animating Gradient Chaser
void animatingGradientChaser()
{
    animatingGradient();
    chaser();
}

//==================================================== Animating Gradient Meteor
void animatingGradientMeteor()
{
    animatingGradient();
    meteor();
}

//==================================================== Static Rainbow
void staticRainbow()
{
    switch (indexVariant)
    {
    case 0:
        fill_rainbow(leds, NUM_LEDS, indexColor, 255 / NUM_LEDS);
        break;
    default:
        changeHueBy(8);
        indexVariant = 0;
        break;
    }
}

//==================================================== Static Rainbow Breath
void staticRainbowBreath()
{
    staticRainbow();
    breath();
}

//==================================================== Static Rainbow Chaser
void staticRainbowChaser()
{
    staticRainbow();
    chaser();
}
//==================================================== Static Rainbow Meteor
void staticRainbowMeteor()
{
    staticRainbow();
    meteor();
}

//==================================================== Animating Rainbow
void animatingRainbow()
{
    staticRainbow();
    EVERY_N_MILLISECONDS(1) { indexColor++; }
}

//==================================================== Animating Rainbow Breath
void animatingRainbowBreath()
{
    animatingRainbow();
    breath();
}

//==================================================== Animating Rainbow Chaser
void animatingRainbowChaser()
{
    animatingRainbow();
    chaser();
}

//==================================================== Animating Rainbow Meteor
void animatingRainbowMeteor()
{
    animatingRainbow();
    meteor();
}
//==================================================== Meteor
//==================================================== Meteor
//==================================================== Meteor

void rainbowGlitter()
{
    fill_rainbow(leds, NUM_LEDS, indexColor, 255 / NUM_LEDS);
    byte chances = 20;
    if (random8() < chances)
        leds[random8(NUM_LEDS)] = CRGB::White;
    EVERY_N_MILLISECONDS(1) { indexColor++; }
    // EVERY_N_MILLISECONDS(1) { leds[random8(NUM_LEDS)]=CRGB::White; }
}

//====================================================
//====================================================

void initLeds()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setMaxRefreshRate(120);
    // FastLED.setBrightness(200);
    // FastLED.setDither(DISABLE_DITHER);
    EEPROM.begin(3);
    indexEffect = EEPROM.read(0);
    indexVariant = EEPROM.read(1);
    indexColor = EEPROM.read(2);
}

void playEffect()
{
    switch (indexEffect)
    {
    case 0:
        staticWhite();
        break;
    case 1:
        staticWhiteBreath();
        break;
    case 2:
        staticWhiteChaser();
        break;
    case 3:
        staticWhiteMeteor();
        break;
    case 4:
        staticColor();
        break;
    case 5:
        staticColorBreath();
        break;
    case 6:
        staticColorChaser();
        break;
    case 7:
        staticColorMeteor();
        break;
    case 8:
        staticGradient();
        break;
    case 9:
        staticGradientBreath();
        break;
    case 10:
        staticGradientChaser();
        break;
    case 11:
        staticGradientMeteor();
        break;
    case 12:
        animatingGradient();
        break;
    case 13:
        animatingGradientBreath();
        break;
    case 14:
        animatingGradientChaser();
        break;
    case 15:
        animatingGradientMeteor();
        break;
    case 16:
        staticRainbow();
        break;
    case 17:
        staticRainbowBreath();
        break;
    case 18:
        staticRainbowChaser();
        break;
    case 19:
        staticRainbowMeteor();
        break;
    case 20:
        animatingRainbow();
        break;
    case 21:
        animatingRainbowBreath();
        break;
    case 22:
        animatingRainbowChaser();
        break;
    case 23:
        animatingRainbowMeteor();
        break;

    default:
        indexEffect = 0;
        break;
    }
    FastLED.show();
    // FastLED.delay(1000/120);
}

void saveState()
{
    EEPROM.write(0, indexEffect);
    EEPROM.write(1, indexVariant);
    EEPROM.write(2, indexColor);
    EEPROM.commit();
}

void saveEffectState()
{
    EEPROM.write(0, indexEffect);
    EEPROM.commit();
}

void saveVariantState()
{
    EEPROM.write(1, indexVariant);
    EEPROM.commit();
}

void saveHueState()
{
    EEPROM.write(2, indexColor);
    EEPROM.commit();
}

void nextEffect()
{
    indexEffect++;
    indexVariant = 0;
    saveState();
}

void previousEffect()
{
    indexEffect--;
    indexVariant = 0;
    saveState();
}

void nextVariant()
{
    indexVariant++;
    saveVariantState();
}

void previousVariant()
{
    indexVariant--;
    saveVariantState();
}

void changeHueBy(byte v)
{
    EVERY_N_MILLISECONDS(10)
    {

        indexColor += v;
    }
}