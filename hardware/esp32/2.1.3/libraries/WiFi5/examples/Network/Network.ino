// WiFi5 library example
// by Industrial Shields

#include <WiFi5.h>

const char* ssid = "YOUR-WIFI-SSID-HERE";
const char* key = "YOUR-WIFI-PASSWORD-HERE";
uint8_t mac[6];

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200UL);
  Serial.println("WiFi5 test");

  uint8_t status = WiFi5.begin(ssid, key);
  Serial.print("WiFi5 status after begin: ");
  Serial.println(status);
  if (status == WL_NO_SHIELD) {
    Serial.println("No hardware found");
    while (true);
  }

  if (WiFi5.macAddress(mac)) {
    Serial.print("MAC address: ");
    Serial.print(mac[0], HEX);
    for (int i = 1; i < 6; ++i) {
      Serial.print(':');
      Serial.print(mac[i], HEX);
    }
    Serial.println();
  } else {
    Serial.println("Get MAC error");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi5.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi5.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi5.gatewayIP());

  IPAddress googleAddr;
  if (WiFi5.hostByName("google.com", googleAddr)) {
    Serial.print("Google IP addr: ");
    Serial.println(googleAddr);
  } else {
    Serial.println("Get host by name error");
  }

  Serial.println("WiFi5 example started");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
}
