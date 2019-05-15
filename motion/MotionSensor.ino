#include <Arduino.h>

#if defined(ESP32)
#include <analogWrite.h>
#endif

#include <Thing.h>
#include <WebThingAdapter.h>

const char* ssid = "";
const char* password = "";

#define A3 4
const int sensorPin = A3;

WebThingAdapter* adapter;

const char* sensorTypes[] = {"MotionSensor", "Sensor", nullptr};
ThingDevice sensor("motion", "MotionDetector", sensorTypes);
ThingProperty sensorMotion("motion detected", "MotionProperty", BOOLEAN, "MotionProperty");
ThingProperty sensorData("motion data", "LevelProperty", NUMBER, "LevelProperty");

void setup(void) {
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
  adapter = new WebThingAdapter("led-lamp", WiFi.localIP());

  sensor.addProperty(&sensorMotion);
  sensor.addProperty(&sensorData);
  adapter->addDevice(&sensor);
  adapter->begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  int sensorValue = analogRead(sensorPin);

  bool motionDetected = (sensorValue > 1024);

  ThingPropertyValue motion;
  motion.boolean = motionDetected;
  sensorMotion.setValue(motion);

  ThingPropertyValue val;
  val.number = sensorValue;
  sensorData.setValue(val);

  Serial.print("sensorValue:");
  Serial.print(sensorValue);
  Serial.print(", detected:");
  Serial.print(motionDetected);
  Serial.println("");

  adapter->update();

  delay(1000);
}
