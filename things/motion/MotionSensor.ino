#include "common/Base.h"

#define SENSOR_PIN A0

WebThingAdapter* adapter;
const char* sensorTypes[] = {"MotionSensor", "Sensor", nullptr};
ThingDevice sensor("motion", "MotionDetector", sensorTypes);
ThingProperty sensorMotion("motion detected", "MotionProperty", BOOLEAN, "MotionProperty");

void setup(void) {
  initWifi(sensor.id);

  adapter = new WebThingAdapter("motion-sensor", WiFi.localIP());
  sensorMotion.readOnly = true;
  sensor.addProperty(&sensorMotion);
  adapter->addDevice(&sensor);
  adapter->begin();

  pinMode(SENSOR_PIN, INPUT);
}

void loop(void) {
  bool motionDetected = digitalRead(SENSOR_PIN) == HIGH;

  if (motionDetected) {
    Serial.println("Motion detected!");
  }

  ThingPropertyValue motion;
  motion.boolean = motionDetected;
  sensorMotion.setValue(motion);

  adapter->update();
  delay(500);
}
