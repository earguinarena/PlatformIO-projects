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


#include <display.h>

#include <DHT.h>

#define DHTTYPE DHT22 
#define DHTPIN 27

DHT dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AutoConnect Portal(Server);

void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}

void doStart(){
  Serial.println("Do Start");
  char content[] = "Server Start";
  Server.send(200, "text/plain", content);
}

// void handle_led1on() {
//   LED1status = HIGH;
//   Serial.println("GPIO4 Status: ON");
//   Server.send(200, "text/html", SendHTML(true,LED2status)); 
// }

// void handle_led1off() {
//   LED1status = LOW;
//   Server.println("GPIO4 Status: OFF");
//   Server.send(200, "text/html", SendHTML(false,LED2status)); 
// }

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTTemperature_(PageArgument& args) {
  return readDHTTemperature();
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

String readDHTHumidity_(PageArgument& args) {
 return readDHTHumidity();
}


// const char index_html[] PROGMEM = R"rawliteral(
static const char PROGMEM index_html[] = R"(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">{{TEMPERATURE}}</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">{{HUMIDITY}}</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)";


PageElement rootElement(index_html, { {"TEMPERATURE", readDHTTemperature_}, {"HUMIDITY", readDHTHumidity_} });
PageBuilder root("/temp", {rootElement});


void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  dht.begin();

  Server.on("/start", doStart);
  // Server.on("/led1on", handle_led1on);
  // Server.on("/led1off", handle_led1off);


  root.insert(Server);
  Server.on("/temperature", []() {
    Server.send(200, "text/plain", readDHTTemperature().c_str());
  });

   Server.on("/humidity", []() {
    Server.send(200, "text/plain", readDHTHumidity().c_str());
  });

  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("HTTP server:" + WiFi.localIP().toString());
  }

  Server.onNotFound([](){
    Serial.println("Error URL: "+Server.uri());
  });
  
  // testDisplay(display);
}
void loop() {
  Portal.handleClient();
}

