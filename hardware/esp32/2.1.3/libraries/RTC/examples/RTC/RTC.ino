// RTC library example
// by Industrial Shields

#include <RTC2.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600L);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  if (!RTC.read()) {
    Serial.println("Read date error: is time set?");
  } else {
    Serial.print("Time: ");
    Serial.print(RTC.getYear());
    Serial.print("-");
    Serial.print(RTC.getMonth());
    Serial.print("-");
    Serial.print(RTC.getMonthDay());
    Serial.print(" ");
    Serial.print(RTC.getHour());
    Serial.print(":");
    Serial.print(RTC.getMinute());
    Serial.print(":");
    Serial.print(RTC.getSecond());
    Serial.print(" (");
    Serial.print(RTC.getTime());
    Serial.println(")");
  }

  delay(1000);
}
