#ifndef MOTOR_h
#define MOTOR_h
#include <mbed.h>

class motor {
  private:
    mbed::DigitalOut _dir;
    mbed::PwmOut _PWM;
    mbed::InterruptIn _encoder;
    int _increment;
    float _mmPerTick;
    bool _inverted;
    void _incrementEncoder();
  public:
    float distance;
    long long encoderCount;
    float velocity = 0;
    motor(PinName, PinName, PinName, bool);
    void init();
    void move(float speed);

};

#endif