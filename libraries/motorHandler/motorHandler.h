#ifndef MOTORHANDLER_h
#define MOTORHANDLER_h
#include "motor.h"
#include <mbed.h>

class MotorHandler {
 private:
    double _prevTurn = 0;
    mbed::Timer _t;
 public:
    motor motorA;
    motor motorB;
    MotorHandler(PinName, PinName, PinName, PinName, PinName, PinName);
    double angle;
    void init();
    void move(double speed, double turn);
};

#endif