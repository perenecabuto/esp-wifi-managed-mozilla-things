#ifndef _base_h
#define _base_h

#define LARGE_JSON_BUFFERS 1

#if defined(ESP32)
#include <analogWrite.h>
#endif

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>

#include <Thing.h>
#include <WebThingAdapter.h>


void initWifi(String deviceName) {
  Serial.begin(115200);
  AsyncWebServer server(80);
  DNSServer dns;

  AsyncWiFiManager wifiManager(&server, &dns);
  char *ssid;
  String macaddress = WiFi.macAddress();
  macaddress.replace(":", "");
  String hostname = deviceName + "_" + macaddress;
  hostname.toCharArray(ssid, hostname.length());
  wifiManager.autoConnect(ssid);

#if defined(ESP8266)
  if (MDNS.begin(hostname)) {
    Serial.println("MDNS responder started");
  }
#endif
}

#endif
