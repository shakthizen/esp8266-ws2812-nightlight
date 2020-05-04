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
void animationChaser(byte speed = 4, byte size = 4)
{
    static byte chaserIndex = 0;
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
void animationMeteor(byte speed = 10, byte fadeSpeed = 1)
{
    static byte currentPixel = 0;
    static int pixelValues[NUM_LEDS];
    EVERY_N_MILLISECONDS(1)
    {
        // Fade In
        if (currentPixel < NUM_LEDS)
        {
            pixelValues[currentPixel] += speed;
            if (pixelValues[currentPixel] >= 255)
            {
                pixelValues[currentPixel] = 255;
                currentPixel++;
            }
        }

        // Fade Out
        byte m = 0;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            if (pixelValues[i] > 0 && i != currentPixel)
            {
                pixelValues[i] -= fadeSpeed;
                if (pixelValues[i] < 0)
                {
                    pixelValues[i] = 0;
                }
            }

            if (pixelValues[i] > m)
            {
                m = pixelValues[i];
            }
        }
        if (m == 0 && currentPixel == NUM_LEDS)
        {
            currentPixel = 0;
        }
    }

    for (int i = 0; i < NUM_LEDS; i++)
    {
        // leds[i].fadeLightBy(255 - pixelValues[i]);
        LED_STRIP[i].nscale8(pixelValues[i]);
    }
}

//================= Random Fade
void animationRandomFade(byte speed = 8, byte fadeSpeed = 2)
{
    static byte currentPixel = 0;
    static int pixelValues[NUM_LEDS];
    EVERY_N_MILLISECONDS(1)
    {
        // Fade In
        if (currentPixel < NUM_LEDS)
        {
            pixelValues[currentPixel] += speed;
            if (pixelValues[currentPixel] >= 255)
            {
                pixelValues[currentPixel] = 255;
                currentPixel = random8(NUM_LEDS);
            }
        }
        else
        {
            currentPixel = 0;
        }

        // Fade Out
        for (int i = 0; i < NUM_LEDS; i++)
        {
            if (pixelValues[i] > 0 && i != currentPixel)
            {
                pixelValues[i] -= fadeSpeed;
                if (pixelValues[i] < 0)
                {
                    pixelValues[i] = 0;
                }
            }
        }
    }

    for (int i = 0; i < NUM_LEDS; i++)
    {
        LED_STRIP[i].nscale8(pixelValues[i]);
    }
}
//================= Sinelon
void animationSinelon(byte speed = 16, byte fadeSpeed = 4)
{
    static byte currentPixel = 0;
    static bool addTo = true;
    static byte pixelValues[NUM_LEDS];
    EVERY_N_MILLISECONDS(1)
    {
        // Fade In
        if (currentPixel < NUM_LEDS)
        {
            if (speed >= 255 - pixelValues[currentPixel])
            {
                pixelValues[currentPixel] = 255;
                if (currentPixel <= 0)
                {
                    addTo = true;
                }
                else if (currentPixel >= NUM_LEDS - 1)
                {
                    addTo = false;
                }
                currentPixel += addTo ? 1 : -1;
            }
            else
            {
                pixelValues[currentPixel] += speed;
            }
        }

        // Fade Out
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            if (i != currentPixel)
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

    for (int i = 0; i < NUM_LEDS; i++)
    {
        LED_STRIP[i].nscale8(pixelValues[i]);
    }
}

//=================== Fire
void animationFire(byte cooling = 4, byte heating = 5)
{
    static byte heat[NUM_LEDS];
    byte cooldown;

    EVERY_N_MILLISECONDS(1)
    {
        // Step 1.  Cool down every cell a little
        for (int i = 0; i < NUM_LEDS; i++)
        {
            // cooldown = random8(0, ((cooling * 10)) + 2);
            cooldown = random8(1, cooling);

            if (cooldown > heat[i])
            {
                heat[i] = 0;
            }
            else
            {
                heat[i] -= cooldown;
            }
        }

        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (int k = NUM_LEDS - 1; k >= 1; k--)
        {
            // heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
            heat[k] = (heat[k] + heat[k - 1] + heat[k - 1]) / 3;
        }

        // Step 3.  Randomly ignite new 'sparks' near the bottom

        byte y = random8(heating);
        if (y > 150 - heat[0])
        {
            heat[0] = 150;
        }
        else
        {
            heat[0] += y;
        }

        // Step 4.  Convert heat to LED colors
        for (int j = 0; j < NUM_LEDS; j++)
        {
            LED_STRIP[j] = HeatColor(heat[j]);
            // LED_STRIP[j].nscale8(heat[j]);
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
        // animationRandomFade();
        animationSinelon();
        break;
    case 6:
        animationFire();
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