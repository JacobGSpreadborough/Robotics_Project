#include <motorHandler.h>
#include <mbed.h>
#include <USSensor.h>
#include "Arduino_BMI270_BMM150.h"

MotorHandler motorHandler(P0_4, P0_27, P1_11, P0_5, P1_2, P1_12);
USSensor L(P1_14, P0_23,0);
USSensor R(P1_14, P1_13,0);

float xAngle,yAngle,zAngle;
float angle;
float delta;
rtos::Thread gyro;
mbed::Timer t;
const bool connected = true;

float calibrateIMU() {
  float delta = 0;
  int count = 0;
  // loop runs for 10 seconds
  long startTime = millis();
  long endTime = startTime;
  while((endTime - startTime) <= 10000) {
    if(IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(xAngle, yAngle, zAngle);
      delta += zAngle;
      count++;
      endTime = millis();
    }
  }
  Serial.print("Offset: ");
  Serial.println(delta);
  Serial.print("After: ");
  Serial.print(count);
  Serial.println(" cycles");
  return delta/count;
}

void gyroLoop(){
  while(connected) {
    t.reset();
    t.start();
    delay(100);
    if(IMU.gyroscopeAvailable()) {
      t.stop();
      long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count();

      IMU.readGyroscope(xAngle,yAngle,zAngle);
      // adjust for the IMU's inaccuracy
      zAngle -= delta;
      // integrate velocity to get position
      angle += (ms * zAngle)/1000;
      // give the motors the current angle to calculate distance
      motorHandler.angle = angle;
    }
  }
}

void setup() {
  motorHandler.init();
  Serial.begin(9600);
  if(!IMU.begin()) {
    Serial.println("Could not start IMU");
  }
  Serial.println("Calibrating IMU");
  delta = calibrateIMU();
  Serial.println("IMU calibrated");
  Serial.println(delta);
  gyro.start(&gyroLoop);
}

void loop() {
  // wait until it's unplugged
  while(Serial);
  delay(500);
  motorHandler.move(200,0);
  delay(1000);
  motorHandler.move(200,90);
  delay(1000);
  motorHandler.move(200,0);
  delay(500);
  motorHandler.move(0,0);
  // wait until it's plugged in
  while(!Serial);
  Serial.println(motorHandler.xPosition);
  Serial.println(motorHandler.angle);
  delay(100000);
}
