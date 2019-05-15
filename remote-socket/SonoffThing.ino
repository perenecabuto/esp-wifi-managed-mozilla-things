#include <Arduino.h>

#if defined(ESP32)
#include <analogWrite.h>
#endif

#include <ESP8266mDNS.h>
#include <Thing.h>
#include <WebThingAdapter.h>


#define SONOFF_RELAY_PIN 12
#define SONOFF_LED_PIN 13

const char* ssid = "";
const char* password = "";

WebThingAdapter* adapter;
const char* sensorTypes[] = {"SmartPlug", "Sensor", nullptr};
ThingDevice device("onoff", "SmartPlug", sensorTypes);
ThingProperty deviceOnProp("on", "on:true,off:false", BOOLEAN, "OnOffProperty");


void setup(void) {
  if (!MDNS.begin(device.name)) {
    Serial.println("Error setting up MDNS responder!");
  }

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
  adapter = new WebThingAdapter("SonoffWifiRelay", WiFi.localIP());

  device.addProperty(&deviceOnProp);
  adapter->addDevice(&device);
  adapter->begin();

  pinMode(SONOFF_RELAY_PIN, OUTPUT);

  Serial.println("started");
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
  delay(1000);
}
