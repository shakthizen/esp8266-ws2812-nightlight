#include <ESP8266WiFi.h>
#include <OneButton.h>
#include <Effects.h>

OneButton btn(BTN, true);

void handleDoubleClick()
{
  nextEffect();
}

void handleClick()
{
  nextAnimation();
}

void handleDuringLongpress()
{
  changeColorBy(1);
}

void handleLongpressStop()
{
  saveColorState();
}

void setup()
{
  WiFi.mode(WIFI_OFF);
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
  WiFi.forceSleepBegin();
  WiFi.mode(WIFI_AP);
  

  btn.attachClick(handleClick);
  btn.attachDoubleClick(handleDoubleClick);
  btn.attachDuringLongPress(handleDuringLongpress);
  btn.attachLongPressStop(handleLongpressStop);
  initLeds();
}

void loop()
{
  playEffect();
  btn.tick();
}