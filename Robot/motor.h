#ifndef MOTOR_h
#define MOTOR_h
#include <mbed.h>

class motor {
  private:
    mbed::DigitalOut _dir;
    mbed::PwmOut _PWM;
    mbed::InterruptIn _encoder;
    int _increment;
    bool _inverted;
    void _incrementEncoder();
  public:
    double distance;
    long long encoderCount;
    motor(PinName, PinName, PinName, bool);
    void init();
    void move(double speed);

};

#endif