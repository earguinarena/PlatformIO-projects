#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#endif

#include <AutoConnect.h>

#if defined(ARDUINO_ARCH_ESP8266)
ESP8266WebServer  Server;
#elif defined(ARDUINO_ARCH_ESP32)
WebServer  Server;
#endif

AutoConnect      Portal(Server);
AutoConnectConfig config;

 #define PIN_LED 16
 #define PIN_RELAY 5

void rootPage() {
  char content[] = "Hello, world\nOther";
  Server.send(200, "text/plain", content);
}
void setup() {
  
  config.apid = "AP-"+String(ESP.getChipId())+"-pass:12345678";
  config.psk  = "12345678";
  
  delay(2000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start Serial");
  Server.on("/", rootPage);
  Portal.config(config);
  if (Portal.begin()) {
    Serial.println("HTTP server:" + WiFi.localIP().toString());
  }

// FUNC_GPIO2
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);

}

void loop() {
  Portal.handleClient();
  digitalWrite(PIN_LED, HIGH);
  digitalWrite(PIN_RELAY, HIGH);
  delay(1000);
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_RELAY, LOW);
  delay(1000); 
}