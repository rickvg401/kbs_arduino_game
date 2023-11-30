#define F_CPU 16000000ul

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <avr/interrupt.h>
#include <Nunchuk.h>

#define nunchuk_ID 0xA4 >> 1

#define TFT_DC 9
#define TFT_CS 10

const int GAMECLOCK = 15; //ball updates every x times per second

const uint16_t FIELD_WIDTH = 32;
const uint16_t FIELD_HEIGHT = 24; // could be uint8_t
const uint8_t BLOCK_SIZE = 10;
const uint8_t FIELD_DIVISION = 8;
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setupTimer();
void drawField(uint8_t [FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION]);
void walkPlayer(uint8_t, uint8_t);
void drawPlayer(uint16_t color);
bool canWalk(uint8_t, uint8_t);
void setSurrounding(uint8_t, uint8_t);

const uint16_t PLAYER_COLOR = ILI9341_RED;
const uint16_t WALL_COLOR = ILI9341_BLUE;
const uint16_t BLACK = ILI9341_BLACK;

enum direction {NORTH, EAST, SOUTH, WEST};



uint8_t surrounding[4];

uint8_t field[FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION] = {
{0xFF,0xFF,0xFF,0xFF,},
{0x80,0x10,0x10,0x01,},
{0x80,0x10,0x10,0x01,},
{0xBF,0xDD,0xC7,0xFD,},
{0xA0,0x40,0x40,0x05,},
{0xA9,0x1F,0x7E,0x85,},
{0xA4,0x00,0x20,0x85,},
{0xA4,0x3C,0x2C,0x85,},
{0x84,0x00,0x20,0x85,},
{0x82,0x00,0x20,0x85,},
{0xF4,0x3E,0x2E,0x01,},
{0x84,0x03,0xFF,0xF1,},
{0xBC,0x00,0x00,0x01,},
{0x84,0x30,0x06,0x01,},
{0xF4,0x20,0x39,0xE1,},
{0x94,0x00,0x00,0x15,},
{0x94,0x00,0x00,0x15,},
{0xB7,0x7F,0xFE,0xF5,},
{0x80,0x00,0x00,0x95,},
{0x80,0x00,0x00,0x11,},
{0x9F,0x6E,0xFF,0x91,},
{0x80,0x00,0x00,0x11,},
{0x80,0x00,0x01,0x11,},
{0xFF,0xFF,0xFF,0xFF,},
};

uint8_t playerX = 1;
uint8_t playerY = 1;

volatile direction cd = EAST;

int main()
{
  setupTimer();

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(BLACK);

  drawField(&field[0]);
  drawPlayer(PLAYER_COLOR);
  sei();

  Wire.begin();
  if(!Nunchuk.begin(nunchuk_ID)){
    return(1);
  }
  while(1)
  {
    if(!Nunchuk.getState(nunchuk_ID))
    {
      return (1);
    } 
    if (Nunchuk.state.joy_y_axis > 200)
      cd = SOUTH;
    else if (Nunchuk.state.joy_x_axis > 200)
      cd = EAST;
    else if (Nunchuk.state.joy_y_axis < 50)
      cd = NORTH;
    else if (Nunchuk.state.joy_x_axis < 50)
      cd = WEST;
  }
}

uint8_t getTileAt (uint8_t x, uint8_t y)
{
  return (field[y][x / 8] & (1 << (7-(x % 8))) ? 1 : 0);
}

void setupTimer()
{
  uint16_t c = ((F_CPU / 1024) / GAMECLOCK ) - 1;

  TCCR1B = (1 << WGM12);
  OCR1AH = (c >> 8);
  OCR1AL = c;
  TIMSK1 = (1 << OCIE1A);
  TCCR1B |= (1 << CS12) | (1 << CS10);
}


ISR(TIMER1_COMPA_vect)
{
  switch (cd)
  {
  case NORTH:
    if (canWalk(playerX, playerY+1))
    {
       drawPlayer(BLACK);
      playerY += 1;
      drawPlayer(PLAYER_COLOR);
    }
    break;

  case EAST:
    if (canWalk(playerX+1, playerY))
      {
      drawPlayer(BLACK);
      playerX += 1;
      drawPlayer(PLAYER_COLOR);
      }
    break;

  case SOUTH:
    if (canWalk(playerX, playerY-1))
      {
      drawPlayer(BLACK);
      playerY -= 1;
      drawPlayer(PLAYER_COLOR);
      }
    break;

  case WEST:
    if(canWalk(playerX-1, playerY))
      {
      drawPlayer(BLACK);
      playerX -= 1;
      drawPlayer(PLAYER_COLOR);
      }
    break;
  }
}

void drawField(uint8_t field[FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION])
{
  for (uint16_t y = 0; y < FIELD_HEIGHT; y++)
  {
    for (uint16_t x = 0; x < FIELD_WIDTH; x++)
    {
      uint8_t fieldType = getTileAt(x, y);
      if (fieldType != 0)
        tft.drawRect(x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, WALL_COLOR);
    }
  }
   
}


void drawPlayer(uint16_t color)
{
  tft.fillRect(playerX*BLOCK_SIZE, playerY*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, color);
}

void setSurrounding(uint8_t x, uint8_t y)
{
  surrounding[0] = getTileAt(x, y+1); //north
  surrounding[1] = getTileAt(x+1, y); //east
  surrounding[2] = getTileAt(x, y-1); //south
  surrounding[3] = getTileAt(x-1, y); //west
}

bool canWalk(uint8_t x, uint8_t y)
{
  if(getTileAt(x, y) != 0)
    return false;
  return true;
}