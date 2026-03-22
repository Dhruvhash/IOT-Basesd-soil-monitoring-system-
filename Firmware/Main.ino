#include "thingProperties.h"
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// ==== PINS ====
#define DHTPIN D4
#define SOIL_PIN A0
#define PIR_PIN D5
#define RAIN_PIN D6
#define RELAY_PIN D3
#define BUTTON_PIN D7

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ==== GOOGLE SCRIPT URL ====
const char* googleScriptURL = "YOUR_WEB_APP_URL";

// ==== VARIABLES ====
unsigned long prevSend = 0;

void setup() {
  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(RELAY_PIN, HIGH);

  dht.begin();

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
}

void loop() {
  ArduinoCloud.update();

  int soilRaw = analogRead(SOIL_PIN);
  soilMoisture = constrain(100 - map(soilRaw, 0, 1023, 0, 100), 0, 100);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    humidity = h;
    temperature = t;
  }

  rainDetected = !digitalRead(RAIN_PIN);
  motionDetected = digitalRead(PIR_PIN);

  digitalWrite(RELAY_PIN, relayControl ? LOW : HIGH);

  // ==== GOOGLE SHEETS SEND ====
  if (millis() - prevSend > 15000) {
    sendToGoogleSheets();
    prevSend = millis();
  }
}

void sendToGoogleSheets() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(googleScriptURL) +
      "?soil=" + soilMoisture +
      "&temp=" + temperature +
      "&hum=" + humidity +
      "&rain=" + rainDetected +
      "&motion=" + motionDetected;

    http.begin(url);
    http.GET();
    http.end();
  }
}
