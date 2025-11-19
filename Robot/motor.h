#ifndef MOTOR_h
#define MOTOR_h
#include <mbed.h>

class motor {
  public:
    double distance;
    mbed::DigitalOut dir;
    mbed::PwmOut PWM;
    motor(PinName, PinName);
    void init();
    void move(double speed);
};

#endif