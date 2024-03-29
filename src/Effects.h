#include <FastLED.h>
#include <EEPROM.h>

#define DATA_PIN 2 //D4
#define BTN 0 //D3
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 30
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
            LED_STRIP[NUM_LEDS - i - 1].nscale8_video(values[i]);
        }
    }
    else
    {
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            LED_STRIP[i].nscale8_video(values[i]);
        }
    }
}

void decreaseBrightnessBy(byte *pixelValues, byte amount = 1, byte minimum = 0)
{
    for (byte i = 0; i < NUM_LEDS; i++)
    {
        if (pixelValues[i] - minimum < amount)
        {
            pixelValues[i] = minimum;
        }
        else
        {
            pixelValues[i] -= amount;
        }
    }
}

void increaseBrightnessBy(byte *pixelValues, byte amount = 1, byte maximum = 255)
{
    for (byte i = 0; i < NUM_LEDS; i++)
    {
        if (maximum - pixelValues[i] < amount)
        {
            pixelValues[i] = maximum;
        }
        else
        {
            pixelValues[i] += amount;
        }
    }
}

byte addUntilMax(byte currentValue, byte amount, byte maximum = 255)
{
    if (currentValue < maximum)
    {
        if (maximum - currentValue <= amount)
        {
            currentValue = maximum;
        }
        else
        {
            currentValue += amount;
        }
    }
    return currentValue;
}

byte subtractUntilMin(byte currentValue, byte amount, byte minimum = 0)
{
    if (currentValue > minimum)
    {
        if (currentValue - minimum <= amount)
        {
            currentValue = minimum;
        }
        else
        {
            currentValue -= amount;
        }
    }
    return currentValue;
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
void animationBreath(byte speed = 12)
{
    nscale8_video(LED_STRIP, NUM_LEDS, beatsin8(speed));
}

//================= Wipe
void animationWipe(byte speed = 1, bool direction = false)
{
    static byte offset = 0;
    static byte pixelValues[NUM_LEDS];
    static bool state = true;

    EVERY_N_MILLISECONDS(1)
    {
        if (state)
        {

            pixelValues[offset] = addUntilMax(pixelValues[offset], speed);
            if (pixelValues[offset] == 255)
            {
                offset++;
            }
        }
        else
        {
            pixelValues[offset] = subtractUntilMin(pixelValues[offset], speed);
            if (pixelValues[offset] == 0)
            {
                offset++;
            }
        }
        if (offset >= NUM_LEDS)
        {
            offset = 0;
            state = !state;
        }
    };

    setBrightness(pixelValues, direction);
}

//================= Chaser
void animationChaser(byte speed = 4, byte size = 4, bool direction = false)
{
    static byte offset = 0;
    static byte pixelValues[NUM_LEDS];

    EVERY_N_MILLISECONDS(1)
    {
        offset -= speed;
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            pixelValues[i] = (sin8(offset + i * 255 / NUM_LEDS / 2 * size));
        }
    };

    setBrightness(pixelValues, direction);
}

//================= Meteor
void animationMeteor(byte speed = 10, byte fadeSpeed = 1, bool direction = false)
{
    static byte offset = 0;
    static byte pixelValues[NUM_LEDS];
    EVERY_N_MILLISECONDS(1)
    {
        // Fade Out
        decreaseBrightnessBy(pixelValues, fadeSpeed);
        // Fade In
        if (offset < NUM_LEDS)
        {
            pixelValues[offset] = addUntilMax(pixelValues[offset], speed);
            if (pixelValues[offset] == 255)
            {
                offset++;
            }
        }

        if (offset == NUM_LEDS && pixelValues[offset - 1] == 0)
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

        // Fade Out
        decreaseBrightnessBy(pixelValues, fadeSpeed);
        // Fade In
        pixelValues[offset] = addUntilMax(pixelValues[offset], speed);
        if (pixelValues[offset] == 255)
        {
            offset = random8(NUM_LEDS);
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
        // Fade Out
        decreaseBrightnessBy(pixelValues, fadeSpeed);

        // Fade In
        if (offset < NUM_LEDS)
        {
            pixelValues[offset] = addUntilMax(pixelValues[offset], speed);
            if (pixelValues[offset] == 255)
            {
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
        }
    }

    setBrightness(pixelValues);
}

//=================== Fire
void animationFire(byte speed = 8, bool colored = false)
{
    static byte pixelValues[NUM_LEDS];

    EVERY_N_MILLISECONDS(1)
    {
        // Fade Out
        for (byte i = 1; i < NUM_LEDS; i++)
        {
            pixelValues[i] = subtractUntilMin(pixelValues[i], 1 + random8(speed));
        }
        pixelValues[0] = subtractUntilMin(pixelValues[0], 1 + random8(speed), 20);

        // Blur
        for (byte k = NUM_LEDS - 1; k >= 1; k--)
        {
            pixelValues[k] = (pixelValues[k] + pixelValues[k - 1] + pixelValues[k - 1]) / random8(3, 4);
        }

        // Fade In
        pixelValues[0] = addUntilMax(pixelValues[0], random8(speed + 2));
    }
    // Apply
    if (colored)
    {
        for (byte i = 0; i < NUM_LEDS; i++)
        {
            byte mapped = map8(pixelValues[i], 0, 160);
            LED_STRIP[i] = HeatColor(mapped);
        }
    }
    else
    {
        setBrightness(pixelValues);
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
void effectAnimatingGradient(byte speed = 1)
{
    effectStaticGradient();
    EVERY_N_MILLISECONDS(50)
    {
        FXColor -= speed;
    }
}

//==================================================== Static Rainbow
void effectStaticRainbow()
{
    fill_rainbow(LED_STRIP, NUM_LEDS, FXColor, 255 / NUM_LEDS);
}

//==================================================== Animating Rainbow
void effectAnimatingRainbow(byte speed = 1)
{
    effectStaticRainbow();
    EVERY_N_MILLISECONDS(10)
    {
        FXColor -= speed;
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

    delay(50);
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
        effectAnimatingGradient(1);
        break;
    case 4:
        effectAnimatingGradient(-1);
        break;
    case 5:
        effectStaticRainbow();
        break;
    case 6:
        effectAnimatingRainbow(1);
        break;
    case 7:
        effectAnimatingRainbow(-1);
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
        animationBreath(12);
        break;
    case 3:
        animationWipe(8, false);
        break;
    case 4:
        animationWipe(8, true);
        break;
    case 5:
        animationWipe(16, false);
        break;
    case 6:
        animationWipe(16, true);
        break;
    case 7:
        animationChaser(4, 4, false);
        break;
    case 8:
        animationChaser(4, 4, true);
        break;
    case 9:
        animationMeteor(12, 1, false);
        break;
    case 10:
        animationMeteor(12, 1, true);
        break;
    case 11:
        animationSinelon(12, 2);
        break;
    case 12:
        animationRandomFade(8, 2);
        break;
    case 13:
        animationFire(8, false);
        break;
    case 14:
        animationFire(8, true);
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
    EEPROM.write(0, FXEffect);
    EEPROM.write(1, FXAnimation);
    EEPROM.write(2, FXColor);
    EEPROM.commit();
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
}

void previousEffect()
{
    FXEffect--;
}

void nextAnimation()
{
    FXAnimation++;
}

void previousAnimation()
{
    FXAnimation--;
}

void changeColorBy(byte v)
{
    EVERY_N_MILLISECONDS(50)
    {
        FXColor += v;
    }
}