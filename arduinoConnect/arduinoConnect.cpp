#include "simpleble/Types.h"
#include <ratio>
#include <simpleble/SimpleBLE.h>
#include <iostream>
#include <thread>

// the point at which the sensor stops giving accurate readings
#define IR_MINIMUM_VALUE 3000
// TODO determine the minimum value from the US sensors
#define US_MINIMUM_VALUE 0
// address for my arduino
SimpleBLE::BluetoothAddress arduinoAddress = "E9525410-C8F6-0B17-7139-E5855BBA4D21";
SimpleBLE::BluetoothUUID controlUUID = "19b10000-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothAddress directionUUID = "19b10001-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothUUID IRSensor0UUID = "19b10002-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothUUID IRSensor1UUID = "19b10003-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothUUID USSensor0UUID ="19b10004-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothUUID USSensor1UUID =  "19b10005-e8f2-537e-4f6c-d104768a1214"; 

// commands for the adruino
// TODO figure out how to do this better
SimpleBLE::ByteArray stop = SimpleBLE::ByteArray::fromHex("00");
SimpleBLE::ByteArray forward = SimpleBLE::ByteArray::fromHex("01");
SimpleBLE::ByteArray left = SimpleBLE::ByteArray::fromHex("02");
SimpleBLE::ByteArray right = SimpleBLE::ByteArray::fromHex("03");
SimpleBLE::ByteArray reverse = SimpleBLE::ByteArray::fromHex("04");



int IRData0 = 0;
int IRData1 = 0;
int USData0 = 0;
int USData1 = 0;

int bytesToInt(SimpleBLE::ByteArray bytes) {
		return (bytes[0] << 8) + bytes[1];
}

int main(int argc, char* argv[]) {

	if(!SimpleBLE::Adapter::bluetooth_enabled()){
        std::cout << "Bluetooth is not enabled" << std::endl;
        return 1;
    }	
    // get and use the first adapter
    // this should be the only adapter on this computer
    std::vector<SimpleBLE::Adapter> adapters = SimpleBLE::Adapter::get_adapters();
    // fail if no adapters are found
    if(adapters.empty()) {
        std::cout << "No adapters available" << std::endl;
        return 1;
    }
    SimpleBLE::Adapter adapter = adapters[0];
    SimpleBLE::Peripheral arduino;
	    
	// callback to check each found peripheral, and connect once the arduino is found
    adapter.set_callback_on_scan_found([&adapter, &arduino](SimpleBLE::Peripheral peripheral) {
        std::cout << "Peripheral found: " << peripheral.identifier() << std::endl;
		std::cout << "Address: " << peripheral.address() << std::endl;
        if(peripheral.address() == arduinoAddress) {
            arduino = peripheral;
            adapter.scan_stop();
        }
    });
    std::cout << "Scanning for Arduino..." << std::endl;
    adapter.scan_start();
    // program idles while waiting for connection
    while(adapter.scan_is_active()) {}
    try {
        arduino.connect();
    } catch (SimpleBLE::Exception::OperationFailed) {
        std::cout << "Connection failed" << std::endl;
        return 1;
    }
    std::cout << "Successfully Connected" << std::endl;
 	// callback for recieving data from the IR sensor
	arduino.notify(controlUUID,IRSensor0UUID,[&](SimpleBLE::ByteArray bytes) {
					IRData0 = bytesToInt(bytes);
					if(IRData0 < IR_MINIMUM_VALUE) {
						IRData0 = 0;
					}
		});
	arduino.notify(controlUUID,IRSensor1UUID,[&](SimpleBLE::ByteArray bytes) {
					IRData1 = bytesToInt(bytes);
					if(IRData1 < IR_MINIMUM_VALUE) {
						IRData1 = 0;
					}
		});
	// callback for recieving data from the US Sensor
	arduino.notify(controlUUID,USSensor0UUID,[&](SimpleBLE::ByteArray bytes) {
					USData0 = bytesToInt(bytes);
					if(USData0 < US_MINIMUM_VALUE) {
						USData0 = 0;
					}
		});
	arduino.notify(controlUUID,USSensor1UUID,[&](SimpleBLE::ByteArray bytes) {
					USData1 = bytesToInt(bytes);
					if(USData1 < US_MINIMUM_VALUE) {
						USData1 = 0;
					}
	//				std::cout<< "US Sensor: " << USData  << std::endl; 
		});
    while(arduino.is_connected()){
		system("clear");
		std::cout << "frontL: " << USData0 << std::endl; 
		std::cout << "frontR: " << USData1 << std::endl; 
		std::cout << "left: " << IRData0 << std::endl; 
		std::cout << "right: " << IRData1 << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));	
	}
}
