#include <FastLED.h>
#include <EEPROM.h>

#define DATA_PIN D4
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 10
CRGB LED_STRIP[NUM_LEDS];

DEFINE_GRADIENT_PALETTE(colorTempPalette){
    0, 255, 80, 0,      //Warmest
    180, 255, 255, 255,  //White
    255, 181, 205, 255}; //Bluest
CRGBPalette16 ColorTemperaturePalette = colorTempPalette;

void changeColorBy(byte v);

byte FXEffect = 0;
byte FXColor = 0; //hue / color
byte FXAnimation = 0;

//====================================================
// Helpers
//====================================================
CRGB kelvinToRGB(byte temperature)
{
    return ColorFromPalette(ColorTemperaturePalette, temperature);
}
//====================================================
// Animations
//====================================================

//================= Glitter
void animationGlitter()
{
    if (random8() < 50)
    {
        byte glitterLed = random8(NUM_LEDS);
        for (int i = 0; i < NUM_LEDS; i++)
        {
            if (i != glitterLed)
            {
                LED_STRIP[i].fadeLightBy(100);
            }
        }
    }
    else
    {
        fadeLightBy(LED_STRIP, NUM_LEDS, 100);
    }
}

//================= Breathing
void animationBreath()
{
    fadeLightBy(LED_STRIP, NUM_LEDS, beatsin8(12));
}

//================= Chaser
byte chaserIndex = 0;
void animationChaser(byte speed = 4, byte size = 4)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        LED_STRIP[i].fadeLightBy(sin8(chaserIndex + i * 255 / NUM_LEDS / 2 * size));
    }

    EVERY_N_MILLISECONDS(1)
    {
        chaserIndex += speed;
    };
}

//================= Meteor
byte meteorIndex = 0;
int meteorLeds[NUM_LEDS];
void animationMeteor(byte speed = 10, byte fadeSpeed = 1)
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
        LED_STRIP[i].nscale8(meteorLeds[i]);
    }
}

//================= Random Fade
void animationRandomFade(byte speed = 8, byte fadeSpeed = 2)
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
        LED_STRIP[i].nscale8(meteorLeds[i]);
    }
}

//====================================================
// Patterns
//====================================================

//==================================================== Static White

void effectStaticWhite()
{
    fill_solid(LED_STRIP, NUM_LEDS, kelvinToRGB(FXColor));
}

//==================================================== Static Color

void effectStaticColor()
{
    fill_solid(LED_STRIP, NUM_LEDS, CHSV(FXColor, 255, 255));
}

//==================================================== Static Gradient
void effectStaticGradient(byte size = 8)
{
    fill_rainbow(LED_STRIP, NUM_LEDS, FXColor, 255 / NUM_LEDS / size);
}

//==================================================== Animating Gradient
void effectAnimatingGradient()
{
    effectStaticGradient();
    EVERY_N_MILLISECONDS(50) { FXColor++; }
}

//==================================================== Static Rainbow
void effectStaticRainbow()
{
    fill_rainbow(LED_STRIP, NUM_LEDS, FXColor, 255 / NUM_LEDS);
}

//==================================================== Animating Rainbow
void effectAnimatingRainbow()
{
    effectStaticRainbow();
    EVERY_N_MILLISECONDS(1) { FXColor++; }
}

//==================================================== End

//====================================================
//====================================================

void initLeds()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(LED_STRIP, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setMaxRefreshRate(120);
    // FastLED.setBrightness(200);
    // FastLED.setDither(DISABLE_DITHER);
    EEPROM.begin(3);
    FXEffect = EEPROM.read(0);
    FXAnimation = EEPROM.read(1);
    FXColor = EEPROM.read(2);
}

void selectFXEffect()
{
    switch (FXEffect)
    {
    case 0:
        effectStaticWhite();
        break;
    case 1:
        effectStaticColor();
        break;
    case 2:
        effectStaticGradient();
        break;
    case 3:
        effectAnimatingGradient();
        break;
    case 4:
        effectStaticRainbow();
        break;
    case 5:
        effectAnimatingRainbow();
        break;

    default:
        FXEffect = 0;
        break;
    }
}

void selectFXAnimation()
{
    switch (FXAnimation)
    {
    case 0: // No Animation
        break;
    case 1:
        animationGlitter();
        break;
    case 2:
        animationBreath();
        break;
    case 3:
        animationChaser();
        break;
    case 4:
        animationMeteor();
        break;
    case 5:
        animationRandomFade();
        break;

    default:
        FXAnimation = 0;
        break;
    }
}

//===============================================================

void saveEffectState()
{
    EEPROM.write(0, FXEffect);
    EEPROM.commit();
}

void saveAnimationState()
{
    EEPROM.write(1, FXAnimation);
    EEPROM.commit();
}

void saveColorState()
{
    EEPROM.write(2, FXColor);
    EEPROM.commit();
}

void saveState()
{
    saveEffectState();
    saveAnimationState();
    saveColorState();
    // EEPROM.commit();
}

//===============================================================

void playEffect()
{
    selectFXEffect();
    selectFXAnimation();

    EVERY_N_SECONDS(5)
    {
        saveState();
    }

    FastLED.show();
}

void nextEffect()
{
    FXEffect++;
    // saveEffectState();
}

void previousEffect()
{
    FXEffect--;
    // saveEffectState();
}

void nextAnimation()
{
    FXAnimation++;
    // saveAnimationState();
}

void previousAnimation()
{
    FXAnimation--;
    // saveAnimationState();
}

void changeColorBy(byte v)
{
    EVERY_N_MILLISECONDS(50)
    {
        FXColor += v;
    }
}