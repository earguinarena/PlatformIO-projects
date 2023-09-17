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

// https://www.pushingbox.com/
char DEVID[] = "v9BE6E7CA4D8495F";
char server[] = "http://api.pushingbox.com";


HTTPClient http;
WiFiClient client;

#include "DHTesp.h"

DHTesp dht;
#define PIN_DHT22 4 //GPIO04


// #define PIN_LED 16
#define PIN_RELAY 5

// variables para prender MAX_SEGUNDOS el calentador
#define MAX_CALEFACCION_ON 30//30
#define MAX_CALEFACCION_OFF 300 //300 

// variables para prender MAX_SEGUNDOS el freezer
#define MAX_FREEZER_ON 1800// 30 mins
#define MAX_FREEZER_OFF 300 //5 mis 

#define MAX_TIME_PUSH_WEB 1800 // 1800 segundos, 30 mins

int heat_min_temp = 17;
int heat_max_temp = 22;
int cold_min_temp = 18;
int cold_max_temp = 19;
int time_push_web=0;

boolean rele_on = false;
int time_rele_on = 0;
int time_rele_off = MAX_CALEFACCION_OFF;



void push(float humidity,float temperature);
void relayOn();
void relayOff();
void heatControl(float current_hum, float current_temp);
void coldControl(float current_hum, float current_temp);

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

  // Inicializacion de sensotDHT22
  dht.setup(PIN_DHT22, DHTesp::DHT22);
  
  // pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  relayOff();  
}

void loop() {
  Portal.handleClient();

  float current_hum = dht.getHumidity();
  float current_temp = dht.getTemperature();
  
  Serial.print("Temperatura: ");
  Serial.print(current_temp);
  Serial.println(" *C ");

  //heatControl(current_hum, current_temp);
  coldControl(current_hum, current_temp);

  //Display
  // print_display(current_temp, current_hum, calefaccion_on, time_calefaccion_off, time_calefaccion_on);
  if( time_push_web==0) {
    push(current_hum, current_temp);
    time_push_web=MAX_TIME_PUSH_WEB;
  }
  time_push_web--;
  delay(1000);
}

void heatControl(float current_hum, float current_temp){
 if (!rele_on) {
    if (current_temp < heat_min_temp && time_rele_off >= MAX_CALEFACCION_OFF) {
      relayOn();
      rele_on = true;
      time_rele_on = 0;
      push(current_hum, current_temp);
      time_push_web=MAX_TIME_PUSH_WEB;
    }
    else
      time_rele_off++;
  }
  else {
    if (current_temp > heat_max_temp || time_rele_on == MAX_CALEFACCION_ON) {
      relayOff();
      time_rele_off = 0;
      rele_on = false;
      push(current_hum, current_temp);
      time_push_web=MAX_TIME_PUSH_WEB;
    }
    else
      time_rele_on++;
  }
}

void coldControl(float current_hum, float current_temp){
 if (!rele_on) {
    if (current_temp > cold_max_temp && time_rele_off >= MAX_FREEZER_OFF) {
      relayOn();
      rele_on = true;
      time_rele_on = 0;
      push(current_hum, current_temp);
      time_push_web=MAX_TIME_PUSH_WEB;
    }
    else
      time_rele_off++;
  }
  else {
    if (current_temp < cold_min_temp || time_rele_on == MAX_FREEZER_ON) {
      relayOff();
      time_rele_off = 0;
      rele_on = false;
      push(current_hum, current_temp);
      time_push_web=MAX_TIME_PUSH_WEB;
    }
    else
      time_rele_on++;
  }
}


void push(float humidity,float temperature){
  char url[300];
  char relay[3];
  if(rele_on)
    strcpy(relay, "on");
  else
    strcpy(relay, "off");

  sprintf(url,"%s/pushingbox?devid=%s&humid=%f&temp=%f&staterelay=%s&device=%u",server,DEVID,humidity,temperature,relay,ESP.getChipId());
  Serial.println(url);


  if (http.begin(client, url)) //Iniciar conexión
{
      Serial.print("[HTTP] GET...\n");
      int httpCode = http.GET();  // Realizar petición
 
      if (httpCode > 0) {
         Serial.printf("[HTTP] GET... code: %d\n", httpCode);
 
         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = http.getString();   // Obtener respuesta
            Serial.println(payload);   // Mostrar respuesta por serial
         }
      }
      else {
         Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
   }
   else {
      Serial.printf("[HTTP] Unable to connect\n");
   }
}


void relayOn() {
  digitalWrite(PIN_RELAY, HIGH);
  Serial.println("Relay On");
}

void relayOff() {
  digitalWrite(PIN_RELAY, LOW);
  Serial.println("Relay Off");
}