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
    char bytes[2];
    // only 2 bytes of data
    // TODO just send the data buffer rather than converting to short
	  unsigned short lastData = 0;
	  unsigned short data = 15;
    bool changed();
    void read();
};

#endif
