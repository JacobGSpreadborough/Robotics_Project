#include "simpleble/Adapter.h"
#include "simpleble/Exceptions.h"
#include "simpleble/Peripheral.h"
#include "simpleble/Types.h"
#include "iostream"
#include <sys/signal.h>

#define US_0_OFFSET 0
#define US_1_OFFSET 4
#define IR_0_OFFSET 8
#define IR_1_OFFSET 10
#define X_POSITION_OFFSET 0
#define Y_POSITION_OFFSET 4
#define ANGLE_OFFSET 8
#define MINIMUM_FRONT_DISTANCE 20
#define PI 3.14159

// address for my arduino
SimpleBLE::BluetoothAddress arduinoAddress = "6CFC6917-94B4-03DA-C5BC-9BC05CA2496B";
SimpleBLE::BluetoothUUID controlUUID = "19b10000-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothUUID directionUUID = "19b10001-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothUUID sensorUUID = "19b10002-e8f2-537e-4f6c-d104768a1214";
SimpleBLE::BluetoothUUID positionUUID = "19b10003-e8f2-537e-4f6c-d104768a1214";
// commands for the adruino
// TODO figure out how to do this better
SimpleBLE::ByteArray stop = SimpleBLE::ByteArray::fromHex("00");
SimpleBLE::ByteArray forward = SimpleBLE::ByteArray::fromHex("01");
SimpleBLE::ByteArray left = SimpleBLE::ByteArray::fromHex("03");
SimpleBLE::ByteArray right = SimpleBLE::ByteArray::fromHex("02");
SimpleBLE::ByteArray reverse = SimpleBLE::ByteArray::fromHex("04");

enum RobotState {
	STATE_STOP = 0,
	STATE_FORWARD,
	STATE_LEFT,
	STATE_RIGHT,
	STATE_BACKWARD
}currentState;

// map in cm
unsigned char map[201][146];

unsigned short IRData0, IRData1 = 65;
float USData0, USData1 = 0;
float xPosition = 73;
float yPosition = 17;
float angle = 0;

void polarToCartesian(float theta, float r, float *x, float *y)  {
	*x = (r * cos(theta * PI/180));	   	
	*y = (r * sin(theta * PI/180));
}
void populateMap(float theta, float distance) {
	float xObstacle;
	float yObstacle;
	// get x and y coordinates from heading and sensor reading
	polarToCartesian(theta, distance, &xObstacle, &yObstacle);
	// adjust for the robot's position
	xObstacle += xPosition;
	yObstacle += yPosition;
	system("clear");
	std::cout << "obstacle at: " << std::endl;
	std::cout << "x: " << xObstacle << std::endl;
	std::cout << "y: " << yObstacle << std::endl;
	map[(int)xObstacle][(int)yObstacle] = 1;
	
}
int bytesToInt(SimpleBLE::ByteArray bytes) {
		return (*(int*)(bytes.data()));
}

int main(int argc, char* argv[]) { 

	map[17][73] = '#';

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

	arduino.notify(controlUUID,sensorUUID,[](SimpleBLE::ByteArray bytes) {
					// TODO extract the type conversion to a function
					USData0 = (*(float*)(bytes.data() + US_0_OFFSET));
					USData1 = (*(float*)(bytes.data() + US_1_OFFSET));
					USData0 = (USData0 * 0.0343)/2;
					USData1 = (USData1 * 0.0343)/2;
					if(USData0 > 120 || USData1 > 120) {
						USData0 = 0;
						USData1 = 0;
					}
					IRData0 = (*(unsigned short*)(bytes.data() + IR_0_OFFSET));
					IRData1 = (*(unsigned short*)(bytes.data() + IR_1_OFFSET));
					IRData0 >>= 10;
					IRData1 >>= 10;
					//populateMap(angle, USData0);
					//populateMap(angle, USData1);
				//	populateMap((angle + 90), float(IRData0));
				//	populateMap((angle - 90), float(IRData1));
		});
	arduino.notify(controlUUID,positionUUID,[](SimpleBLE::ByteArray bytes) {
					xPosition = (*(float*)(bytes.data() + X_POSITION_OFFSET));
					yPosition = (*(float*)(bytes.data() + Y_POSITION_OFFSET));
					angle = (*(float*)(bytes.data() + ANGLE_OFFSET));
		});
    while(arduino.is_connected()){	
			if(xPosition >= 180) {
					std::cout << "maze completed" << std::endl;
					return 0;
			}
			system("clear");
			std::cout << "frontL: " << USData0 << std::endl;
			std::cout << "frontR: " << USData1 << std::endl;
			std::cout << "left:   " << IRData0 << std::endl;
			std::cout << "right:  " << IRData1 << std::endl;
			std::cout << "x:      " << xPosition << std::endl;
			std::cout << "y:      " << yPosition << std::endl;
			std::cout << "angle:  " << angle << std::endl;
			std::cout << "state:  " << currentState << std::endl;
		switch(currentState){
			case STATE_FORWARD:
					// tell arduino to move forward;
					arduino.write_request(controlUUID, directionUUID, forward);
					// turn left or right based on sensor input
					if((USData0 < MINIMUM_FRONT_DISTANCE) || (USData1 < MINIMUM_FRONT_DISTANCE)){
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
					// wait until there is more space to move forward
					arduino.write_request(controlUUID,directionUUID, left);
					if(USData0 > 35) {
							currentState = STATE_FORWARD;
					}
					break;
			case STATE_RIGHT:
					// wait until there is more space to move forward
					arduino.write_request(controlUUID,directionUUID,right);
					if(USData1 > 35) {
							currentState = STATE_FORWARD;
					}
					break;				
			case STATE_BACKWARD:
					arduino.write_request(controlUUID,directionUUID,reverse);
					if((IRData0 > 15) || (IRData1 > 15)) {
						currentState = (IRData0 < IRData1) ? STATE_RIGHT : STATE_LEFT;	
					}
					break;
			case STATE_STOP:
					arduino.write_request(controlUUID,directionUUID,stop);
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
	for(int i=0;i<201;i++){
			std::cout << i << ": ";
			for(int j=0;j<146;j++){
					std::cout << (map[i][j] ? "#" : "_");
			}
			std::cout << std::endl;
	}
	return 0;
}
