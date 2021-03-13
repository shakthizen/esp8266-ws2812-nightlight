#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>

#include <OneButton.h>
#include <Effects.h>

OneButton btn(BTN, true);

const byte DNS_PORT = 53;
const char *ssid = "SSLightString";

IPAddress apIP(192, 168, 11, 1);

DNSServer dnsServer;

ESP8266WebServer server(80);

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
  if (server.hasArg("download"))
    return "application/octet-stream";
  else if (filename.endsWith(".htm"))
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

//Given a file path, look for it in the SPIFFS file storage. Returns true if found, returns false if not found.
bool handleFileRead(String path)
{
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/"))
    path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (LittleFS.exists(pathWithGz) || LittleFS.exists(path))
  {
    if (LittleFS.exists(pathWithGz))
      path += ".gz";
    File file = LittleFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}
void initWiFi()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  //start debug port
  Serial.print("\n");
  Serial.setDebugOutput(true);
  LittleFS.begin();

  // handle effect controls
  server.on("/api/status", []() {
    String allStatus = String(FXEffect) + "," + String(FXAnimation) + "," + String(FXColor);
    server.send(200, "text/html", allStatus);
  });

  server.on("/api/set", []() {
    byte args = server.args();
    if (args == 2)
    {
      FXEffect = server.arg("fxeffect").toInt();
      FXAnimation = server.arg("fxanimation").toInt();
    }
    else if (args == 1)
    {
      changeColorBy(server.arg("fxcolor").toInt());
    }
    server.send(200, "text/html", "DONE");
  });

  //redirect all traffic to index.html
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
    {
      const char *metaRefreshStr = "<head><meta http-equiv=\"refresh\" content=\"0; url=http://192.168.11.1/index.html\" /></head><body><p>redirecting...</p></body>";
      server.send(200, "text/html", metaRefreshStr);
    }
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
  dnsServer.processNextRequest();
  server.handleClient();
  playEffect();
  btn.tick();
}