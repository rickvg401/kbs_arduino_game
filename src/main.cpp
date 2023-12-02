#include <avr/interrupt.h>
#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#include "SPI.h"
#include "display/Adafruit-GFX/Adafruit_GFX.h"
#include "display/Adafruit_ILI9341.h"
#include <util/delay.h>


//serial
#define SerialActive //if defined serial is active
#define BAUDRATE 9600

//nunchuk
#define nunchuk_ID 0xA4 >> 1
// unsigned char buffer[4];// array to store arduino output
uint8_t NunChuckPosition[4];
bool NunChuckPositionDivided = false;

/*display*/
// 240x320 pixels
#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//game
uint16_t playerPosX;
uint16_t playerPosY;

enum direction {
  up,right,down,left
};
uint8_t thickness = 10;




//game level
uint16_t** mazes(){
  const uint16_t segments = 20;

  const uint8_t WallCount = 225;
  const uint8_t WallDescriptionLength = 4;
  uint16_t** mazing = new uint16_t*[WallCount];
  for(int i = 0;i<WallCount;i++){
    mazing[i] = new uint16_t[WallDescriptionLength];
  }

  int mI = 0;
  
  //game level
  mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;
  mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;
  mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;
  mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;
  mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;
  mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;
  mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;mazing[mI][0] = 100;mazing[mI][1] = 40;mazing[mI][2] = right;mazing[mI][3] = 150;mI++;
  

  return mazing ;
}
  const uint8_t WallCount = 50;
  const uint8_t WallDescriptionLength = 4;
// uint16_t maze[1][4] = {{100,40,right,150}};
// uint16_t maze[WallCount][WallDescriptionLength] = {
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70},
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70},
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70},
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70},
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70},
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70},
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70},
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70},
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70},
//   {100,40,right,150},{0,0,up,150},{160,10,up,150},{10,160,left,150},{200,200,down,70}
// };

void deleteAll(uint16_t** deleted){


}

uint16_t** maze = mazes();


// uint16_t** maze2 = mazes();
//drawing start(x0,y0) in left bottom


// 7 segment display
#define PCF8574_i2cAdr 0x21
void PCF8574_write(byte bytebuffer);
byte PCF8574_read();
byte bits;

// boolean voor het printen straks niet meer nodig
volatile bool flag = false;

// infrared vars
volatile uint16_t prevCounterValue = 0;
volatile uint16_t currentCounterValue;
volatile uint16_t pulseDuration;
volatile uint8_t eenofnull;
volatile uint16_t counter = 0;

bool setupDisplay();
bool setupNunchuck();
void drawPlayer(uint16_t x, uint16_t y);
void drawPath(uint16_t x, uint16_t y);
void movePlayer(uint16_t newX,uint16_t newY);
void movePlayerNunchuk();
void drawLevel();
void getNunchukPosition();
void PCF8574_write(byte bytebuffer);

void drawMaze();
void drawWall(uint16_t x,uint16_t y,uint16_t direction,uint16_t length);

// bool pointCollision(uint16_t x,uint16_t y,uint8_t margin);


void sendNEC(uint8_t data) {
  uint8_t data1 = data;

    // for (uint8_t i = 0; i < 8; i++)
    // {

      // if (data1 & (0x01)) voor bytes
      if(data1 == 1)
      {
        // Verzend een logische '1'
        TCCR0A |= (1<< COM0A0);
        PORTD |= (1<< PD6);
        _delay_us(1700); // houd dit bit voor 1700 us aan // BUSY WAITING WORDT VERANDERT IN SPRINT 2

      }
      else
      {
        // Verzend een logische '0'
        TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
        PORTD |= (1<< PD6); // pin naar high zetten
        _delay_us(700); // houd dit bit voor 700 us aan // BUSY WAITING WORDT VERANDERT IN SPRINT 2

      }
      // data1 = data1 >> 1; // voor bytes
      // Verzend een logische '0'

        TCCR0A &= ~(1<< COM0A0);
        PORTD &= ~(1<<PD6);
    // }
    Serial.println(" einde ");
  
}

