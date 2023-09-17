#include <Arduino.h>

#include "DHTesp.h"

DHTesp dht;
#define PIN_DHT22 4 //GPIO04
#define PIN_RELAY 5
#define PIN_LED 16


// variables para prender MAX_SEGUNDOS el calentador
#define MAX_CALEFACCION_ON 30//30
#define MAX_CALEFACCION_OFF 300 //300 

// variables para prender MAX_SEGUNDOS el freezer
#define MAX_FREEZER_ON 1800// 30 mins
#define MAX_FREEZER_OFF 300 //5 mis 


int heat_min_temp = 17;
int heat_max_temp = 22;
int cold_min_temp = 18;
int cold_max_temp = 19;

boolean rele_on = false;
int time_rele_on = 0;
int time_rele_off = MAX_CALEFACCION_OFF;



void relayOn();
void relayOff();
void heatControl(float current_hum, float current_temp);
void coldControl(float current_hum, float current_temp);


void setup() {
   delay(2000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start Serial");

   // Inicializacion de sensotDHT22
  dht.setup(PIN_DHT22, DHTesp::DHT22);
  
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  relayOff();  
}
void loop() {
  float current_hum = dht.getHumidity();
  float current_temp = dht.getTemperature();
  
  Serial.print("Temperatura: ");
  Serial.print(current_temp);
  Serial.println(" *C ");

  //heatControl(current_hum, current_temp);
  coldControl(current_hum, current_temp);

  digitalWrite(PIN_LED, HIGH);
  delay(100);
  digitalWrite(PIN_LED, LOW);
  delay(1000);
}


void heatControl(float current_hum, float current_temp){
 if (!rele_on) {
    if (current_temp < heat_min_temp && time_rele_off >= MAX_CALEFACCION_OFF) {
      relayOn();
      rele_on = true;
      time_rele_on = 0;
    }
    else
      time_rele_off++;
  }
  else {
    if (current_temp > heat_max_temp || time_rele_on == MAX_CALEFACCION_ON) {
      relayOff();
      time_rele_off = 0;
      rele_on = false;
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
    }
    else
      time_rele_off++;
  }
  else {
    if (current_temp < cold_min_temp || time_rele_on == MAX_FREEZER_ON) {
      relayOff();
      time_rele_off = 0;
      rele_on = false;
    }
    else
      time_rele_on++;
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