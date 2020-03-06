#include "common/Base.h"

#if defined(ESP8266)
#include <ESP_EEPROM.h>
#else
#include <EEPROM.h>
#endif

#include <math.h>

#define HUMIDITY_VALUE_PIN A0
// #define HUMIDITY_VALUE_MIN 0.0 // 5v
#define HUMIDITY_VALUE_MIN 592.0 // 3.3v
#define HUMIDITY_VALUE_MAX 1023.0
#define DRY_PIN 5
#define TRIGGER_PIN 4
#define RELEASE_TIME_SLOT 0
#define UNDEFINED -1

void changedReleaseTime(ThingPropertyValue);

WebThingAdapter* adapter;
const char* sensorTypes[] = {"MultiLevelSensor", "Sensor", nullptr};
ThingDevice sensor("moisture", "MoistureSensor", sensorTypes);
ThingProperty isDryProp("isdry", "Dry detected", BOOLEAN, "BooleanProperty");
ThingProperty humidityProp("humidity", "Humidity", INTEGER, "LevelProperty");
ThingProperty triggerReleaseTimeProp("triggerReleaseTimeMs", "Milliseconds", INTEGER, "triggerReleaseTimeProp", changedReleaseTime);

ThingActionObject *triggerActionGenerator(DynamicJsonDocument *);
ThingAction triggerAction("trigger", "Activate trigger", "activate trigger", "ToggleAction", nullptr, triggerActionGenerator);

unsigned long long triggeredAt = UNDEFINED;
ThingActionObject *triggerActionGenerator(DynamicJsonDocument *input) {
  int interval = triggerReleaseTimeProp.getValue().integer;
  triggeredAt = millis();
}

void changedReleaseTime(ThingPropertyValue newValue) {
  int interval = min((int)newValue.integer, 5000);
  EEPROM.put(RELEASE_TIME_SLOT, interval);
  EEPROM.commit();
  ThingPropertyValue value = {.integer = interval};
  triggerReleaseTimeProp.setValue(value);

  Serial.print("changedReleaseTime: ");
  Serial.println(interval);
}

void setup(void) {
  wifi_set_sleep_type(LIGHT_SLEEP_T);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(DRY_PIN, INPUT);

  initWifi(sensor.id);

  EEPROM.begin(8);

  isDryProp.readOnly = true;
  humidityProp.readOnly = true;
  humidityProp.unit = "percent";
  triggerReleaseTimeProp.unit = "milliseconds";
  sensor.addProperty(&isDryProp);
  sensor.addProperty(&humidityProp);
  sensor.addProperty(&triggerReleaseTimeProp);
  sensor.addAction(&triggerAction);

  adapter = new WebThingAdapter("MoistureSensor", WiFi.localIP());
  adapter->addDevice(&sensor);
  adapter->begin();

  ThingPropertyValue initialIntervalValue;
  EEPROM.get(RELEASE_TIME_SLOT, initialIntervalValue.integer);
  triggerReleaseTimeProp.setValue(initialIntervalValue);
}

void loop(void) {
  if (triggeredAt != UNDEFINED) {
    digitalWrite(TRIGGER_PIN, HIGH);
    Serial.println("trigger activated");

    int triggerReleaseTime = triggerReleaseTimeProp.getValue().integer;
    int ellapsedTimeSinceTrigger = millis() - triggeredAt;
    if (ellapsedTimeSinceTrigger >= triggerReleaseTime) {
      digitalWrite(TRIGGER_PIN, LOW);
      triggeredAt = UNDEFINED;
      Serial.println("trigger deactivated");
    }
  }

  int sensorValue = analogRead(HUMIDITY_VALUE_PIN);
  int humidityValue = HUMIDITY_VALUE_MAX - sensorValue;
  float humidityRate = max(humidityValue - HUMIDITY_VALUE_MIN, 0.0) / (HUMIDITY_VALUE_MAX - HUMIDITY_VALUE_MIN);
  int humidityPercent = round(humidityRate * 100);
  if (humidityPercent != humidityProp.getValue().integer) {
    Serial.print("sensorValue: "); Serial.print(sensorValue);
    Serial.print(", humidityRate: "); Serial.println(humidityRate);
  }
  ThingPropertyValue humidityVal = {.integer = humidityPercent};
  humidityProp.setValue(humidityVal);

  bool isDry = digitalRead(DRY_PIN) == HIGH;
  if (isDry) {
    Serial.println("Builtin dry detected");
  }
  ThingPropertyValue isDryVal = {.boolean = isDry};
  isDryProp.setValue(isDryVal);

  adapter->update();
  delay(500);
}
