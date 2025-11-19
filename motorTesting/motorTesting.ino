#include "motorHandler.h"
#include <mbed.h>

mbed::InterruptIn encoderA(P1_11);
mbed::InterruptIn encoderB(P1_12);
MotorHandler motorHandler(P0_4, P0_27, P0_5, P1_2);


void encoderACallback() {
  motorHandler.incrementEncoderA();
}
void encoderBCallback() {
  motorHandler.incrementEncoderB();
}

int loops;

void setup() {
  motorHandler.init();
  encoderA.rise(&encoderACallback);
  encoderB.rise(&encoderBCallback);
  encoderA.fall(&encoderACallback);
  encoderB.fall(&encoderBCallback);
  Serial.begin(9600);
}

void loop() {
  motorHandler.move(320,2);
}
