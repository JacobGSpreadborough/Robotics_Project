#ifndef MOTORHANDLER_h
#define MOTORHANDLER_h
#include <mbed.h>

class motorHandler {
  public:
    motorHandler(PinName dirA, PinName speedA, PinName dirB, PinName speedB);
    long long encoderCountA;
    long long revCountA;
    long long encoderCountB;
    long long revCountB;
    mbed::DigitalOut dirA;
    mbed::PwmOut speedA;
    mbed::DigitalOut dirB;
    mbed::PwmOut speedB;
    void init();
    void incrementEncoderA();
    void incrementEncoderB();
    void turn(double angle);
    void move(double speed);
    void left(double speed);
    void right(double speed);
};

#endif