#include "USSensor.h"
#define US_MAX_DISTANCE 10000
#define US_MIN_DISTANCE 300

USSensor::USSensor(PinName trig, PinName echo, int threshold):
   _trig(trig), _echo(echo), _threshold(threshold) {}

bool USSensor::changed() {
  if(abs(data - lastData) > _threshold) {
      return true;
    } else {
      return false;
    }
}
// TODO maybe implement this without waiting
void USSensor::trigger(){
  _trig.write(1);
  wait_us(10);
  _trig.write(0);
}

void USSensor::init(){
  _echo.rise(mbed::callback(this, &USSensor::_startTimer));
  _echo.fall(mbed::callback(this, &USSensor::_stopTimer));
}

void USSensor::_startTimer() {
  _t.start();
}

void USSensor::_stopTimer() {
  _t.stop();
  long long int uSeconds = std::chrono::duration_cast<std::chrono::microseconds>(_t.elapsed_time()).count();
  if(uSeconds > US_MAX_DISTANCE) {
    uSeconds = US_MAX_DISTANCE;
  }
  if(uSeconds < US_MIN_DISTANCE) {
    uSeconds = 0;
  }
  lastData = data;
  data = (float)uSeconds;
  _t.reset();
}
