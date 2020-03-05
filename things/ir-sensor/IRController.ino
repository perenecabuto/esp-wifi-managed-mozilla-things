#define LARGE_JSON_BUFFERS 1

#include "common/Base.h"
#include "StringSplitter.h"

#if defined(ESP8266)
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#else
#include <IRRemote.h>
#endif

#define IR_RX_PIN 5
#define IR_TX_PIN 4

IRrecv irRX(IR_RX_PIN);
IRsend irTX(IR_TX_PIN);

ThingActionObject *sendActionGenerator(DynamicJsonDocument *);
ThingActionObject *replayActionGenerator(DynamicJsonDocument *);
void codeChanged(ThingPropertyValue newValue);

WebThingAdapter* adapter;
const char* sensorTypes[] = {"IRSensor", nullptr};
ThingDevice sensor("infrared", "IR receiver and emmiter", sensorTypes);
ThingProperty codeProp("code", "code", STRING, "StringProperty", codeChanged);

StaticJsonDocument<256> sendInputData;
JsonObject sendInput = sendInputData.to<JsonObject>();
ThingAction sendAction("send", "Send", "Send code", "ToggleAction", &sendInput, sendActionGenerator);
ThingAction replayAction("replay", "replay", "Replay last code", "ToggleAction", nullptr, replayActionGenerator);

String codeToSend = "";

void setup() {
  initWifi(sensor.id);

  sensor.addProperty(&codeProp);

  sendInput["type"] = "object";
  JsonObject sendInputProperties = sendInput.createNestedObject("properties");
  sendInputProperties.createNestedObject("code")["type"] = "string";
  sensor.addAction(&sendAction);
  sensor.addAction(&replayAction);

  adapter = new WebThingAdapter(sensor.id, WiFi.localIP());
  adapter->addDevice(&sensor);
  adapter->begin();

  ThingPropertyValue initialCode = {.string = new String("")};
  codeProp.setValue(initialCode);

  pinMode(IR_TX_PIN, OUTPUT);
  irRX.enableIRIn();
}


void loop() {
  decode_results results;

  adapter->update();

  if (codeToSend != "") {
    String msg = codeToSend;
    codeToSend = "";
    Serial.println("Received code:" + msg);
    StringSplitter *splitter = new StringSplitter(msg, ',', 3);
    if (splitter->getItemCount() != 3) {
      ThingPropertyValue initialCode = {.string = new String("")};
      codeProp.setValue(initialCode);
      Serial.println("invalid code format");
      return;
    }

    int protocol = splitter->getItemAtIndex(0).toInt();
    int bits = splitter->getItemAtIndex(1).toInt();
    long code = splitter->getItemAtIndex(2).toInt();
    sendCode(protocol, bits, code);
    Serial.println("Sent code: " + msg);
  }
  else if (irRX.decode(&results)) {
    long code = results.value;
    int bits = results.bits;
    int protocol = results.decode_type;

    if (isValidCode(protocol, bits, code)) {
      char buffer[128];
      sprintf(buffer, "%d,%d,%ld", protocol, bits, code);
      Serial.print("Scanned code: ");
      Serial.println(buffer);

      ThingPropertyValue codeVal = {.string = new String(buffer)};
      codeProp.setValue(codeVal);
    }

    irRX.resume();
  }
}

bool isValidCode(int protocol, int bits, long code) {
  return code != -1 && bits != 0 && protocol != UNKNOWN && protocol != UNUSED;
}

void sendCode(int protocol, int bits, long code) {
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

ThingActionObject *sendActionGenerator(DynamicJsonDocument *input) {
  return new ThingActionObject("send", input, evalSendCode, nullptr);
}

void evalSendCode(const JsonVariant &input) {
  JsonObject inputObj = input.as<JsonObject>();
  if (!inputObj.containsKey("code")) {
    Serial.println("no code field found");
    return;
  }
  String code = inputObj["code"];
  codeToSend = code;
}

ThingActionObject *replayActionGenerator(DynamicJsonDocument *input) {
  return new ThingActionObject("replay", input, replayCode, nullptr);
}

void replayCode(const JsonVariant &input) {
  String code = *codeProp.getValue().string;
  if (code != "") {
    codeToSend = code;
  }
}

void codeChanged(ThingPropertyValue newValue) {
  codeToSend = *newValue.string;
}