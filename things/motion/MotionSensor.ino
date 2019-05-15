#include "common/Base.h"

#define SENSOR_PIN A0

WebThingAdapter* adapter;
const char* sensorTypes[] = {"MotionSensor", "Sensor", nullptr};
ThingDevice sensor("motion", "MotionDetector", sensorTypes);
ThingProperty sensorMotion("motion detected", "MotionProperty", BOOLEAN, "MotionProperty");
ThingProperty sensorData("motion data", "LevelProperty", NUMBER, "LevelProperty");

void setup(void) {
  initWifi(sensor.name);

  adapter = new WebThingAdapter("motion-sensor", WiFi.localIP());
  sensorMotion.readOnly = true;
  sensorData.readOnly = true;
  sensor.addProperty(&sensorMotion);
  sensor.addProperty(&sensorData);
  adapter->addDevice(&sensor);
  adapter->begin();

  pinMode(SENSOR_PIN, INPUT);
}

void loop(void) {
  int sensorValue = analogRead(SENSOR_PIN);
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
  delay(500);
}
