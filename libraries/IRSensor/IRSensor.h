#ifndef IRSENSOR_h
#define IRSENSOR_h
#include <mbed.h>

class IRSensor {
  private: 
    mbed::I2C &_i2c;
    int _id;
    int _threshold;
    char _mux_cmd;
  public: 
    IRSensor(mbed::I2C &i2c, int id, int threshold);
    char data[2];
    char lastData[2];
    void test();
    bool changed();
    void read();
};

#endif