#include "common/Base.h"

#define SENSOR_PIN A0
#define TRIGGER_PIN 4

WebThingAdapter* adapter;
const char* sensorTypes[] = {"MultiLevelSensor", "Sensor", nullptr};
ThingDevice sensor("moisture", "MoistureSensor", sensorTypes);
ThingProperty sensorValueProp("humidity", "Humidity", NUMBER, "LevelProperty");
ThingProperty triggerValueProp("trigger", "Trigger", BOOLEAN, "TriggerProperty");
ThingProperty triggerReleaseTimeMsValueProp("triggerReleaseTimeMs", "Milliseconds", NUMBER, "triggerReleaseTimeMsProperty");



void setup(void) {
  initWifi(sensor.name);

  pinMode(TRIGGER_PIN, OUTPUT);

  adapter = new WebThingAdapter("MoistureSensor", WiFi.localIP());
  sensorValueProp.readOnly = true;
  sensor.addProperty(&sensorValueProp);
  sensor.addProperty(&triggerValueProp);
  sensor.addProperty(&triggerReleaseTimeMsValueProp);
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

  ThingPropertyValue humidity;
  humidity.number = sensorValue;
  sensorValueProp.setValue(humidity);

  int currentInterval = triggerReleaseTimeMsValueProp.getValue().number;
  ThingPropertyValue interval;
  interval.number = min(currentInterval, 5000);
  if (interval.number != currentInterval) {
    triggerReleaseTimeMsValueProp.setValue(interval);
    currentInterval = interval.number;
  }

  ThingPropertyValue triggerValue = triggerValueProp.getValue();
  bool isTriggerOn = triggerValue.boolean;
  if (isTriggerOn) {
    Serial.println("isTriggerOn: " + isTriggerOn ? "Y" : "N");
    digitalWrite(TRIGGER_PIN, HIGH);
    delay(currentInterval);
  }
  triggerValue.boolean = false;
  triggerValueProp.setValue(triggerValue);
  digitalWrite(TRIGGER_PIN, LOW);

  adapter->update();
  delay(500);
}
