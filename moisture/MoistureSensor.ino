#include <Arduino.h>

#if defined(ESP32)
#include <analogWrite.h>
#endif

#include <ESP8266mDNS.h>

#include <Thing.h>
#include <WebThingAdapter.h>


const char* ssid = "";
const char* password = "";

const int sensorPin = A0;


WebThingAdapter* adapter;
const char* sensorTypes[] = {"MultiLevelSensor", "Sensor", nullptr};
ThingDevice sensor("moisture", "MoistureSensor", sensorTypes);
ThingProperty sensorValueProp("humidity", "Humidity", NUMBER, "LevelProperty");


void setup(void) {
  MDNS.begin("moisture-sensor");

  pinMode(sensorPin, INPUT);

  Serial.begin(115200);
#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  adapter = new WebThingAdapter("MoistureSensor", WiFi.localIP());

  sensor.addProperty(&sensorValueProp);
  sensorValueProp.readOnly = true;
  adapter->addDevice(&sensor);
  adapter->begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  int sensorValue = analogRead(sensorPin);

  if (sensorValue != sensorValueProp.getValue().number) {
    Serial.print("sensorValue:");
    Serial.print(sensorValue);
    Serial.println("");
  }

  ThingPropertyValue val;
  val.number = sensorValue;
  sensorValueProp.setValue(val);

  adapter->update();

  delay(1000);
}
