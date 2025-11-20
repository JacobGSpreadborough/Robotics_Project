#include "motor.h"

motor::motor(PinName dir, PinName PWM, PinName encoder, bool inverted) :
  _dir(dir), _PWM(PWM), _encoder(encoder), _inverted(inverted) { }

  void motor::init() {
    _PWM.period(0.001f);
    _increment = _inverted ? 1 : -1;
    _encoder.rise(mbed::callback(this, &motor::_incrementEncoder));
    _encoder.fall(mbed::callback(this, &motor::_incrementEncoder));
  }

  void motor::_incrementEncoder() {
    // increment or decrement based on direction
    encoderCount += _dir ? _increment : -_increment;
    distance += _dir? -0.24 : 0.24;
  }

  void motor::move(double speed) {
    if(speed < 0) {
      _dir = HIGH;
    }
    if(speed > 0) {
      _dir = LOW;
    }
    _PWM.write(fabs(speed));
  }