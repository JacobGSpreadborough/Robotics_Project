#include "motorHandler.h"

using namespace std::chrono;

MotorHandler::MotorHandler(PinName dirA, PinName PWMA, PinName encoderA, PinName dirB, PinName PWMB, PinName encoderB):
   motorA(dirA, PWMA, encoderA, false), motorB(dirB, PWMB, encoderB, true) {
  }
  // extra initialization function to set up the motors, doing this in the constructor did not work
  // TODO maybe add some error handling stuff here
void MotorHandler::init() {
  motorA.init();
  motorB.init();
}

// speed should be in mm/s, turn should be in deg/s
// TODO compensate for the difference between left and right motors
void MotorHandler::move(double speed, double turn) {
  _t.stop();
  long long int uSeconds = duration_cast<milliseconds>(_t.elapsed_time()).count();
  angle += (_prevTurn * uSeconds)/1000;
  _prevTurn = turn;
  _t.reset();
  _t.start();
  if(speed == 0) {
    motorA.move(0);
    motorB.move(0);
    _t.stop();
    _t.reset();
  } else {
    // convert cm/s into 0...1
    speed = (speed + 30)/350;
    // convert deg/s into 0...1
    double compensation = (1 - speed) * 25;
    turn = (turn + compensation)/115;
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