ISR(INT0_vect)
{
  uint8_t pinstatus = (PIND & (1<<PD2));
  
 if(pinstatus)
 {

    
    // opgaande flank bepaal je het verschil tussen huidige counterstand en de onthouden counterstand
    // Je hebt dan gemeten hoe lang de puls duurde. Daarmee kan je
    // bepalen of het uitgezonden bit een 0 of een 1 was. Schuif dit
    // bit in het resulterende buffer.
     currentCounterValue = counter;
     pulseDuration = currentCounterValue - prevCounterValue;
    if(pulseDuration > 169 && pulseDuration < 178) {
      // Voeg '1' toe aan buffer
      flag = true;
      eenofnull = 1;
    } else if (pulseDuration < 76 && pulseDuration > 70) {
      // Voeg '0' toe aan buffer
      flag = false;
      eenofnull = 0;
    }

 } else {
    
    
    // bij een neergaande flank onthouden counter van timer 1
    prevCounterValue = counter;
    eenofnull = -1;
 }
}

ISR(TIMER0_COMPA_vect)
{
  // counter altijd ophogen 
  // counter hier altijd ophogen 
  counter++;
  if(counter > 100000){
    counter = 0;
  }
}

void initIR()
{
    DDRD |= (1<<DDD6); // ir led op gameshield

    // TCCR1A &= ~(1<<COM1A0); // disable  -> toggle OC1A pin 9 on compare match

    // timer 0 voor irled
    OCR0A = 208;
    TCCR0A |= (1<<WGM01);
    TCCR0B |= (1<<CS00); // no prescaler
    TIMSK0 |= (1<<OCIE0A); // enable timer compare interrupt

    TCCR0A &= ~(1<<COM0A0);// toggle OC0A pin 6

    // external interrupt
    EIMSK |= (1<<INT0); // int0 external interrupt aan
    EICRA |= (1<<ISC00); // any logical change generate interrupt

}


bool setupDisplay(){
    //returned if setup is correctly completed
    tft.begin();
    return true;
}

bool setupNunchuck(){
    if(!Nunchuk.begin(nunchuk_ID)){
        #ifdef SerialActive
        Serial.println("******** No nunchuk found");
        Serial.flush();
        #endif
        return(false);
    }
    #ifdef SerialActive
    Serial.print("-------- Nunchuk with Id: ");
	Serial.println(Nunchuk.id);
    #endif
    return true;
}


void drawMaze(){
// for(uint8_t i=0;i<sizeof(maze) / (sizeof(uint16_t)*maxWallDescriptionLength);i++){


for(uint8_t i=0;i<WallCount;i++){
  drawWall(maze[i][0],maze[i][1],maze[i][2],maze[i][3]);
}


}





void drawWall(uint16_t x,uint16_t y,uint16_t dir,uint16_t length){
  if(dir == right){
    tft.fillRect(y,x,thickness,length,ILI9341_BLUE);
  }
  if(dir==up){
    tft.fillRect(y,x,length,thickness,ILI9341_YELLOW);
  }
  if(dir==down){
    tft.fillRect(y-length,x,length,thickness,ILI9341_GREEN);
  }

  if(dir==left){
    tft.fillRect(y,x-length,thickness,length,ILI9341_PINK);
  }
}



bool collision(uint16_t posX,uint16_t posY){
  for(uint8_t i=0;i<WallCount;i++){
    // Serial.println(" ");
    // Serial.println("collision check");
    bool colX = false;//collision x axis
    bool colY = false;//collision y axis

    uint16_t mzX = maze[i][0];
    uint16_t mzY = maze[i][1];
    uint16_t mzD = maze[i][2];
    uint16_t mzL = maze[i][3];
    uint8_t t = thickness;

    // if(mzD == right){
      bool t1 = posX>mzX;
      bool t2 = posX<(mzX+mzL);

      if(t1 && t2 ){
        colX = true;
      }
      // Serial.print("x");
      // Serial.print(t1);
      // Serial.print(":");
      // Serial.print(t2);
      // Serial.println(" ");

      bool t3 = posY>mzY;
      bool t4 = posY<(mzY+t);
      if(t3 && t4){
        colY = true;
      }
      // Serial.print("Y");
      // Serial.print(t3);
      // Serial.print(":");
      // Serial.print(t4);
      // Serial.println(" ");

    

    // Serial.print("posX");
    // Serial.println(posX);
    // Serial.print("posY");
    // Serial.println(posY);

    // Serial.print("mzX");
    // Serial.println(mzX);
    // Serial.print("mzY");
    // Serial.println(mzY);
    // Serial.print("mzD");
    // Serial.println(mzD);
    // Serial.print("mzL");
    // Serial.println(mzL);
    // Serial.print("t");
    // Serial.println(t);


    // Serial.print("colX");
    // Serial.println(colX);
    // Serial.print("colY");
    // Serial.println(colY);

    // _delay_ms(100);

    if(colX && colY){
      Serial.println("colli");
      return true;
    }
  }


  return false;
}


