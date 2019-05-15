/*
 * https://circuits4you.com/2019/01/25/interfacing-dht11-with-nodemcu-example/
 */

#include <Arduino.h>

#if defined(ESP32)
#include <analogWrite.h>
#endif

#include <Thing.h>
#include <WebThingAdapter.h>

#include <DHTesp.h>


const char* ssid = "";
const char* password = "";

const int sensorPin = 14;
DHTesp dht;

WebThingAdapter* adapter;
const char* sensorTypes[] = {"TemperatureSensor", "MultiLevelSensor", "Sensor", nullptr};
ThingDevice sensor("TempHumiditySensor", "TempHumiditySensor", sensorTypes);
ThingProperty sensorTempProp("temperature", "Temperature", NUMBER, "TemperatureProperty");
ThingProperty sensorHumidityProp("humidity", "Humidity", NUMBER, "HumidityProperty");


void setup(void) {
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

  adapter = new WebThingAdapter("TemperatureSensor", WiFi.localIP());

  sensorTempProp.readOnly = true;
  sensorTempProp.unit = "celsius";
  sensor.addProperty(&sensorTempProp);

  sensorHumidityProp.readOnly = true;
  sensorHumidityProp.unit = "percent";
  sensor.addProperty(&sensorHumidityProp);

  adapter->addDevice(&sensor);
  adapter->begin();

  dht.setup(sensorPin, DHTesp::DHT11);

  Serial.println("started");
}

void loop(void) {
  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  Serial.print(dht.getStatusString());
  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.print("\tTemperature: ");
  Serial.print(temperature, 1);
  Serial.print("\tHeatIndex: ");
  Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
  Serial.print("\n");

  ThingPropertyValue tempVal;
  tempVal.number = temperature;
  sensorTempProp.setValue(tempVal);
  ThingPropertyValue humidityVal;
  humidityVal.number = humidity;
  sensorHumidityProp.setValue(humidityVal);

  adapter->update();

  delay(1000);
}
