#include <mbed.h>

mbed::I2C i2c(P0_31, P0_2);

// this is the command to send to the multiplexer, the 4 sensors are selected with binary 0b0001, 0b0010, 0b0100, and 0b1000
// so 0x01, 0x02, 0x04, and 0x08 in hexidecimal
char mux_cmd = 0x01;
// this is the address of the multiplexer
char mux_addr = 0xEE;
// byte array to send commands to the sensor
char cmd[2];
// byte array to read commands. For some reason he uses the same array to read and write in the lab script
// but this is much more readable and maintainable
char bytes[2];
void setup() {
  // selects the 'Distance' address in the sensor's register
  cmd[0] = 0x5e;
  // selects 'Bank0' in the sensor's register, this is where the distance is stored
  // this isn't strictly neccessary, since it defaults to 0x00
  cmd[1] = 0x00;
  Serial.begin(9600);
}

void loop() {
  // sends 0x01 to the multiplexer, telling it to use sensor 1
  // the 1 at the end tells the i2c how many bytes are being sent
  i2c.write(mux_addr, &mux_cmd, 1);
  // 0x80 is the address of the sensor, cmd is the command byte array telling it to get the distance data ready (i think, i'm not sure of this part)
  i2c.write(0x80, cmd, 2);
  wait_us(250000);
  // reads the distance data into the bytes array
  i2c.read(0x80, bytes, 2);
  // data is recieved in two bytes so we have to shift one to the left 8 bits (1 byte) and add to combine them
  // [10101010, 10101010] -> 1010101010101010 etc.
  char data = (bytes[1] << 8) + bytes[0];
  Serial.println(data, DEC);
}