void drawPlayer(uint16_t x, uint16_t y){
    tft.fillRect(y,x,25,25,ILI9341_DARKCYAN);//x&y corrected
    playerPosX = x;
    playerPosY = y;
}

void drawPath(uint16_t x, uint16_t y){
    tft.fillRect(y,x,25,25,ILI9341_RED);//x&y corrected
}

void movePlayer(uint16_t newX,uint16_t newY){
    drawPath(playerPosX,playerPosY);
    drawPlayer(newX,newY);
}


void movePlayerNunchuk(){

  uint16_t newX = playerPosX + ((NunChuckPosition[0]-128)/100*1);
  uint16_t newY = playerPosY + ((NunChuckPosition[1]-128)/100*1);
  if(!(playerPosX==newX && playerPosY==newY)){
    drawPath(playerPosX,playerPosY);
    drawPlayer(newX,newY);
  }
}

/*bool pointCollision(uint16_t x,uint16_t y , uint8_t margin){
  uint16_t posx = 100;
  uint16_t posY = 200;


  bool stateX = false;
  bool stateY = false;
  if(x+margin>posx && x-margin<posx){
     stateX = true;
  }
  if(y+margin>posY && y-margin<posY){
    stateY = true; 
  }
  if(stateX && stateY){
    return true;
  }


  return ;
}


*/


void drawLevel(){
  tft.fillScreen(ILI9341_RED);
  drawMaze();
  drawPlayer(140,10);
}


void getNunchukPosition(){
    if(!Nunchuk.getState(nunchuk_ID)){
        return;
    } 
    
    uint8_t x = Nunchuk.state.joy_x_axis;
    uint8_t y = Nunchuk.state.joy_y_axis;
    // //flipped
    // uint8_t x = Nunchuk.state.joy_y_axis;
    // uint8_t y = Nunchuk.state.joy_x_axis;
    
    

    uint8_t c = Nunchuk.state.c_button;
    uint8_t z = Nunchuk.state.z_button;
    
    

    if(NunChuckPositionDivided){
        if(x != 128){
            NunChuckPosition[0] = x<128 ? 50 : 200;}
        else{
            NunChuckPosition[0] = 128;
        }
        if(y != 128){
                NunChuckPosition[1] = y>128 ? 50 : 200;} 
        else{
                NunChuckPosition[1] = 128;
        }
        NunChuckPosition[2] = c ? 0 : 1;
        NunChuckPosition[3] = z ? 0 : 1;


    }else{
        NunChuckPosition[0] = x;
        NunChuckPosition[1] = y;
        NunChuckPosition[2] = c;
        NunChuckPosition[3] = z;
    }

    
    
}

void PCF8574_write(byte bytebuffer)
{
  Wire.beginTransmission(PCF8574_i2cAdr);
  Wire.write(bytebuffer);
  Wire.endTransmission();
}


int main(void)
{
    sei();
    initIR();
    Serial.begin(BAUDRATE);
    Wire.begin();

    if(!setupNunchuck()){return 0;}
    if(!setupDisplay()){return 0;}


    drawLevel();

    while(1)
    {
        // uint8_t data = 0b00001101;
        // sendNEC(data); // Voorbeeld: Verstuur een testsignaal met waarde 0x00FF  
        
      // nunchuck en display
        getNunchukPosition();
        /*if(NunChuckPosition[2])
        {
          sendNEC(1);
        } else if(NunChuckPosition[3]) {
          sendNEC(0);
        }
        if(eenofnull == 1)
        {
          PCF8574_write(0b11111111);
        } else if(eenofnull == 0) {
          PCF8574_write((0b00000000));
        } */

        // Serial.println(pulseDuration);
        collision(playerPosX,playerPosY);

        movePlayerNunchuk();
        
    } 
    return 0;
}
