#include "motorHandler.h"
#define PI 3.1415926536

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
void MotorHandler::move(float speed, float turn) {
  location(speed);
  if(speed == 0) {
    motorA.move(0);
    motorB.move(0);
    _t.stop();
    _t.reset();
  } else {
    // convert cm/s into 0...1
    speed /= 300;
    // compensate for speed
    turn += 30 * (1 - speed);
    // convert deg/s into -2...2
    turn /= 120;
    //-2 for hard left, +2 for hard right
    if(turn == 0) {
      motorA.move(speed - 0.1);
      motorB.move(-(speed));
    } else if(turn > 0) {
      motorA.move(speed - 0.1);
      motorB.move(-(speed - turn));
    } else if(turn < 0) {
      motorA.move(speed + turn - 0.1);
      motorB.move(-speed);
    }
  }
}

void MotorHandler::location(float speed){
  _t.stop();
  long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(_t.elapsed_time()).count();
  // speed * time = distance
  xPosition += ((_prevSpeed * ms)/1000) * cos(angle * PI/180);
  yPosition += ((_prevSpeed * ms)/1000) * sin(angle * PI/180);
  _prevSpeed = speed;
  _t.reset();
  _t.start();
}