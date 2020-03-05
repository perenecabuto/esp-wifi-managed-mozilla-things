#include "common/Base.h"

#define SENSOR_PIN 2

WebThingAdapter* adapter;
const char* sensorTypes[] = {"DoorSensor", "Sensor", nullptr};
ThingDevice sensor("door-sensor", "DoorSensor", sensorTypes);
ThingProperty sensorMotion("open", "MotionProperty", BOOLEAN, "OpenProperty");

void setup(void) {
  initWifi(sensor.id);

  adapter = new WebThingAdapter("door-sensor", WiFi.localIP());
  sensorMotion.readOnly = true;
  sensor.addProperty(&sensorMotion);
  adapter->addDevice(&sensor);
  adapter->begin();

  setPullDownPin(SENSOR_PIN);
}

void loop(void) {
  int doorIsOpen = digitalRead(SENSOR_PIN) == LOW;
  Serial.println(doorIsOpen ? "door open" : "door closed");

  ThingPropertyValue motion;
  motion.boolean = doorIsOpen;
  sensorMotion.setValue(motion);

  setPullDownPin(SENSOR_PIN);

  adapter->update();
  delay(500);
}

void setPullDownPin(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
}
