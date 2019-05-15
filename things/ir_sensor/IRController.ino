#include "common/Base.h"

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>

#define IR_RX_PIN 5
#define IR_TX_PIN 4
IRrecv irRX(IR_RX_PIN);
IRsend irTX(IR_TX_PIN);

WebThingAdapter* adapter;
const char* sensorTypes[] = {"IRController", "Sensor", nullptr};
ThingDevice sensor("infrared", "IRSensor", sensorTypes);
ThingProperty sendProp("send", "BooleanProperty", BOOLEAN, "BooleanProperty");
ThingProperty codeProp("ir code", "NumberProperty", NUMBER, "NumberProperty");
ThingProperty bitsProp("ir bits", "NumberProperty", NUMBER, "NumberProperty");
ThingProperty protoProp("ir protocol", "NumberProperty", NUMBER, "NumberProperty");

void setup() {
  initWifi(sensor.name);

  adapter = new WebThingAdapter("ir-controller", WiFi.localIP());
  sensor.addProperty(&sendProp);
  sensor.addProperty(&codeProp);
  sensor.addProperty(&bitsProp);
  sensor.addProperty(&protoProp);
  adapter->addDevice(&sensor);
  adapter->begin();

  pinMode(IR_TX_PIN, OUTPUT);
  irRX.enableIRIn();
}

void loop() {
  decode_results results;
  ThingPropertyValue sendVal = sendProp.getValue();

  if (sendVal.boolean) {
    long code = codeProp.getValue().number;
    int bits = bitsProp.getValue().number;
    int protocol = protoProp.getValue().number;

    debugMessage("sending", code, bits, protocol);
    sendCode(code, bits, protocol);

    sendVal.boolean = false;
    sendProp.setValue(sendVal);
  }
  else if (irRX.decode(&results)) {
    long code = results.value;
    int bits = results.bits;
    int protocol = results.decode_type;

    if (isValidMessage(code, bits, protocol)) {
      debugMessage("received", code, bits, protocol);

      ThingPropertyValue codeVal;
      ThingPropertyValue bitsVal;
      ThingPropertyValue protVal;
      codeVal.number = code;
      bitsVal.number = bits;
      protVal.number = protocol;
      codeProp.setValue(codeVal);
      bitsProp.setValue(bitsVal);
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
    switch (protocol) {
        case NEC: irTX.sendNEC(code, bits); break;
        case SONY: irTX.sendSony(code, bits); break;
        case RC5: irTX.sendRC5(code, bits); break;
        case RC6: irTX.sendRC6(code, bits); break;
        case DISH: irTX.sendDISH(code, bits); break;
        case PANASONIC: irTX.sendPanasonic(code, bits); break;
        case JVC: irTX.sendJVC(code, bits, 5); break;
        case LG: irTX.sendLG(code, bits, 5); break;
        case LG2: irTX.sendLG2(code, bits, 5); break;
    }
}

void debugMessage(String msg, int code, int bits, int protocol) {
  char format[] = "%s: %ld,%d,%d", buffer[128];
  char msgChar[msg.length()];
  msg.toCharArray(msgChar, msg.length());
  sprintf(buffer, format, msgChar, code, bits, protocol);
  Serial.println(buffer);
}
