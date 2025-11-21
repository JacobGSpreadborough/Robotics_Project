#ifndef MOTORHANDLER_h
#define MOTORHANDLER_h
#include "motor.h"

class MotorHandler {
 public:
    motor motorA;
    motor motorB;
    MotorHandler(PinName, PinName, PinName, PinName, PinName, PinName);
    void init();
    void move(double speed, double turn);
};

#endif