// WiFi5 library example
// by Industrial Shields

#include <WiFi5.h>

char info[200];
uint16_t infoLen = sizeof(info) / sizeof(char);

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200UL);

  if (!WiFi5.begin()) {
    Serial.println("Begin error");

  } else if (!WiFi5.getInfo(info, &infoLen)) {
    Serial.println("Info error");

  } else {
    Serial.print("INFO: ");
    Serial.println(info);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
}
