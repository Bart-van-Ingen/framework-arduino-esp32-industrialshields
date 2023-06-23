// WiFi5 library example
// by Industrial Shields

#include <WiFi5.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200UL);

  if (!WiFi5.begin()) {
    Serial.println("Begin error");

  } else if (!WiFi5.updateFirmware()) {
    Serial.println("Update error");

  } else {
    Serial.print("Waiting firmware...");

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
}
