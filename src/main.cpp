#include <ESP8266WiFi.h>

#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>

#include <LittleFS.h>

#include <OneButton.h>
#include <Effects.h>

OneButton btn(BTN, true);

const byte DNS_PORT = 53;
const char *ssid = "SSLightString";

IPAddress apIP(192, 168, 11, 1);

AsyncWebServer server(80);

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

String getContentType(String filename)
{
  if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

void initWiFi()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  MDNS.addService("http", "tcp", 80);

  //start debug port
  Serial.print("\n");
  Serial.setDebugOutput(true);
  LittleFS.begin();

  // handle effect controls
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String allStatus = String(FXEffect) + "," + String(FXAnimation) + "," + String(FXColor);
    request->send(200, "text/plain", allStatus);
  });

  server.on("/api/set", HTTP_GET, [](AsyncWebServerRequest *request) {
    byte args = request->args();
    if (args == 2)
    {
      FXEffect = request->arg("fxeffect").toInt();
      FXAnimation = request->arg("fxanimation").toInt();
    }
    else if (args == 1)
    {
      changeColorBy(request->arg("fxcolor").toInt());
    }
    request->send(200, "text/plain", "DONE");
  });

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });

  server.begin();
}

void setup()
{
  initWiFi();
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