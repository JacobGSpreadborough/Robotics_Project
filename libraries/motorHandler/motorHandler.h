#ifndef MOTORHANDLER_h
#define MOTORHANDLER_h
#include "motor.h"
#include <mbed.h>

class MotorHandler {
 private:
    mbed::Timer _t;
 public:
    motor motorA, motorB;
    MotorHandler(PinName, PinName, PinName, PinName, PinName, PinName);
    double xPosition, yPosition, angle;
    void init();
    void move(double speed, double turn);
    void location();
};

#endif