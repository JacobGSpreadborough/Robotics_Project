#include "motorHandler.h"
MotorHandler::MotorHandler(PinName dirA, PinName PWMA, PinName encoderA, PinName dirB, PinName PWMB, PinName encoderB):
   motorA(dirA, PWMA, encoderA, false), motorB(dirB, PWMB, encoderB, true) {
  }
  // extra initialization function to set up the motors, doing this in the constructor did not work
  // TODO maybe add some error handling stuff here
void MotorHandler::init() {
  motorA.init();
  motorB.init();
}
void MotorHandler::move(double speed, double turn) {
  if(speed == 0) {
    motorA.move(0);
    motorB.move(0);
  } else {
    // convert cm/s into 0...1
    speed = (speed + 30)/350;
    // -2 for hard left, +2 for hard right
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
  }
}
