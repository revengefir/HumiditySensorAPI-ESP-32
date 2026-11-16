#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#define DHTPin 21
#define DHTType DHT11

const char* ssid = "";
const char* password = "";
float humidity;
float temperature;
unsigned long measureDelay = 3000;
unsigned long lastTimeRan;

StaticJsonDocument<1024> jsonDocument;
char buffer[1024];

DHT dht_sensor(DHTPin,DHTType);
WebServer server(80);

void createJson(char *name, float value, char *unit){
  jsonDocument.clear();
  jsonDocument["name"] = name;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}

void addJsonObject(char *name, float value, char *unit){
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = value;
  obj["unit"] = unit;
}

void getValues(){
  Serial.println("Getting values...");
  jsonDocument.clear();
  addJsonObject("temperature", temperature, "C");
  addJsonObject("humidity", humidity, "%");

  serializeJson(jsonDocument,buffer);
  server.send(200, "applications/json", buffer);
}

void setupApi(){
  server.on("/getValues", getValues);

  server.begin();
}

void setup() {
  Serial.begin(9600);
  dht_sensor.begin();
  Serial.println();

  Serial.print("Connecting to network: ");
  Serial.print(ssid);
  WiFi.begin(ssid,password);
  Serial.println();

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  Serial.println("WiFi connected!");
  Serial.println("Your IP: ");
  Serial.println(WiFi.localIP());

  setupApi();
}

void loop() {
  server.handleClient();
  if (millis() > lastTimeRan + measureDelay){
    humidity = dht_sensor.readHumidity();
    temperature = dht_sensor.readTemperature();
    if (isnan(temperature) || isnan(humidity)){
      Serial.println("Reading failed!");
    }

    lastTimeRan = millis();
  }
}
