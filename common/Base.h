#ifndef _base_h
#define _base_h

#include <Arduino.h>

#if defined(ESP32)
#include <analogWrite.h>
#endif

#if defined(ESP8266)
#include <ESP8266mDNS.h>
#endif

#include <Thing.h>
#include <WebThingAdapter.h>


const char* ssid = "";
const char* password = "";


void initWifi(String hostname) {
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

  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up MDNS responder!");
  }
}

#endif
