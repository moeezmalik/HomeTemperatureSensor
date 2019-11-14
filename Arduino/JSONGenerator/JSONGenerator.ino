#include <ArduinoJson.h>`

void setup() {
  Serial.begin(115200);
  while (!Serial) continue;

  //Allocates 200 RAM to the JSON Document
  StaticJsonDocument<200> doc;

  JsonObject root = doc.to<JsonObject>();

  // Add values in the object
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do root.set<long>("time", 1351824120);
  root["id"] = "123123";
  root["temperature"] = 23;
  root["humidity"] = 47;

  serializeJson(root, Serial);
  Serial.println();

}

void loop() {
}
