#include "motor.h"

motor::motor(PinName dir, PinName PWM) :
  dir(dir), PWM(PWM) { }

  void motor::init() {
    PWM.period(0.001f);
  }

  void motor::move(double speed) {
    if(speed < 0) {
      dir = HIGH;
    }
    if(speed > 0) {
      dir = LOW;
    }
    
    PWM.write(fabs(speed));
  }