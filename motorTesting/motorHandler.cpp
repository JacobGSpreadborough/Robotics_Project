#include "motorHandler.h"

MotorHandler::MotorHandler(PinName dirA, PinName PWMA, PinName dirB, PinName PWMB):
   motorA(dirA, PWMA), motorB(dirB, PWMB) {
  }
  // extra initialization function to set up the motors, doing this in the constructor did not work
  // TODO maybe add some error handling stuff here
void MotorHandler::init() {
  motorA.init();
  motorB.init();
}

void MotorHandler::incrementEncoderA() {
  encoderCountA += motorA.dir ? -1 : 1;
  motorA.distance += motorA.dir ? -0.235 : 0.235;
  if(encoderCountA % 330 == 0) {
    revCountA++;
  }
}

void MotorHandler::incrementEncoderB() {
  encoderCountB += motorB.dir ? 1 : -1;
  motorB.distance += motorB.dir ? 0.241 : -0.241;
  if(encoderCountB % 330 == 0) {
    revCountB++;
  }
}

void MotorHandler::move(double speed, double turn) {
  // convert cm/s into 0...1
  if(speed != 0) {
    speed = (speed + 30)/350;
  }
if(turn == 0) {
  motorA.move(speed);
  motorB.move(-speed);
} else if(turn > 0) {
    motorA.move(speed);
    motorB.move(-(speed - turn));
  } else if(turn < 0) {
    motorA.move(speed + turn);
    motorB.move(-speed);
  }

//  motorA.move(speed + turn);
//  motorB.move(-speed + turn);
}
