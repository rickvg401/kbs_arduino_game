/********************************************************************************
 * Code for read data from Wii nunchuck., base on http://www.windmeadow.com/node/42
 * File : ArduinoNunchuk.pde
 * by : K.Worapoht
 * Hardware: Arduino, POP-168 , AVR ATMega168 ,
 * connect SDA to PC4 (An4) and SCL PC5 (An5)
 *******************************************************************************/
#include <Wire.h>
#define nunchuk_ID 0xA4 >> 1
unsigned char buffer[6];// array to store arduino output
int cnt = 0;
void setup ()
{
 Serial.begin (9600);
 Wire.begin (); // join i2c bus with address 0x52
 nunchuck_init (); // send the initilization handshake
 delay (100);
}
void nunchuck_init ()
{
 Wire.beginTransmission (nunchuk_ID); // transmit to device 0x52
 Wire.send (0x40); // sends memory address
 Wire.send (0x00); // sends sent a zero.
 Wire.endTransmission (); // stop transmitting
}
void send_zero ()
{
 Wire.beginTransmission (nunchuk_ID); // transmit to device 0x52
 Wire.send (0x00); // sends one byte
 Wire.endTransmission (); // stop transmitting
}
void loop ()
{
 Wire.requestFrom (nunchuk_ID, 6); // request data from nunchuck
 while (Wire.available ())
 {
 buffer[cnt] = nunchuk_decode_byte (Wire.receive ()); // receive byte as an integer
 cnt++;
 }
 // If we recieved the 6 bytes, then go print them
 if (cnt >= 5)
 {
 print ();
 }
 cnt = 0;
 send_zero (); // send the request for next bytes
 delay (100);
}
// Print the input data we have recieved
// accel data is 10 bits long
// so we read 8 bits, then we have to add
// on the last 2 bits.
void print ()
{
 unsigned char joy_x_axis;
 unsigned char joy_y_axis;
 int accel_x_axis;
 int accel_y_axis;
 int accel_z_axis;
 unsigned char z_button;
 unsigned char c_button;
 joy_x_axis = buffer[0];
 joy_y_axis = buffer[1];
 accel_x_axis = (buffer[2]) << 2;
 accel_y_axis = (buffer[3]) << 2;
 accel_z_axis = (buffer[4]) << 2;
 // byte outbuf[5] contains bits for z and c buttons
 // it also contains the least significant bits for the accelerometer data
 // so we have to check each bit of byte outbuf[5]
 if ((buffer[5] & 0x01)!=0)
 { z_button = 1; }
 else
 { z_button = 0; }
 if ((buffer[5] & 0x02)!=0)
  { c_button = 1; }
 else
 { c_button = 0; }
 accel_x_axis += ((buffer[5]) >> 2) & 0x03;
 accel_y_axis += ((buffer[5]) >> 4) & 0x03;
 accel_z_axis += ((buffer[5]) >> 6) & 0x03;
 Serial.print (joy_x_axis, DEC);
 Serial.print ("\t");
 Serial.print (joy_y_axis, DEC);
 Serial.print ("\t");
 Serial.print (accel_x_axis, DEC);
 Serial.print ("\t");
 Serial.print (accel_y_axis, DEC);
 Serial.print ("\t");
 Serial.print (accel_z_axis, DEC);
 Serial.print ("\t");
 Serial.print (z_button, DEC);
 Serial.print ("\t");
 Serial.print (c_button, DEC);
 Serial.print ("\r\n");
}
// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char nunchuk_decode_byte (char x)
{
 x = (x ^ 0x17) + 0x17;
 return x;
}