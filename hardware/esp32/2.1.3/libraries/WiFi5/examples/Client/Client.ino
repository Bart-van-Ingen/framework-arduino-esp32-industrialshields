// WiFi5 library example
// by Industrial Shields

#include <WiFi5.h>

const char* ssid = "YOUR-WIFI-SSID-HERE";
const char* key = "YOUR-WIFI-PASSWORD-HERE";
const IPAddress serverIp(192, 168, 1, 213);
const uint16_t serverPort = 1234;

WiFi5Client client;

uint32_t lastSendTime = 0;
const int bufferLen = 200;
uint8_t buffer[bufferLen];

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200UL);

  switch (WiFi5.begin(ssid, key)) {
    case WL_NO_SHIELD:
      Serial.println("WiFi error");
      while (true);
      break;

    case WL_NO_SSID_AVAIL:
      Serial.println("No SSID available");
      while (true);
      break;

  }

  Serial.println("WiFi5 example started");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  if (!client.connected()) {
    Serial.println("Connecting to server...");

    if (!client.connect(serverIp, serverPort)) {
      Serial.println("Failed. Wait some time and retry");
      delay(5000);

    } else {
      Serial.println("Client connected");
    }

  } else {
    if (millis() - lastSendTime > 1000) {
      String msg("get-number");
      if (!client.write((uint8_t*) msg.c_str(), msg.length())) {
        Serial.println("Write failed");
        client.stop();
      }

      lastSendTime = millis();
    }

    int rxLen = client.read(buffer, bufferLen);
    if (rxLen < 0) {
      Serial.println("Read failed");
      client.stop();

    } else if (rxLen > 0) {
      Serial.print("RX: ");
      Serial.print(rxLen);
      Serial.print(": ");
      Serial.write(buffer, rxLen);
      Serial.println();

    }
  }
}
