#include <motorHandler.h>
#include <mbed.h>
#include <USSensor.h>

MotorHandler motorHandler(P0_4, P0_27, P1_11, P0_5, P1_2, P1_12);
USSensor L(P1_14, P0_23,0);
USSensor R(P1_14, P1_13,0);


void setup() {
  motorHandler.init();
  L.init();
  R.init();
  Serial.begin(9600);
  delay(2000);
}

void loop() {

  motorHandler.move(200,90);
  delay(1000);
  delay(1000);
  motorHandler.move(0,0);
  Serial.println(motorHandler.angle);
  delay(100000);
}
