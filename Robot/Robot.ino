#include <mbed.h>
#include <ArduinoBLE.h>
#include <IRSensor.h>
#include <motorHandler.h>
#include <USSensor.h>

#define IR_DATA_WIDTH 2
#define US_DATA_WIDTH 2
#define IR_SENSOR_THRESHOLD 10
#define US_SENSOR_THRESHOLD 10

BLEService controlService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic directionCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEIntCharacteristic sensorIR1Characteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic sensorIR2Characteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic sensorUS1Characteristic("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic sensorUS2Characteristic("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);


mbed::I2C i2c(P0_31, P0_2);
IRSensor IR_1(i2c, 0, IR_SENSOR_THRESHOLD);
IRSensor IR_2(i2c, 1, IR_SENSOR_THRESHOLD);
USSensor US_1(P1_14,P0_23,US_SENSOR_THRESHOLD);
USSensor US_2(P1_14,P1_13,US_SENSOR_THRESHOLD);
MotorHandler motorHandler(P0_4, P0_27, P1_11, P0_5, P1_2, P1_12);
rtos::Thread sensors;
rtos::Thread control;
volatile bool connected = false;

int bytesToInt(char bytes[2]) {
  return (bytes[0] << 8) + bytes[1];
}

void sensorLoop() {
  while(connected){
    // only trigger one US sensor at a time
    US_1.trigger();
    IR_1.read();
    IR_2.read();
    // long enough for the US sensors to time out
    delay(100);
    if(US_1.changed()) {
      sensorUS1Characteristic.writeValue(US_1.data);
    }
    if(US_2.changed()) {
      sensorUS2Characteristic.writeValue(US_2.data);
    }
    if(IR_1.changed()) {
      sensorIR1Characteristic.writeValue(IR_1.data);
    }
    if(IR_2.changed()) {
      sensorIR2Characteristic.writeValue(IR_2.data);
    }
  }
}

void controlLoop() {
  while(connected) {
    
  }
}

void setup() {
  motorHandler.init();
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
  controlService.addCharacteristic(sensorIR1Characteristic);
  controlService.addCharacteristic(sensorIR2Characteristic);
  controlService.addCharacteristic(sensorUS1Characteristic);
  controlService.addCharacteristic(sensorUS2Characteristic);

  BLE.addService(controlService);

  BLE.advertise();

  sensors.start(&sensorLoop);
  control.start(&controlLoop);

}
void loop() {

  delay(100);
  BLEDevice central = BLE.central();

  if (central.connected()) {
    connected = true;
    Serial.println("Connected to central: ");
    Serial.println(central.address());
  } else {
    // if central was connected before
    if(connected) {
      connected = false;
      Serial.println("Connection lost");
      // stop the motors and threads
      sensors.join();
      Serial.println("Sensors joined");
      control.join();
      Serial.println("Controls joined");
      motorHandler.move(0,0);

    } else {
      Serial.println("No connection");
    }
  }
  // main loop
  while (central.connected()) {
    BLE.poll();
    motorHandler.move(100,0);
    delay(5000);
  }
}
