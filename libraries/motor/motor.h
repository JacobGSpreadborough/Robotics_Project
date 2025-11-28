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
    float distance;
    long long encoderCount;
    motor(PinName, PinName, PinName, bool);
    void init();
    void move(float speed);

};

#endif