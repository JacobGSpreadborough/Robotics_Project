#include <mbed.h>
#include <ArduinoBLE.h>
#include "IRSensor.h"
#include "motorHandler.h"
#include "USSensor.h"

#define IR_DATA_WIDTH 2
#define US_DATA_WIDTH 2
#define IR_SENSOR_THRESHOLD 10
#define US_SENSOR_THRESHOLD 10

BLEService controlService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic directionCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLECharacteristic sensorIR0Characteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, IR_DATA_WIDTH, true);
BLECharacteristic sensorIR1Characteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, IR_DATA_WIDTH, true);
BLECharacteristic sensorUS0Characteristic("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, US_DATA_WIDTH, true);
BLECharacteristic sensorUS1Characteristic("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, US_DATA_WIDTH, true);

mbed::I2C i2c(P0_31, P0_2);
IRSensor IR_0(i2c, 0, IR_SENSOR_THRESHOLD);
IRSensor IR_1(i2c, 1, IR_SENSOR_THRESHOLD);
USSensor US_1(P1_14,P0_23,100);
USSensor US_2(P1_14,P1_13,100);
motorHandler motor(P0_4, P0_27, P0_5, P1_2);

int bytesToInt(char bytes[2]) {
  return (bytes[0] << 8) + bytes[1];
}
void setup() {
  motor.init();
  US_1.init();
  US_2.init();
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
