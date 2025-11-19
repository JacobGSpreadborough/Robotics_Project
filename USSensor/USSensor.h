#ifndef USSENSOR_h
#define USSENSOR_h
#include <mbed.h>

class USSensor {
  private: 
    int _threshold;
    mbed::DigitalInOut _trig;
    mbed::Timer _t;
  public:
    USSensor(PinName trig, int threshold);
    char data[2];
    char lastData[2];
    
    bool changed();
    void read();
};

#endif