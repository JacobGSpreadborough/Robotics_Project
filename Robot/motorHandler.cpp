#include "motorHandler.h"

motorHandler::motorHandler(PinName dirA, PinName speedA, PinName dirB, PinName speedB):
   dirA(dirA), speedA(speedA), dirB(dirB), speedB(speedB) {
  }
  // extra initialization function to set up the motors, doing this in the constructor did not work
  // TODO maybe add some error handling stuff here
void motorHandler::init() {
    speedA.period(0.001f);
    speedB.period(0.001f);
}

void motorHandler::incrementEncoderA() {
  encoderCountA += dirA ? 1 : -1;
  if(encoderCountA % 330 == 0) {
    revCountA++;
  }
}

void motorHandler::incrementEncoderB() {
  encoderCountB += dirB ? -1 : 1;
  if(encoderCountB % 330 == 0) {
    revCountB++;
  }
}

void motorHandler::turn(double angle) {}

void motorHandler::move(double speed) {
  if(speed >= 0) {
    dirA = LOW;
    dirB = HIGH;
  } 
  if(speed < 0) {
    dirA = HIGH;
    dirB = LOW;
  }
  speedA.write(abs(speed));
  speedB.write(abs(speed));
}

void motorHandler::left(double speed) {
  dirA = HIGH;
  dirB = HIGH;
  speedA.write(speed);
  speedB.write(speed);
}
void motorHandler::right(double speed) {
  dirA = LOW;
  dirB = LOW;
  speedA.write(speed);
  speedB.write(speed);
}
