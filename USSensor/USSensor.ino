#include <mbed.h>
// sensor 2 trig/echo pin
// triggers both sensors
mbed::DigitalOut trig(P1_14);
// sensor 3 trig/echo pin
// echo for sensor 2
mbed::InterruptIn echo1(P1_13);
// sensor 1 trig/echo pin
// echo for sensor 1
mbed::InterruptIn echo2(P0_23);


mbed::Timer t1;
mbed::Timer t2;
long long US1Data;
long long US2Data;

void echo1Rising(){
  t1.reset();
  t1.start();
}

void echo1Falling() {
  t1.stop();
  US1Data = std::chrono::duration_cast<std::chrono::microseconds>(t1.elapsed_time()).count();
}

void echo2Rising(){
  t2.reset();
  t2.start();
}

void echo2Falling() {
  t2.stop();
  US2Data = std::chrono::duration_cast<std::chrono::microseconds>(t2.elapsed_time()).count();
}

void triggerSensors() {
  // trigger the sensors
  trig.write(1);
  wait_us(15);
  trig.write(0);
}

void setup() {
  echo1.rise(&echo1Rising);
  echo1.fall(&echo1Falling);
  echo2.rise(&echo2Rising);
  echo2.fall(&echo2Falling);
  Serial.begin(9600);
  delay(2000);
  Serial.println("Starting");
}

void loop() {
  triggerSensors();
  delay(50);
  Serial.print("Sensor 1: ");
  Serial.println(US1Data);
  Serial.print("Sensor 2: ");
  Serial.println(US2Data);
}
