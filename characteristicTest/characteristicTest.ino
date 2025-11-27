#include <ArduinoBLE.h>

BLEService service("b0eff457-bd36-4086-912f-0d6f78142dc9");
BLECharacteristic characteristic(
    "b0eff458-bd36-4086-912f-0d6f78142dc9",
    BLERead | BLENotify,
    8  // float = 4 bytes
);

volatile bool connected = false;

void connectionHandler(BLEDevice central){
  Serial.println("Connected");
  connected = true;
}

void disconnectionHandler(BLEDevice central){
  Serial.println("disconnected");
  connected = false;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  BLE.begin();


  BLE.setLocalName("Spread");
  BLE.setAdvertisedService(service);
  service.addCharacteristic(characteristic);
  BLE.addService(service);
  BLE.advertise();
  BLE.setEventHandler(BLEConnected, connectionHandler);
  BLE.setEventHandler(BLEDisconnected, disconnectionHandler);
}

void loop() {
  BLEDevice central = BLE.central();
  while (connected) {
    if (Serial.available()) {
      float x = Serial.parseFloat();
      Serial.print("Sending float: ");
      Serial.println(x);
      float y = 0.4;
      
      uint8_t buf[4];
      memcpy(buf, &x, 4);
      memcpy(buf+4, &y, 4);

      characteristic.writeValue(buf, 8);

      while (Serial.available()) Serial.read();
    }

    BLE.poll();
    delay(5);
  }

}