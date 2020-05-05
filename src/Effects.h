#include <FastLED.h>
#include <EEPROM.h>

#define DATA_PIN D4
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 10
CRGB LED_STRIP[NUM_LEDS];

DEFINE_GRADIENT_PALETTE(colorTempPalette){
    0, 255, 80, 0,       //Warmest
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

void setBrightness(byte values[], bool direction = false)
{
    if (direction)
    {
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            LED_STRIP[i].nscale8_video(values[i]);
        }
    }
    else
    {
        for (byte i = NUM_LEDS - 1; i >= 0; i--)
        {
            LED_STRIP[i].nscale8_video(values[i]);
        }
    }
}

//====================================================
// Animations
//====================================================

//================= Glitter
void animationGlitter()
{
    static byte pixelValues[NUM_LEDS];

    for (byte i = 0; i < NUM_LEDS; i++)
    {
        pixelValues[i] = 150;
    }

    if (random8() < 50)
    {
        byte glitterLed = random8(NUM_LEDS);
        pixelValues[glitterLed] = 255;
    }
    setBrightness(pixelValues);
}

//================= Breathing
void animationBreath()
{
    nscale8_video(LED_STRIP, NUM_LEDS, beatsin8(12));
}

//================= Chaser
void animationChaser(byte speed = 4, byte size = 4, bool direction = false)
{
    static byte offset = 0;
    static byte pixelValues[NUM_LEDS];

    EVERY_N_MILLISECONDS(1)
    {
        offset -= speed;
    };

    for (byte i = 0; i < NUM_LEDS; i++)
    {
        pixelValues[i] = (sin8(offset + i * 255 / NUM_LEDS / 2 * size));
    }

    setBrightness(pixelValues, direction);
}

//================= Meteor
void animationMeteor(byte speed = 10, byte fadeSpeed = 1, bool direction = false)
{
    static byte offset = 0;
    static byte pixelValues[NUM_LEDS];
    EVERY_N_MILLISECONDS(1)
    {
        // Fade In
        if (offset < NUM_LEDS)
        {
            if (255 - pixelValues[offset] <= speed)
            {
                pixelValues[offset] = 255;
                offset++;
            }
            else
            {
                pixelValues[offset] += speed;
            }
        }

        // Fade Out
        byte m = 0;
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            if (i != offset)
            {
                if (pixelValues[i] <= fadeSpeed)
                {
                    pixelValues[i] = 0;
                }
                else
                {
                    pixelValues[i] -= fadeSpeed;
                }
            }

            if (pixelValues[i] > m)
            {
                m = pixelValues[i];
            }
        }
        if (m == 0 && offset == NUM_LEDS)
        {
            offset = 0;
        }
    }

    setBrightness(pixelValues, direction);
}

//================= Random Fade
void animationRandomFade(byte speed = 8, byte fadeSpeed = 2)
{
    static byte offset = 0;
    static byte pixelValues[NUM_LEDS];
    EVERY_N_MILLISECONDS(1)
    {
        // Fade In
        if (255 - pixelValues[offset] <= speed)
        {
            pixelValues[offset] = 255;
            offset = random8(NUM_LEDS);
        }
        else
        {
            pixelValues[offset] += speed;
        }

        // Fade Out
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            if (pixelValues[i] > 0 && i != offset)
            {
                if (pixelValues[i] <= fadeSpeed)
                {
                    pixelValues[i] = 0;
                }
                else
                {
                    pixelValues[i] -= fadeSpeed;
                }
            }
        }
    }

    setBrightness(pixelValues);
}
//================= Sinelon
void animationSinelon(byte speed = 16, byte fadeSpeed = 4)
{
    static byte offset = 0;
    static bool direction = true;
    static byte pixelValues[NUM_LEDS];
    EVERY_N_MILLISECONDS(1)
    {
        // Fade In
        if (offset < NUM_LEDS)
        {
            if (speed >= 255 - pixelValues[offset])
            {
                pixelValues[offset] = 255;
                if (offset <= 0)
                {
                    direction = true;
                }
                else if (offset >= NUM_LEDS - 1)
                {
                    direction = false;
                }
                offset += direction ? 1 : -1;
            }
            else
            {
                pixelValues[offset] += speed;
            }
        }

        // Fade Out
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            if (i != offset)
            {
                if (pixelValues[i] <= fadeSpeed)
                {
                    pixelValues[i] = 0;
                }
                else
                {
                    pixelValues[i] -= fadeSpeed;
                }
            }
        }
    }

    setBrightness(pixelValues);
}

//=================== Fire
void animationFire(byte cooling = 4, byte heating = 5, bool colored = false)
{
    static byte pixelValues[NUM_LEDS];
    byte cooldown;

    EVERY_N_MILLISECONDS(1)
    {
        // Step 1.  Cool down every cell a little
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            // cooldown = random8(0, ((cooling * 10)) + 2);
            cooldown = random8(1, cooling);

            if (cooldown > pixelValues[i])
            {
                pixelValues[i] = 0;
            }
            else
            {
                pixelValues[i] -= cooldown;
            }
        }

        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (byte k = NUM_LEDS - 1; k >= 1; k--)
        {
            // heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
            pixelValues[k] = (pixelValues[k] + pixelValues[k - 1] + pixelValues[k - 1]) / 3;
        }

        // Step 3.  Randomly ignite new 'sparks' near the bottom

        byte y = random8(heating);
        if (y > 150 - pixelValues[0])
        {
            pixelValues[0] = 150;
        }
        else
        {
            pixelValues[0] += y;
        }

        // Apply
        if (colored)
        {
            for (byte j = 0; j < NUM_LEDS; j++)
            {
                LED_STRIP[j] = HeatColor(pixelValues[j]);
            }
        }
        else
        {
            setBrightness(pixelValues);
        }
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
void effectAnimatingGradient(bool direction = false)
{
    effectStaticGradient();
    EVERY_N_MILLISECONDS(50)
    {
        if (direction)
        {
            FXColor--;
        }

        else
        {
            FXColor++;
        }
    }
}

//==================================================== Static Rainbow
void effectStaticRainbow()
{
    fill_rainbow(LED_STRIP, NUM_LEDS, FXColor, 255 / NUM_LEDS);
}

//==================================================== Animating Rainbow
void effectAnimatingRainbow(bool direction = false)
{
    effectStaticRainbow();
    EVERY_N_MILLISECONDS(1)
    {
        if (direction)
        {
            FXColor--;
        }

        else
        {
            FXColor++;
        }
    }
}

//==================================================== End

//====================================================
//====================================================

void initLeds()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(LED_STRIP, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setMaxRefreshRate(120);

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
        effectAnimatingGradient(false);
        break;
    case 4:
        effectAnimatingGradient(true);
        break;
    case 5:
        effectStaticRainbow();
        break;
    case 6:
        effectAnimatingRainbow(false);
        break;
    case 7:
        effectAnimatingRainbow(true);
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
        animationChaser(4, 4, false);
        break;
    case 4:
        animationChaser(4, 4, true);
        break;
    case 5:
        animationMeteor(10, 1, false);
        break;
    case 6:
        animationMeteor(10, 1, true);
        break;
    case 7:
        animationRandomFade();
        break;
    case 8:
        animationSinelon();
        break;
    case 9:
        animationFire(4, 5, false);
        break;
    case 10:
        animationFire(4, 5, true);
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