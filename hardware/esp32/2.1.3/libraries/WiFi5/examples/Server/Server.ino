// WiFi5 library example
// by Industrial Shields

#include <WiFi5.h>

const char* ssid = "YOUR-WIFI-SSID-HERE";
const char* key = "YOUR-WIFI-PASSWORD-HERE";
const uint16_t serverPort = 1234;

WiFi5Server server(serverPort);
const int bufferLen = 200;
uint8_t buffer[bufferLen];

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200UL);

  if (WiFi5.begin(ssid, key) != WL_CONNECTED) {
    Serial.println("WiFi join error");
    while (true);
  }

  server.begin();

  Serial.println("WiFi5 example started");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  static uint32_t lastCheckTime = millis();
  if (millis() - lastCheckTime > 5000) {
    WiFi5Client client = server.available();
    if (client) {
      int rxLen = client.read(buffer, bufferLen);
      if (rxLen > 0) {
        Serial.print("RX: ");
        Serial.write(buffer, rxLen);

        client.stop();
      }
    }
    lastCheckTime = millis();
  }
}
