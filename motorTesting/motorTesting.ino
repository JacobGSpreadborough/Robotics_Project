#include <motorHandler.h>
#include <mbed.h>

MotorHandler motorHandler(P0_4, P0_27, P1_11, P0_5, P1_2, P1_12);

void setup() {
  motorHandler.init();
  Serial.begin(9600);
}

void loop() {
  motorHandler.move(200,0);
  delay(1000);
  Serial.print("A: ");
  Serial.println(motorHandler.motorA.encoderCount);
  Serial.print("B: ");
  Serial.println(motorHandler.motorB.encoderCount);
  motorHandler.move(0,0);
  delay(100000);
}
