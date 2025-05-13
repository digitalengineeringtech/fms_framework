#include <Preferences.h>
#include <ArduinoJson.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);
  preferences.begin("config", false);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    StaticJsonDocument<256> doc;

    DeserializationError err = deserializeJson(doc, input);
    if (err) return;

    if (doc.containsKey("read")) {
      StaticJsonDocument<256> out;
      out["device"] = preferences.getUChar("device_id", 0);
      out["nozzles"] = preferences.getUChar("nozzle_cnt", 0);
      JsonArray ids = out.createNestedArray("ids");
      for (int i = 1; i <= 8; i++) {
        String key = "noz" + String(i) + "_id";
        ids.add(preferences.getUChar(key.c_str(), 0));
      }
      serializeJson(out, Serial);
      Serial.println();

    } else if (doc.containsKey("write")) {
      JsonObject w = doc["write"];
      preferences.putUChar("device_id", w["device"]);
      preferences.putUChar("nozzle_cnt", w["nozzles"]);
      JsonArray ids = w["ids"];
      for (int i = 0; i < ids.size(); i++) {
        String key = "noz" + String(i + 1) + "_id";
        preferences.putUChar(key.c_str(), ids[i]);
      }
      Serial.println("{\"status\":\"ok\"}");
    }
  }
}
