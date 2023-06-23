//// IMPORTANT: check switches configuration
void setup() {
  // Begin serial port
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
}

void loop() {
  // Print received byte when available
  if (Serial2.available()) {
    byte rx = Serial2.read();
    Serial2.println("Rx ->");
    Serial.println(rx);
    delay(1000);
  }
}
