#ifndef USSENSOR_h
#define USSENSOR_h
#include <mbed.h>

class USSensor {
  private:
    mbed::DigitalOut _trig;
    mbed::InterruptIn _echo;
    int _threshold;
    mbed::Timer _t;
    void _startTimer();
    void _stopTimer();
  public: 
    USSensor(PinName, PinName, int);
	int data;
	int lastData;
    bool changed();
    void init();
    void trigger();
};

#endif
