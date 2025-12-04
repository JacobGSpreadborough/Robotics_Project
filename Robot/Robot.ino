#include <mbed.h>
#include "Arduino_BMI270_BMM150.h"
#include <ArduinoBLE.h>
#include <IRSensor.h>
#include <motorHandler.h>
#include <USSensor.h>

#define IR_DATA_WIDTH 2
#define US_DATA_WIDTH 2
#define IR_SENSOR_THRESHOLD 0
#define US_SENSOR_THRESHOLD 0
#define US_1_OFFSET 0
#define US_2_OFFSET 4
#define IR_1_OFFSET 8
#define IR_2_OFFSET 10
#define X_POSITION_OFFSET 0
#define Y_POSITION_OFFSET 4
#define ANGLE_OFFSET 8

BLEService controlService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic directionCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLECharacteristic sensorCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 12);
BLECharacteristic positionCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 12);

unsigned char sensorBuffer[12];
unsigned char positionBuffer[12];

mbed::I2C i2c(P0_31, P0_2);
mbed::Timer t;
IRSensor IR_1(i2c, 0, IR_SENSOR_THRESHOLD);
IRSensor IR_2(i2c, 1, IR_SENSOR_THRESHOLD);
USSensor US_1(P1_14,P0_23,US_SENSOR_THRESHOLD);
USSensor US_2(P1_14,P1_13,US_SENSOR_THRESHOLD);
MotorHandler motorHandler(P0_4, P0_27, P1_11, P0_5, P1_2, P1_12);
rtos::Thread sensors;
rtos::Thread gyro;
volatile bool connected = false;
volatile bool disconnect = false;
float xAngle,yAngle,zAngle;
float xPosition, yPosition = 0;
float angle, delta = 0;
float vA, vB, V = 0;

int bytesToInt(char bytes[2]) {
  return (bytes[0] << 8) + bytes[1];
}

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
    delay(50);
    t.stop();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count();
    if(IMU.gyroscopeAvailable()) {

      IMU.readGyroscope(xAngle,yAngle,zAngle);
      // adjust for the IMU's inaccuracy
      zAngle -= delta;
      // integrate velocity to get position
      angle += (ms * zAngle)/1000;
      // give the motors the current angle to calculate distance
      motorHandler.angle = angle;

    }
    // divide by 10 to get cm instead of mm
    xPosition += (motorHandler.motorA.distance + motorHandler.motorB.distance)/2 * cos(angle * PI/180) / 10;
    yPosition += (motorHandler.motorA.distance + motorHandler.motorB.distance)/2 * sin(-angle * PI/180) / 10;

    motorHandler.motorA.distance = 0;
    motorHandler.motorB.distance = 0;

    memcpy(positionBuffer + X_POSITION_OFFSET, &xPosition, 4);
    memcpy(positionBuffer + Y_POSITION_OFFSET, &yPosition, 4);
    memcpy(positionBuffer + ANGLE_OFFSET, &angle, 4);
    positionCharacteristic.writeValue(positionBuffer, 12);
  }
}

mbed::Timer test;

void sensorLoop() {
  while(connected){
    // only trigger one US sensor at a time
    US_1.trigger();
    IR_1.read();
    IR_2.read();
    // long enough for the US sensors to time out
    delay(50);
    if(US_1.changed()) {
      memcpy(sensorBuffer + US_1_OFFSET, &US_1.data, 4);
    }
    if(US_2.changed()) {
      memcpy(sensorBuffer + US_2_OFFSET, &US_2.data, 4);
    }
    if(IR_1.changed()) {
      memcpy(sensorBuffer + IR_1_OFFSET, &IR_1.data, 2);
    }
    if(IR_2.changed()) {
      memcpy(sensorBuffer + IR_2_OFFSET, &IR_2.data, 2);
    }
    sensorCharacteristic.writeValue(sensorBuffer, 12);
  }
}

void directionCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  switch(directionCharacteristic.value()) {
    case 0:
          motorHandler.move(0,0);
          break;
        case 1:
          motorHandler.move(140,0);
          break;
        case 2:
          motorHandler.move(140,150);
          break;
        case 3:
          motorHandler.move(140,-150);
          break;
        case 4:
          motorHandler.move(-140,0);
          break;
        default:
          motorHandler.move(0,0);
          break;
  }
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  motorHandler.init();
  US_1.init();
  US_2.init();
  Serial.begin(115200);
  Serial.println("Starting");

  if(!IMU.begin()) {
    Serial.println("Could not start IMU");
  }
  Serial.println("Calibrating IMU");
  delta = calibrateIMU();
  Serial.println("IMU calibrated");
  Serial.println(delta);

  if (!BLE.begin()) {
    Serial.print("Could not start BLE");
    NVIC_SystemReset();
  }

  Serial.println("BLE Started");

  BLE.setDeviceName("Spread");
  BLE.setLocalName("Spread");
  BLE.setAdvertisedService(controlService);
  controlService.addCharacteristic(directionCharacteristic);
  controlService.addCharacteristic(sensorCharacteristic);
  controlService.addCharacteristic(positionCharacteristic);

  BLE.addService(controlService);

  BLE.advertise();
  BLE.setEventHandler(BLEConnected, connectionHandler);
  BLE.setEventHandler(BLEDisconnected, disconnectionHandler);

  directionCharacteristic.setEventHandler(BLEWritten, directionCharacteristicWritten);

  Serial.println("BLE advertising");

  digitalWrite(LED_BUILTIN, LOW);

}
// this doesn't work when reconnecting
void connectionHandler(BLEDevice central) {
  disconnect = false;
  connected = true;
  delay(100);
  Serial.print("Connected to central: ");
  Serial.println(central.address());
  sensors.start(&sensorLoop);
  Serial.println("sensors started");
  gyro.start(&gyroLoop);
  Serial.println("gyro started");
  // set LED high when connected
  digitalWrite(LED_BUILTIN, HIGH);
}

void disconnectionHandler(BLEDevice central) {
  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
  connected = false;
  disconnect = true;
  // set LED low when connected
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  // flash LED while waiting for connection
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);

  if(disconnect) {
    sensors.join();
    Serial.println("sensors joined");
    gyro.join();
    Serial.println("gyro joined");
    motorHandler.move(0,0);
  }

  BLEDevice central = BLE.central();
  BLE.poll();

  // main loop
  while (central.connected()) {
    BLE.poll();
    delay(1000);
  }

}
