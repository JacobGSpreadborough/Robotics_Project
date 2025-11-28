#ifndef MOTORHANDLER_h
#define MOTORHANDLER_h
#include "motor.h"
#include <mbed.h>

class MotorHandler {
 private:
    mbed::Timer _t;
    float _prevSpeed;
 public:
    motor motorA, motorB;
    MotorHandler(PinName, PinName, PinName, PinName, PinName, PinName);
    float xPosition = 17;
    float yPosition = 73;
    float angle;
    void init();
    void move(float,float);
    void location(float);
};

#endif