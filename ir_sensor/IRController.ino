#include <Arduino.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <ESP8266mDNS.h>

#include "Thing.h"
#include "WebThingAdapter.h"

const char* ssid = "";
const char* password = "";


WebThingAdapter* adapter;

const char* sensorTypes[] = {"IRController", "Sensor", nullptr};
ThingDevice sensor("infrared", "IRSensor", sensorTypes);
ThingProperty sendProp("send", "BooleanProperty", BOOLEAN, "BooleanProperty");
ThingProperty codeProp("ir code", "NumberProperty", NUMBER, "NumberProperty");
ThingProperty bitsProp("ir bits", "NumberProperty", NUMBER, "NumberProperty");
ThingProperty protoProp("ir protocol", "NumberProperty", NUMBER, "NumberProperty");

#define IR_RX_PIN 5
#define IR_TX_PIN 4

IRrecv irRX(IR_RX_PIN);
IRsend irTX(IR_TX_PIN);


void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
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


  sensor.addProperty(&sendProp);
  sensor.addProperty(&codeProp);
  sensor.addProperty(&bitsProp);
  sensor.addProperty(&protoProp);

  adapter = new WebThingAdapter("ir-controller", WiFi.localIP());
  adapter->addDevice(&sensor);
  adapter->begin();

  pinMode(IR_TX_PIN, OUTPUT);
  irRX.enableIRIn();
}


decode_results results;

void loop() {
  ThingPropertyValue sendVal = sendProp.getValue();

  if (sendVal.boolean) {

    long code = codeProp.getValue().number;
    int bits = bitsProp.getValue().number;
    int protocol = protoProp.getValue().number;

    sendCode(code, bits, protocol);

    sendVal.boolean = false;
    sendProp.setValue(sendVal);

  } else if (irRX.decode(&results)) {

    long code = results.value;
    int bits = results.bits;
    int protocol = results.decode_type;

    if (isValidMessage(code, bits, protocol)) {
      char format[] = "received: %ld,%d,%d", buffer[128];
      sprintf(buffer, format, code, bits, protocol);
      Serial.println(String(buffer));

      ThingPropertyValue codeVal;
      ThingPropertyValue bitsVal;
      ThingPropertyValue protVal;

      codeVal.number = code;
      codeProp.setValue(codeVal);
      bitsVal.number = bits;
      bitsProp.setValue(bitsVal);
      protVal.number = protocol;
      protoProp.setValue(protVal);
    }

    irRX.resume();
  }

  adapter->update();
  delay(500);
}

bool isValidMessage(int code, int bits, int protocol) {
  return code != -1 && bits != 0 && protocol != UNKNOWN && protocol != UNUSED;
}

void sendCode(int code, int bits, int protocol) {
    Serial.print("sending:");
    Serial.print(code);
    Serial.print(",");
    Serial.print(bits);
    Serial.print(",");
    Serial.print(protocol);
    Serial.print("\n");

//    cli();
    switch (protocol) {
        case NEC:
            irTX.sendNEC(code, bits);
            break;
        case SONY:
            irTX.sendSony(code, bits);
            break;
        case RC5:
            irTX.sendRC5(code, bits);
            break;
        case RC6:
            irTX.sendRC6(code, bits);
            break;
        case DISH:
            irTX.sendDISH(code, bits);
            break;
        case PANASONIC:
            irTX.sendPanasonic(code, bits);
            break;
        case JVC:
            irTX.sendJVC(code, bits, 5);
            break;
    }
//    sei();
}
