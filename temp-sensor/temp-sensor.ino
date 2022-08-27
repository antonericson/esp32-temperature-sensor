#include <Wire.h>
#include <HTTPClient.h>
#include "WiFi.h"
#include "Adafruit_SHT31.h"

// SENSOR
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// WIFI
const char* ssid = "";
const char* password = "";

// API
const char* URL = "http://192.168.1.4:8081/sensordata";
const char* HTTP_METHOD = "POST";

unsigned long lastTime = 0;
unsigned long timerDelay = 600000; // 10 min
//unsigned long timerDelay = 5000; // 5 seconds for debugging

void setup()
{
    Serial.begin(115200);
    // Init sensor
    if (! sht31.begin(0x44)) {
      Serial.println("Couldn't find SHT31");
      while (1) delay(1);
    }

    initWiFi();

    Serial.println("Setup done");
}

void loop()
{
  if ((millis() - lastTime) > timerDelay) {
    float temperature = sht31.readTemperature();
    float humidity = sht31.readHumidity();
    
    // Double check WiFi status
    if (WiFi.status() == WL_CONNECTED && !isnan(temperature) && !isnan(humidity)) {
      WiFiClient client;
      HTTPClient http;

      http.begin(client, URL);

      http.addHeader("uuidKey", "");
      http.addHeader("Content-Type", "application/json");
      String httpRequestData ="{\"temperature\":\"" + String(temperature, 2) + "\",\"humidity\":\"" + String(humidity, 2) + "\",\"sensorId\":\"Sensor01\"}";

      // Send request
      int httpResponseCode = http.POST(httpRequestData);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("");
  Serial.print("Wifi connected: ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}
