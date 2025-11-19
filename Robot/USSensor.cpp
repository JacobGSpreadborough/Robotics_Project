#include "USSensor.h"
#define US_MAX_DISTANCE 10000

USSensor::USSensor(PinName trig, int threshold):
   _trig(trig), _threshold(threshold) {}

bool USSensor::changed() {
  if(abs(data[1] - lastData[1]) > _threshold) {
      return true;
    } else {
      return false;
    }
}

void USSensor::read() {
  lastData[0] = data[0];
  lastData[1] = data[1];
  _t.reset();
  _trig.output();
  _trig.write(1);
  wait_us(10);
  _trig.write(0);
  _trig.input();
  while(_trig == LOW){}
  _t.start();
  while(_trig == HIGH){}
  _t.stop();
  long long int uSeconds = std::chrono::duration_cast<std::chrono::microseconds>(_t.elapsed_time()).count();
  if(uSeconds > US_MAX_DISTANCE) {
    uSeconds = US_MAX_DISTANCE;
  }
  data[0] = char(uSeconds >> 8);
  data[1] = char(uSeconds);
}