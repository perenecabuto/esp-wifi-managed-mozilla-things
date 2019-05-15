#include "common/Base.h"

#define SONOFF_RELAY_PIN 12
#define SONOFF_LED_PIN 13

WebThingAdapter* adapter;
const char* sensorTypes[] = {"SmartPlug", "Sensor", nullptr};
ThingDevice device("onoff", "SmartPlug", sensorTypes);
ThingProperty deviceOnProp("on", "on:true,off:false", BOOLEAN, "OnOffProperty");

void setup(void) {
  initWifi(device.name);

  adapter = new WebThingAdapter("SonoffWifiRelay", WiFi.localIP());
  device.addProperty(&deviceOnProp);
  adapter->addDevice(&device);
  adapter->begin();

  pinMode(SONOFF_RELAY_PIN, OUTPUT);
}

void loop(void) {
  int relaySetOn
    = deviceOnProp.getValue().boolean
    ? HIGH : LOW;

  int relayIsOn = digitalRead(SONOFF_RELAY_PIN);

  if (relayIsOn != relaySetOn) {
    digitalWrite(SONOFF_LED_PIN, relaySetOn);
    digitalWrite(SONOFF_RELAY_PIN, relaySetOn);
    Serial.print("Socket is: ");
    Serial.println(relaySetOn ? "on" : "off");
  }

  adapter->update();
  delay(500);
}
