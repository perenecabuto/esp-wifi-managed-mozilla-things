/*
 * https://circuits4you.com/2019/01/25/interfacing-dht11-with-nodemcu-example/
 */

#include "common/Base.h"

#include <DHTesp.h>

#define SENSOR_PIN 14
DHTesp dht;

WebThingAdapter* adapter;
const char* sensorTypes[] = {"TemperatureSensor", "MultiLevelSensor", "Sensor", nullptr};
ThingDevice sensor("TempHumiditySensor", "TempHumiditySensor", sensorTypes);
ThingProperty sensorTempProp("temperature", "Temperature", INTEGER, "TemperatureProperty");
ThingProperty sensorHumidityProp("humidity", "Humidity", INTEGER, "HumidityProperty");

void setup(void) {
  initWifi(sensor.id);

  adapter = new WebThingAdapter("TemperatureSensor", WiFi.localIP());
  sensorTempProp.readOnly = true;
  sensorTempProp.unit = "celsius";
  sensorHumidityProp.readOnly = true;
  sensorHumidityProp.unit = "percent";
  sensor.addProperty(&sensorTempProp);
  sensor.addProperty(&sensorHumidityProp);
  adapter->addDevice(&sensor);
  adapter->begin();

  dht.setup(SENSOR_PIN, DHTesp::DHT11);
}

void loop(void) {
  delay(dht.getMinimumSamplingPeriod());

  long temperature = dht.getTemperature();
  long humidity = dht.getHumidity();

  Serial.print(dht.getStatusString());
  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.print("\tTemperature: ");
  Serial.print(temperature, 1);
  Serial.print("\tHeatIndex: ");
  Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
  Serial.print("\n");

  ThingPropertyValue tempVal;
  tempVal.integer = temperature;
  sensorTempProp.setValue(tempVal);
  ThingPropertyValue humidityVal;
  humidityVal.integer = humidity;
  sensorHumidityProp.setValue(humidityVal);

  adapter->update();
}
