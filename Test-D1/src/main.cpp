#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#endif

#include <AutoConnect.h>
ESP8266WebServer Server;
AutoConnect      Portal(Server);
void rootPage() {
  char content[] = "Hello, world\nOther";
  Server.send(200, "text/plain", content);
}
void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("HTTP server:" + WiFi.localIP().toString());
  }
}
void loop() {
  Portal.handleClient();
}