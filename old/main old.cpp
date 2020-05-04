#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <OneButton.h>
#include <ShaFX.h>

#define LENGTH 10
#define STRIP D4
#define BTN D3

OneButton button(BTN, true);
Adafruit_NeoPixel strip(LENGTH, STRIP, NEO_GRB + NEO_KHZ800);

long randomOffset[LENGTH];

long hue = 0;
int saturation = 255;
int value = 255;
int brightness = 255;

int mode = 2;
int speed = 8;

//=============================================================================================
// Helpers
//=============================================================================================

void resetHue()
{
  if (hue >= 65536)
    hue = 0;
}

void resetBrightness()
{
  if (brightness >= 512)
    brightness = 0;
}

void setBrightness(int v)
{
  v = v >= 512 ? 512 : v;
  v = v < 0 ? 0 : v;

  if (v < 256)
  {
    value = v;
    saturation = 255;
  }
  else
  {
    value = 255;
    saturation = 511 - v;
  }
}
//=============================================================================================
// Patterns
//=============================================================================================

void startSingleColor()
{
  setBrightness(brightness);
  for (int pixel = 0; pixel < LENGTH; pixel++)
  {
    strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(hue, saturation, value)));
  }
  strip.show();
}

void startAnimateSingleColor()
{
  setBrightness(brightness);
  hue += speed;
  for (int pixel = 0; pixel < LENGTH; pixel++)
  {
    strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(hue, saturation, value)));
  }
  strip.show();
}

void startAnimateRainbow()
{
  setBrightness(brightness);
  hue += speed;
  for (int pixel = 0; pixel < LENGTH; pixel++)
  {
    long newHue = hue + (pixel * 65535 / LENGTH);
    newHue = newHue > 65535 ? newHue - 65536 : newHue;
    strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(newHue, saturation, value)));
  }
  strip.show();
}

void startAnimateRandomRainbow()
{
  setBrightness(brightness);
  hue += speed;
  for (int pixel = 0; pixel < LENGTH; pixel++)
  {
    long newHue = hue + randomOffset[pixel];
    newHue = newHue > 65535 ? newHue - 65536 : newHue;
    strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(newHue, saturation, value)));
  }
  strip.show();
}

//=============================================================================================
// Button Events
//=============================================================================================

void btnClicked()
{
  brightness += 16;
}

void btnDoubleClicked()
{
  mode++;
}

void btnPressed()
{
  hue += speed;
}

//=============================================================================================
// Main Program
//=============================================================================================

void setup()
{
  // Serial.begin(9600);
  pinMode(D4, OUTPUT);
  pinMode(D3, INPUT_PULLUP);
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.show();  // Initialize all pixels to 'off'
  button.attachClick(btnClicked);
  button.attachDoubleClick(btnDoubleClicked);
  button.attachDuringLongPress(btnPressed);

  for (int i = 0; i < LENGTH; i++)
  {
    randomOffset[i] = random(65535);
  }
}

void loop()
{
  button.tick();

  resetHue();
  resetBrightness();

  switch (mode)
  {
  case 0:
    startSingleColor();
    break;
  case 1:
    startAnimateSingleColor();
    break;
  case 2:
    startAnimateRainbow();
    break;
  case 3:
    startAnimateRandomRainbow();
    break;
  default:
    mode = 0;
    break;
  }

  // Serial.print(hue);
  // Serial.print("  ");
  // Serial.print(saturation);
  // Serial.print("  ");
  // Serial.print(value);
  // Serial.print("  ");
  // Serial.print(brightness);
  // Serial.println("  ");
}