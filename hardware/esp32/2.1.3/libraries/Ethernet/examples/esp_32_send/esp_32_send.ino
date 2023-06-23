/// IMPORTANT: check switches configuration
void setup() {
  // Begin serial port
  Serial.begin(115200);
  Serial2.begin(115200);
 pinMode(5, OUTPUT);
 digitalWrite(5, HIGH);
}

void loop() {
    // Print received byte when available
    Serial2.println("Tx ->");
    delay(1000);
}
