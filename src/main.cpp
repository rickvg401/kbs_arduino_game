#define F_CPU 16000000ul

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <avr/interrupt.h>

#define TFT_DC 9
#define TFT_CS 10

const int GAMECLOCK = 60; //ball updates every x times per second

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setupTimer();

uint16_t MAX_WIDTH = 320;
uint16_t MAX_HEIGHT = 240;

uint16_t ballX = MAX_WIDTH / 2;
uint16_t ballY = MAX_HEIGHT / 2;
uint8_t ballWidth = 10;
uint8_t ballHeight = 10;

int8_t velX = 1;
int8_t velY = 1;

const uint16_t BALL_COLOR = ILI9341_BLUE;
const uint16_t BLACK = ILI9341_BLACK;

int main()
{
  setupTimer();
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  sei();
  while(1);
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
  tft.fillRect(ballX, ballY, ballWidth, ballHeight, BLACK); // Redraw
  if (ballX + ballWidth > MAX_WIDTH)
    velX *= -1;
  else if (ballX < 0)
    velX *= -1;
  
  if (ballY + ballHeight > MAX_HEIGHT)
    velY *= -1;
  else if (ballY < 0)
    velY *= -1;
  
  ballX += velX;
  ballY += velY;

  tft.fillRect(ballX, ballY, ballWidth, ballHeight, BALL_COLOR); // Redraw
}
