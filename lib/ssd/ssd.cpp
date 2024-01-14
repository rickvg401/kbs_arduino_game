#include "ssd.h"
#include <Wire.h>

uint8_t numToByte[10] = {
    0b11000000,
    0b11111001,
    0b10100100,
    0b10110000,
    0b10011001,
    0b10010010,
    0b10000010,
    0b11111000,
    0b10000000,
    0b10010000,
};

void showNum(uint8_t num)
{
  num = num % 10;

  Wire.beginTransmission(0x21);
  Wire.write(numToByte[num]);
  Wire.endTransmission();
}