#include "simpleble/Types.h"
#include <simpleble/SimpleBLE.h>
#include <iostream>

#define US_0_OFFSET 0
#define US_1_OFFSET 4
#define IR_0_OFFSET 8
#define IR_1_OFFSET 10

// address for my arduino
SimpleBLE::BluetoothAddress arduinoAddress = "6CFC6917-94B4-03DA-C5BC-9BC05CA2496B";
SimpleBLE::BluetoothUUID controlUUID = "19b10000-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothUUID directionUUID = "19b10001-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothUUID sensorUUID = "19B10002-E8F2-537E-4F6C-D104768A1214";

// commands for the adruino
// TODO figure out how to do this better
SimpleBLE::ByteArray stop = SimpleBLE::ByteArray::fromHex("00");
SimpleBLE::ByteArray forward = SimpleBLE::ByteArray::fromHex("01");
SimpleBLE::ByteArray left = SimpleBLE::ByteArray::fromHex("02");
SimpleBLE::ByteArray right = SimpleBLE::ByteArray::fromHex("03");
SimpleBLE::ByteArray reverse = SimpleBLE::ByteArray::fromHex("04");

enum RobotState {
	STATE_STOP = 0,
	STATE_FORWARD,
	STATE_LEFT,
	STATE_RIGHT,
	STATE_BACKWARD
}currentState;

short IRData0 = 0;
short IRData1 = 0;
float USData0 = 0;
float USData1 = 0;

int bytesToInt(SimpleBLE::ByteArray bytes, size_t length) {
		int r =0;
		for(int i=0;i<length;i++){
			r += bytes[i] << ( 8 * i);
		}
		return r;
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

	arduino.notify(controlUUID,sensorUUID,[&](SimpleBLE::ByteArray bytes) {
					std::cout << "incoming size: " << bytes.size() << std::endl;
					USData0 = (*(float*)(bytes.data() + US_0_OFFSET));
					std::cout << USData0 << std::endl;
					USData1 = (*(float*)(bytes.data() + US_1_OFFSET));
					std::cout << USData1 << std::endl;
					IRData0 = (*(short*)(bytes.data() + IR_0_OFFSET));
					std::cout << IRData0 << std::endl;
					IRData1 = (*(short*)(bytes.data() + IR_1_OFFSET));
					std::cout << IRData1 << std::endl;
		});

    while(arduino.is_connected()){
			
			system("clear");
			std::cout << "frontL: " << USData0 << std::endl;
			std::cout << "frontR: " << USData1 << std::endl;
			std::cout << "left:   " << IRData0 << std::endl;
			std::cout << "right:  " << IRData1 << std::endl;
	switch(currentState){
			case STATE_FORWARD:
					std::cout << currentState << std::endl;
					// tell arduino to move forward;
					arduino.write_request(controlUUID, directionUUID, forward);
					// turn left or right based on sensor input
					if((USData0 < 15) || (USData1 < 15)){
						// go backwards if there are walls on both sides
						if((IRData0 < 15) && (IRData1 < 15)) {
								currentState = STATE_BACKWARD;
							//otherwise turn to the more open side
						} else{
							currentState = (IRData0 < IRData1) ? STATE_RIGHT : STATE_LEFT;
						}
					}
					break;
			case STATE_LEFT:
					std::cout << currentState << std::endl;
					// wait until there is more space to move forward
					arduino.write_request(controlUUID,directionUUID, left);
					if((USData0 > 40) || (USData1 > 40)) {
							currentState = STATE_FORWARD;
					}
					break;
			case STATE_RIGHT:
					std::cout << currentState << std::endl;
					// wait until there is more space to move forward
					arduino.write_request(controlUUID,directionUUID,right);
					if((USData0 > 40) || (USData1 > 40)) {
							currentState = STATE_FORWARD;
					}
					break;				
			case STATE_BACKWARD:
					std::cout << currentState << std::endl;
					arduino.write_request(controlUUID,directionUUID,reverse);
					if((IRData0 > 15) || (IRData1 > 15)) {
						currentState = (IRData0 < IRData1) ? STATE_RIGHT : STATE_LEFT;	
					}
					break;
			case STATE_STOP:
					std::cout << currentState << std::endl;
					if((USData0 > 15) || (USData1 > 15)) {
							currentState = STATE_FORWARD;
					} else {
							currentState = (IRData0 < IRData1) ? STATE_RIGHT : STATE_LEFT;
					}
					break;
			default:
					currentState = STATE_STOP;
					break;
	}
	}
}
