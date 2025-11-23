#include "IRSensor.h"

IRSensor::IRSensor(mbed::I2C &i2c, int id, int threshold)
 : _i2c(i2c), _id(id), _threshold(threshold) {
  _mux_cmd = 0x01 << id;
 }


void IRSensor::read() {
  // strcpy should work here but it freezes the program for some reason
  lastData = data;
  _i2c.write(0xEE, &_mux_cmd, 1);
  char cmd[2] = {0x5e, 0x00};
  _i2c.write(0x80, cmd, 2);
  _i2c.read(0x80, bytes, 2);
  data = (bytes[0] << 8) + bytes[1];
  
}

bool IRSensor::changed() {
  if(abs(data - lastData) > _threshold) {
    return true;
  } else {
    return false;
  }
}
