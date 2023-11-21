#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#define nunchuk_ID 0xA4 >> 1
unsigned char buffer[6];// array to store arduino output

int main(){
NunChuk.begin(nunchuk_ID);

}