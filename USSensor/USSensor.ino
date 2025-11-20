#include "USSensor.h"
// The sensors share the fused trig/echo pin for sensor 1
// US_1 uses the trig/echo pin for sensor 1 as its echo
// US_2 uses the trig/echo pin for sensor 3 as its echo
// this way we can use interrupts without having to dynamically configure the pins
USSensor US_1(P1_14,P0_23,100);
USSensor US_2(P1_14,P1_13,100);

int bytesToInt(char bytes[2]) {
  return (int(bytes[0]) << 8) + int(bytes[1]);
}

void setup() {
  US_1.init();
  US_2.init();
  Serial.begin(9600);
  delay(2000);
  Serial.println("Starting");
}

void loop() {
  US_1.trigger();
  US_2.trigger();
  delay(100);
  if(US_1.changed()){
    Serial.print("1: ");
    Serial.println(bytesToInt(US_1.data));
  }
  if(US_2.changed()){
    Serial.print("2: ");
    Serial.println(bytesToInt(US_2.data));
  }
}
