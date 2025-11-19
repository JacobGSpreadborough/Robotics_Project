#include "IRSensor.h"

IRSensor::IRSensor(mbed::I2C &i2c, int id, int threshold)
 : _i2c(i2c), _id(id), _threshold(threshold) {
  _mux_cmd = 0x01 << id;
 }

void IRSensor::test() {
  Serial.print("ID: ");
  Serial.println(_id);
  Serial.print("Address: ");
  Serial.println(_mux_cmd, HEX);
}

void IRSensor::read() {
  // strcpy should work here but it freezes the program for some reason
  lastData[0] = data[0];
  lastData[1] = data[1];
  _i2c.write(0xEE, &_mux_cmd, 1);
  char cmd[2] = {0x5e, 0x00};
  _i2c.write(0x80, cmd, 2);
  _i2c.read(0x80, data, 2);
}

bool IRSensor::changed() {
  if(abs(data[1] - lastData[1]) > _threshold) {
    return true;
  } else {
    return false;
  }
}