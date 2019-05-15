#include "common/Base.h"

#define SENSOR_PIN A0

WebThingAdapter* adapter;
const char* sensorTypes[] = {"MultiLevelSensor", "Sensor", nullptr};
ThingDevice sensor("moisture", "MoistureSensor", sensorTypes);
ThingProperty sensorValueProp("humidity", "Humidity", NUMBER, "LevelProperty");

void setup(void) {
  initWifi(sensor.name);

  adapter = new WebThingAdapter("MoistureSensor", WiFi.localIP());
  sensorValueProp.readOnly = true;
  sensor.addProperty(&sensorValueProp);
  adapter->addDevice(&sensor);
  adapter->begin();
}

void loop(void) {
  int sensorValue = analogRead(SENSOR_PIN);

  if (sensorValue != sensorValueProp.getValue().number) {
    Serial.print("sensorValue:");
    Serial.print(sensorValue);
    Serial.println("");
  }

  ThingPropertyValue val;
  val.number = sensorValue;
  sensorValueProp.setValue(val);

  adapter->update();
  delay(500);
}
