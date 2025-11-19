#ifndef MOTORHANDLER_h
#define MOTORHANDLER_h
#include "motor.h"

class MotorHandler {
  public:
    motor motorA;
    motor motorB;
    MotorHandler(PinName dirA, PinName PWMA, PinName dirB, PinName PWMB);
    long long encoderCountA;
    long long revCountA;
    long long encoderCountB;
    long long revCountB;
    void init();
    void incrementEncoderA();
    void incrementEncoderB();
    void move(double speed, double turn);
};

#endif