#include <mbed.h>
#include <ArduinoBLE.h>
#include "IRSensor.h"
#include "motorHandler.h"

#define IR_DATA_WIDTH 2
#define US_DATA_WIDTH 2
#define IR_SENSOR_THRESHOLD 10
#define US_SENSOR_THRESHOLD 10
#define US_MAX_DISTANCE 10000

BLEService controlService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic directionCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLECharacteristic sensorIR0Characteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, IR_DATA_WIDTH, true);
BLECharacteristic sensorIR1Characteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, IR_DATA_WIDTH, true);
BLECharacteristic sensorUS0Characteristic("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, US_DATA_WIDTH, true);
BLECharacteristic sensorUS1Characteristic("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, US_DATA_WIDTH, true);

mbed::I2C i2c(P0_31, P0_2);
mbed::InterruptIn encoderA(P1_11);
mbed::InterruptIn encoderB(P1_12);
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
IRSensor IR_0(i2c, 0, IR_SENSOR_THRESHOLD);
IRSensor IR_1(i2c, 1, IR_SENSOR_THRESHOLD);
motorHandler motor(P0_4, P0_27, P0_5, P1_2);
char US1Data[2];
char US2Data[2];

int bytesToInt(char bytes[2]) {
  return (bytes[0] << 8) + bytes[1];
}
void echo1Rising(){
  t1.reset();
  t1.start();
}
void echo1Falling() {
  t1.stop();
  long long uSeconds = std::chrono::duration_cast<std::chrono::microseconds>(t1.elapsed_time()).count();  
  if(uSeconds > US_MAX_DISTANCE) {
    uSeconds = US_MAX_DISTANCE;
  }
  US1Data[0] = char(uSeconds >> 8);
  US2Data[1] = char(uSeconds);
}
void echo2Rising(){
  t2.reset();
  t2.start();
}
void echo2Falling() {
  t2.stop();
  long long uSeconds = std::chrono::duration_cast<std::chrono::microseconds>(t2.elapsed_time()).count();  
  if(uSeconds > US_MAX_DISTANCE) {
    uSeconds = US_MAX_DISTANCE;
  }
  US2Data[0] = char(uSeconds >> 8);
  US2Data[1] = char(uSeconds);
}
void encoderACallback(){
  motor.incrementEncoderA();
}
void encoderBCallback(){
  motor.incrementEncoderB();
}
void triggerUSSensors() {
  // trigger the sensors
  trig.write(1);
  wait_us(15);
  trig.write(0);
}
void setup() {
  motor.init();
  encoderA.rise(&encoderACallback);
  encoderB.rise(&encoderBCallback);
  echo1.rise(&echo1Rising);
  echo1.fall(&echo1Falling);
  echo2.rise(&echo2Rising);
  echo2.fall(&echo2Falling);
  Serial.begin(115200);
  delay(5000);
  Serial.println("Starting");

  if (!BLE.begin()) {
    Serial.print("Could not start BLE");
    NVIC_SystemReset();
  }

  BLE.setDeviceName("Spread");
  BLE.setLocalName("Spread");
  BLE.setAdvertisedService(controlService);
  controlService.addCharacteristic(directionCharacteristic);
  controlService.addCharacteristic(sensorIR0Characteristic);
  controlService.addCharacteristic(sensorIR1Characteristic);
  controlService.addCharacteristic(sensorUS0Characteristic);
  controlService.addCharacteristic(sensorUS1Characteristic);

  BLE.addService(controlService);

  BLE.advertise();
}
void loop() {

  BLEDevice central = BLE.central();
  if (central.connected()) {
    Serial.println("Connected to central: ");
    Serial.println(central.address());
  } else {
    //Serial.println("no connection");
  }

  while (central.connected()) {
    BLE.poll();

    IR_0.read();
    IR_1.read();
    triggerUSSensors();
    delay(50);
    sensorIR0Characteristic.writeValue(IR_0.data);
    sensorIR1Characteristic.writeValue(IR_1.data);
    sensorUS0Characteristic.writeValue(US1Data);
    sensorUS1Characteristic.writeValue(US2Data);

    if (directionCharacteristic.written()) {
      switch (directionCharacteristic.value()) {
        case 0:
          Serial.println("stop");
          motor.move(0.0f);
          break;
        case 1:
          Serial.println("forward");
          motor.move(0.7f);
          break;
        case 2:
          Serial.println("left");
          motor.left(0.7f);
          break;
        case 3:
          Serial.println("right");
          motor.right(0.7f);
          break;
        case 4:
          Serial.println("reverse");
          motor.move(-0.7f);
          break;
        default: 
          Serial.println("Invalid input");
          break;
      }
    }
  }
}
