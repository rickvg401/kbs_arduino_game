#include <avr/interrupt.h>
#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#include "SPI.h"
#include "display/Adafruit-GFX/Adafruit_GFX.h"
#include "display/Adafruit_ILI9341.h"
#include <EEPROM.h>
#include <Fonts/PressStart2P_vaV76pt7b.h>
#include <util/delay.h>
#include <sound.h>
#include <notes.h>

// #include "SPI.h"
// #include "Adafruit_GFX.h"
// #include "Adafruit_ILI9341.h"
// #include <avr/interrupt.h>
// #include <Nunchuk.h>

//general
#define F_CPU 16000000ul
#define _x_ 0
#define _y_ 1

//serial
#define SerialActive //if defined serial is active
#define BAUDRATE 9600

// Arduino IR address
uint8_t address1 = 0b1010;
uint8_t address2 = 0b1101;

//nunchuk
#define nunchuk_ID 0xA4 >> 1

// unsigned char buffer[4];// array to store arduino output
uint8_t NunChuckPosition[4];
bool NunChuckPositionDivided = false;

/*display*/
#define TFT_DC 9
#define TFT_CS 10
#define HIGHSCORECOLOR ILI9341_YELLOW //color of highscore text
#define PLAYERNAME "Jurre" //player name for adding highscores
char nameList [10][6] = {}; //list to store playernames from EEPROM
uint32_t scoreList [10] = {};//list to store scores from EEPROM
int posList[10] = {0,1,2,3,4,5,6,7,8,9}; //list to keep track of indexes in EEPROM
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//game
const int GAMECLOCK = 15; //ball updates every x times per second
uint16_t playerPosX;
uint16_t playerPosY;
uint16_t* playerVector = NULL;

const uint16_t FIELD_WIDTH = 32;
const uint16_t FIELD_HEIGHT = 24; // could be uint8_t
const uint8_t BLOCK_SIZE = 10;
const uint8_t FIELD_DIVISION = 8;

#define PLAYER_COLOR TFT_YELLOW
#define GHOST_COLOR TFT_INDIANRED
#define WALL_COLOR ILI9341_BLUE
#define BACKGROUND  ILI9341_BLACK
#define COIN_COLOR  TFT_GOLD

const uint8_t playerSpeed = 1*BLOCK_SIZE;

// 7 segment display
#define PCF8574_i2cAdr 0x21
void PCF8574_write(byte bytebuffer);
byte PCF8574_read();
byte bits;
    
struct music pacmanTheme;
namespace notes
{
  uint16_t pacmanNotes[] = {_C4, _C5, _G4, _E4, _C5, _G4, _C4S, _C5S, _G4S, _F4, _G4S, _F4, _C4, _C4, _C5, _G4, _E4, _C5, _G4, _E4, _D4, _D4S,_E4, _E4, _F4, _F4S, _F4, _F4S, _G4, _C5}; 
  uint16_t pacmanDurations[sizeof(pacmanNotes) / sizeof(uint16_t)] = {300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
}
    

// infrared vars
volatile int16_t prevCounterValue = 0;
volatile int16_t currentCounterValue = 0;
volatile int16_t pulseDuration = 0;
volatile uint8_t eenofnull = 2;
volatile uint16_t counter = 0;
volatile uint16_t counter69 = 0;


// ir 
volatile bool logicalone = false;
volatile bool logicalzero = false;
volatile bool logicalend = false;
volatile bool logicalbegin = false;

volatile bool end = true;

volatile uint16_t currentcounterone = 0;
volatile uint16_t currentcounterzero = 0;

volatile uint16_t currentcounterend = 0;
volatile uint16_t currentcounterbegin = 0;

// buffer
// uint16_t bufferResult = 0;
volatile uint8_t bufferIndex = 0; // Huidige index in de buffer
volatile uint16_t buffer = 0;
volatile uint8_t bufferdata = 0; // buffer zonder address bits


bool setupDisplay();
bool setupNunchuck();
void drawPlayer(uint16_t x, uint16_t y);
void drawPath(uint16_t x, uint16_t y);
void movePlayer(uint16_t newX,uint16_t newY);
void movePlayerNunchuk();
void drawLevel();
void getNunchukPosition();
void PCF8574_write(byte bytebuffer);
void addToBuffer();
uint8_t nunchuckWrap();
void moveOverIR();
void setupTimer();
void drawField(uint8_t [FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION]);
void walkPlayer(uint8_t, uint8_t);
void drawPlayer(uint16_t color);
void drawCoins();
void drawGhosts();
bool canWalk(uint8_t, uint8_t);
void setSurrounding(uint8_t, uint8_t);

void HighScorePage();
bool setupHighScore();
void getScore();
void setupScore();
void printHighScore(char names[10][6],uint32_t scores[10]);
void sort(char names[10][6],uint32_t scores[10], int pos[10]);
void addScore(char name[6], uint32_t points);




enum direction {NORTH, EAST, SOUTH, WEST};



uint8_t surrounding[4];


const uint8_t COINS_LENGTH  = 45;
const uint8_t GHOSTS_LENGTH = 10;
const uint8_t PLAYER_LENGTH = 12;
uint8_t coins[45][2] = {
{2,12},
{2,11},
{3,12},
{3,13},
{4,12},
{5,12},
{5,13},
{4,11},
{4,10},
{3,10},
{3,9},
{4,8},
{3,8},
{4,1},
{6,1},
{6,2},
{5,3},
{8,2},
{6,5},
{14,3},
{13,7},
{12,1},
{13,5},
{20,13},
{26,3},
{21,14},
{19,9},
{25,15},
{17,4},
{26,9},
{23,7},
{20,12},
{21,7},
{16,11},
{15,15},
{13,17},
{13,18},
{6,21},
{5,17},
{18,19},
{21,19},
{24,21},
{25,22},
{27,17},
{19,17},
};
uint8_t players[12][2] = {
{27,22},
{27,21},
{28,21},
{29,21},
{29,22},
{28,22},
{28,20},
{29,20},
{29,19},
{27,19},
{27,20},
{28,19},
};
uint8_t ghosts[10][2] = {
{5,11},
{3,11},
{4,13},
{6,13},
{8,12},
{7,11},
{2,13},
{2,10},
{4,9},
{6,9},
};
uint8_t field[FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION] = {
{0xFF,0xFF,0xFF,0xFF,},
{0x80,0x00,0x00,0x01,},
{0x80,0x00,0xE4,0x01,},
{0xF0,0x01,0x84,0x01,},
{0xE0,0x71,0x04,0x01,},
{0xE0,0x51,0x8C,0xC1,},
{0xE0,0x10,0xFC,0x63,},
{0xEE,0x10,0xC0,0x43,},
{0xE0,0x10,0x00,0x43,},
{0xE1,0x10,0x00,0x03,},
{0xC1,0x90,0x01,0xC3,},
{0xC0,0x10,0x01,0xF9,},
{0xC0,0x13,0x81,0xF9,},
{0xC0,0x14,0x41,0xF9,},
{0xFF,0xF0,0x79,0xC1,},
{0x80,0x00,0x00,0x01,},
{0x80,0x80,0x00,0x01,},
{0x80,0x80,0x81,0x81,},
{0x80,0x80,0x81,0xC1,},
{0x80,0x80,0x83,0xE3,},
{0x80,0x00,0x03,0xE3,},
{0x80,0x18,0x03,0x03,},
{0x80,0x00,0x00,0x03,},
{0xFF,0xFF,0xFF,0xFF,},
};


uint8_t playerX = 1;
uint8_t playerY = 1;

// volatile direction cd = EAST;

uint8_t getTileAt (uint8_t x, uint8_t y)
{
  return (field[y][x / 8] & (1 << (7-(x % 8))) ? 1 : 0);
}
/*
void setupTimer()
{
  uint16_t c = ((F_CPU / 1024) / GAMECLOCK ) - 1;

  TCCR1B = (1 << WGM12);
  OCR1AH = (c >> 8);
  OCR1AL = c;
  TIMSK1 = (1 << OCIE1A);
  TCCR1B |= (1 << CS12) | (1 << CS10);
}
*/
/*
ISR(TIMER1_COMPA_vect)
{
  switch (cd)
  {
  case NORTH:
    if (canWalk(playerX, playerY+1))
    {
       drawPlayer(BACKGROUND);
      playerY += 1;
      drawPlayer(PLAYER_COLOR);
    }
    break;

  case EAST:
    if (canWalk(playerX+1, playerY))
      {
      drawPlayer(BACKGROUND);
      playerX += 1;
      drawPlayer(PLAYER_COLOR);
      }
    break;

  case SOUTH:
    if (canWalk(playerX, playerY-1))
      {
      drawPlayer(BACKGROUND);
      playerY -= 1;
      drawPlayer(PLAYER_COLOR);
      }
    break;

  case WEST:
    if(canWalk(playerX-1, playerY))
      {
      drawPlayer(BACKGROUND);
      playerX -= 1;
      drawPlayer(PLAYER_COLOR);
      }
    break;
  }
}
*/
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


// void drawPlayer(uint16_t color)
// {
//   tft.fillRect(playerX*BLOCK_SIZE, playerY*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, PLAYER_COLOR);
// }

void drawCoins(){
  for(int i=0;i<COINS_LENGTH;i++){
    tft.fillRoundRect(coins[i][0]*BLOCK_SIZE, coins[i][1]*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE,100, COIN_COLOR);
  }
}
// void moveGhost(){
//   for(int i=0;i<GHOSTS_LENGTH;i++){
//     tft.fillRoundRect(coins[i][0]*BLOCK_SIZE, coins[i][1]*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE,5, BACKGROUND);
//   }
// }

void drawGhosts(){
  for(int i=0;i<GHOSTS_LENGTH;i++){
    tft.fillRoundRect(coins[i][0]*BLOCK_SIZE, coins[i][1]*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE,5, GHOST_COLOR);
  }
}
void setSurrounding(uint8_t x, uint8_t y)
{
  surrounding[0] = getTileAt(x, y+1); //north
  surrounding[1] = getTileAt(x+1, y); //east
  surrounding[2] = getTileAt(x, y-1); //south
  surrounding[3] = getTileAt(x-1, y); //west
}

bool isNotWall(uint8_t x, uint8_t y)
{//x ,y grid coord
  if(getTileAt(x, y) != 0)
    return false;
  return true;
}

float revRound(float x){
  int c = ceil(x);
  int f = floor(x);

  float dec = x-((int)(x));//only decimals //define sepperate 
  float newX = x;

  if(dec>0.5){
     newX = floor(x);
  }else{
    newX = ceil(x);
  }
  
  // Serial.print("RR:");
  // Serial.print(dec);
  // Serial.print(":");
  // Serial.print(x);
  // Serial.print("*");
  // Serial.print(newX);
  

  return newX;
}

uint16_t* walkTo(uint16_t xFrom, uint16_t yFrom,uint16_t xTo,uint16_t yTo){
//xy free coord

//work in progress
  uint16_t *coords= new uint16_t[2];
  float xg = (float)xTo/(float)BLOCK_SIZE;
  float yg = (float)yTo/(float)BLOCK_SIZE;


  
  int xg_m = revRound(xg);//main x
  int yg_m = revRound(yg);//main y

  int xg_d = xg_m;//main down
  int yg_d = yg_m+1;//main down

  int xg_r = xg_m+1;//main right
  int yg_r = yg_m;//main right


  int xg_rd = xg_r;//main right down
  int yg_rd = yg_d;//main right down

  // tft.width();
  // Serial.print(xTo);
  // Serial.print(":");
  // Serial.print(yTo);

  // Serial.print(" ");
  // Serial.print(xg);
  // Serial.print(":");
  // Serial.print(yg);
  

  // Serial.print("=xg_m/");
  // Serial.print(xg_m);
  // Serial.print(":");
  // Serial.print(yg_m);

  // Serial.print("=xg_d/");
  // Serial.print(xg_d);
  // Serial.print(":");
  // Serial.print(yg_d);

  // Serial.print("=xg_r/");
  // Serial.print(xg_r);
  // Serial.print(":");
  // Serial.print(yg_r);

  // Serial.print("=xg_rd/");
  // Serial.print(xg_rd);
  // Serial.print(":");
  // Serial.print(yg_rd);


  bool xgmb = getTileAt(xg_m,yg_m);
  // Serial.print(xgmb);

  getTileAt(xg_d,yg_d);
  getTileAt(xg_r,yg_r);
  getTileAt(xg_rd,yg_rd);
  
  // getTileAt()
  // uint16_t coords[2]= new uint16_t {100,230};
  
  // Serial.print("=snap/");
  // Serial.print(xg_m*BLOCK_SIZE);
  // Serial.print(":");
  // Serial.print(yg_m*BLOCK_SIZE);

  // xTo = xg_m*BLOCK_SIZE;//snap x
  // yTo = yg_m*BLOCK_SIZE;//snap y
  // if()
  coords[0] = xFrom;
  coords[1] = yFrom;



  if(!xgmb){
    coords[0] = xg_m*BLOCK_SIZE;
    coords[1] = yg_m*BLOCK_SIZE;
  }

  

  // coords[0] = xTo;
  // coords[1] = yTo;
  
  Serial.println(" ");
  return coords;
}



void sendNEC(uint8_t data) {
 
      if(data == 1 ) // && !logicalzero && !logicalone
      {
        // Verzend een logische '1'
        // begin counter van 0 en tot die tijd zet infrared aan 
        counter69 = 0;
        logicalone = true;
        TCCR0A |= (1<< COM0A0); // toggle pin met 38Khz
      }
      else if (data == 0 ) 
      {
        // Verzend een logische '0'
        // begin counter van 0 en tot die tijd zet infrared aan
        counter69 = 0;
        logicalzero = true;
        TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
      }
      while(logicalone || logicalzero || logicalend || logicalbegin)
      {     
      }

}

void sendEnd()
{
    counter69 = 0;
    logicalend = true;
    TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
    while(logicalone || logicalzero || logicalend || logicalbegin)
    {      
    }
}

void sendBegin()
{
    counter69 = 0;
    logicalbegin = true;
    TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
    while(logicalone || logicalzero || logicalend || logicalbegin)
    {      
    }
}

ISR(INT0_vect)
{
  uint8_t pinstatus = (PIND & (1<<PD2));
  
 if(pinstatus)
 {

    // opgaande flank bepaal je het verschil tussen huidige       	//counterstand en de onthouden counterstand
    // Je hebt dan gemeten hoe lang de puls duurde. Daarmee kan je
    // bepalen of het uitgezonden bit een 0 of een 1 was. Schuif dit
    // bit in het resulterende buffer.
     currentCounterValue = counter;
     pulseDuration = abs(currentCounterValue - prevCounterValue);

     if(bufferIndex == 4)
     {
      bufferIndex = 0;
     }
    if (pulseDuration > 290 && pulseDuration < 320)
    {
      end = 1;
      bufferdata = buffer ;
      
    }
    if (pulseDuration > 260 && pulseDuration < 280) // 552 // 565
    {
      end = 0;
      buffer = 0;
      bufferIndex = 0;
      
    }
    if(pulseDuration > 160 && pulseDuration < 190 ) {
      // Voeg '1' toe aan buffer
      eenofnull = 1; 
      buffer |= (1 << bufferIndex); // Zet het bit op 1 
      bufferIndex++;

    } else if (pulseDuration < 95 && pulseDuration > 65 ) {
      // Voeg '0' toe aan buffer
      eenofnull = 0;
      buffer &= ~(1 << bufferIndex); // Zet het bit op 0
      bufferIndex++;
    }

 } else {
    
    // bij een neergaande flank onthouden counter van timer 1
    prevCounterValue = counter;
    eenofnull = 2;
 }
}

ISR(TIMER0_COMPA_vect)
{
  // counter hier altijd ophogen 
  counter++;
  counter69++;
  if (logicalone == true)
  {
    currentcounterone = counter69;
    if(currentcounterone > 200)
    {
      logicalone = false;
    }
    if ((currentcounterone ) >= (175)) 
    { 
      TCCR0A &= ~(1 << COM0A0);
    }
  }

  if (logicalzero == true)
  {
    currentcounterzero = counter69;
    if(currentcounterzero > 100)
    {
      logicalzero = false;
    }
    if ((currentcounterzero ) >= (75)) 
    {
      TCCR0A &= ~(1 << COM0A0);
    }
  }
  //begin 555
  if(logicalbegin == true)
  {
    currentcounterbegin = counter69;
    if(currentcounterbegin > 750) // 750
    {
      logicalbegin = false;
    }
    if((currentcounterbegin) >= (268))  // 268
    {
      TCCR0A &= ~(1 << COM0A0);
    }
  }

  //end 310
  if(logicalend == true)
  {
    currentcounterend = counter69;
    if(currentcounterend > 5000) // 325
    {
      logicalend = false;
    }
    if((currentcounterend) >= (310)) //
    {
      TCCR0A &= ~(1 << COM0A0);
    }
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
    EIFR |= (1 << INTF0);

}


bool setupDisplay(){
    //returned if setup is correctly completed
    tft.begin();
    tft.setRotation(1);//origin top left 
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

void drawPacmen(uint16_t x,uint16_t y){
  uint8_t r = (BLOCK_SIZE/2)-1;
  uint8_t maxBite = BLOCK_SIZE*0.4;//height of pacmen bite
  uint8_t bite = maxBite;
  // tft.fillRect(x,y,BLOCK_SIZE,BLOCK_SIZE,PLAYER_COLOR);
  tft.fillCircle(x+(BLOCK_SIZE/2),y+(BLOCK_SIZE/2),r,PLAYER_COLOR);
  tft.fillTriangle(x+(BLOCK_SIZE/2),y+(BLOCK_SIZE/2),(x+BLOCK_SIZE)-1,(y+(BLOCK_SIZE/2))+(bite/2),(x+BLOCK_SIZE)-1,(y+(BLOCK_SIZE/2))-(bite/2),BACKGROUND);

}


void drawPlayer(uint16_t x, uint16_t y){

    // tft.fillRect(playerX*BLOCK_SIZE, playerY*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, PLAYER_COLOR);
    // tft.fillRect(x,y,BLOCK_SIZE,BLOCK_SIZE,PLAYER_COLOR);
    drawPacmen(x,y);


    playerPosX = x;
    playerPosY = y;
}

void drawPath(uint16_t x, uint16_t y){
    tft.fillRect(x,y,BLOCK_SIZE,BLOCK_SIZE,BACKGROUND);
}

void movePlayer(uint16_t newX,uint16_t newY){
    drawPath(playerPosX,playerPosY);
    drawPlayer(newX,newY);
}
uint16_t* xyToVector(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){
  uint16_t* vector = new uint16_t[2];
  
  uint16_t vx = x0 - x1;
  uint16_t vy = y0 - y1;

  uint16_t l =(uint16_t) round(sqrt((sq(vx)+sq(vy))));//calc length of vector


  if(x0==x1 && y0==y1){//1
    vector[0] = 0;
    vector[1] = 0;
  }else

  if(x0<x1 && y0==y1){//2
    vector[0] = 90;
    vector[1] = l;
  }else

  if(x0==x1 && y0<y1){//3
    vector[0] = 0;
    vector[1] = l;
  }else

  if(x0>x1 && y0==y1){//4
    vector[0] = 270;
    vector[1] = l;
  }else
  
  if(x0==x1 && y0>y1){//5
    vector[0] = 180;
    vector[1] = l;
  }else
  
  if(x0<x1 && y0>y1){//6
    vector[0] = 135;
    vector[1] = l;
  }else

  if(x0<x1 && y0<y1){//7
    vector[0] = 45;
    vector[1] = l;
  }else

  if(x0>x1 && y0<y1){//8
    vector[0] = 315;
    vector[1] = l;
  }else

  if(x0>x1 && y0>y1){//9
    vector[0] = 225;
    vector[1] = l;
  }else{
    Serial.println("unkown");
  }
  
  return vector;
}

uint16_t* vectorToXY(uint16_t xb,uint16_t yb,uint16_t* vector){
  uint16_t* xy = new uint16_t[2];
  
  uint16_t l = vector[1];
  uint16_t s = round(sqrt((sq(l)/2)));//calc length to position

  if(vector[0] == 0 && vector[1] == 0){
    xy[0] = 0;
    xy[1] = 0;
  }else
  if(vector[0] == 0){
    xy[0] = xb;
    xy[1] = yb+l;
  }else
  if(vector[0] == 45){
    xy[0] = xb+s;
    xy[1] = yb+s;
  }else
  if(vector[0] == 90){
    xy[0] = xb+l;
    xy[1] = yb; 
  }else
  if(vector[0] == 135){
    xy[0] = xb+s;
    xy[1] = yb-s;
  }else
  if(vector[0] == 180){
    xy[0] = xb;
    xy[1] = yb-l;
  }else
  if(vector[0] == 225){
    xy[0] = xb-s;
    xy[1] = yb-s;
  }else
  if(vector[0] == 270){
    xy[0] = xb-l;
    xy[1] = yb;
  }else
  if(vector[0] == 315){
    xy[0] = xb-s;
    xy[1] = yb+s;
  }



  return xy ;
}

void movePlayerNunchuk(uint8_t playerIndex){
    
    uint16_t newX = players[playerIndex][_x_];
    uint16_t newY = players[playerIndex][_y_];
    
    if (nunchuckData & (1 << 3)) // 1000 <--
      {
        newX += playerSpeed;
        // newY
      }
      else if (nunchuckData & (1 << 2)) // 0100 -->
      {
       newX-=playerSpeed;
      }
      else if (nunchuckData & (1 << 1)) // 0010 v
      {
        newY+=playerSpeed;
      }
      else if (nunchuckData == 1 ) // 0001 ^
      {
        newY-=playerSpeed;
      }


    
    uint16_t* coordPtr = walkTo(players[playerIndex][_x_],players[playerIndex][_y_],newX,newY);
    // playerVector = xyToVector(playerPosX,playerPosY,coordPtr[0],coordPtr[1]);
    movePlayer(playerIndex,coordPtr[0],coordPtr[1]);
    delete coordPtr;
    
    

    
    

}

void drawLevel(){
  tft.fillScreen(BACKGROUND);
  drawField(&field[0]);
  drawCoins();
  drawGhosts();
  drawPlayer(players[0][0]*BLOCK_SIZE,players[0][1]*BLOCK_SIZE);
}


void getNunchukPosition(){

    if(!Nunchuk.getState(nunchuk_ID)){
        return;
    } 
    
    // uint8_t x = Nunchuk.state.joy_x_axis;
    // uint8_t y = Nunchuk.state.joy_y_axis;
    //flipped
    uint8_t x = Nunchuk.state.joy_x_axis;
    uint8_t y = -Nunchuk.state.joy_y_axis;
    
    

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

#define MASK 0b0000
uint8_t nunchuckWrap(){

  nunchuckData = 0b0000;

  if(NunChuckPosition[1] > 128){
      nunchuckData = (1<<1) | MASK;
  } else if( NunChuckPosition[1] < 128 ){
      nunchuckData = (1<<0) | MASK;
  } 
  if(NunChuckPosition[0] > 128){
      nunchuckData = (1<<3) | MASK;
  } else if( NunChuckPosition[0] < 128 ){
      nunchuckData = (1<<2) | MASK;
  } 
  return nunchuckData;
}

void sendByte(uint8_t byte, bool address)
{
  if(!address){
    for (uint8_t bit = 0; bit < 8; bit++)
    {
      sendNEC((byte >> bit) & 1);
      // Serial.println(pulseDuration);
    }
  }
  if(address)
  {
    for (uint8_t bit = 0; bit < 4; bit++)
    {
      sendNEC((byte >> bit) & 1);
      // Serial.println(pulseDuration);
    } 
  }
  
}


void sendCommand(uint8_t address, uint8_t command)
{
  sendBegin();
  sendByte(address,true);
  // sendByte(command,false);
  sendEnd();
}


void moveOverIR(uint8_t playerIndex)
{
  
    uint16_t newX = players[playerIndex][_x_];
    uint16_t newY = players[playerIndex][_y_];


      if (bufferdata & (1 << 3)) // 1000 <--
      {
        newX += playerSpeed;
        // newY
      }
      else if (bufferdata & (1 << 2)) // 0100 -->
      {
       newX-=playerSpeed;
      }
      else if (bufferdata & (1 << 1)) // 0010 v
      {
        newY+=playerSpeed;
      }
      else if (bufferdata == 1 ) // 0001 ^
      {
        newY-=playerSpeed;
      }

    uint16_t* coordPtr = walkTo(players[playerIndex][_x_],players[playerIndex][_y_],newX,newY);
    movePlayer(playerIndex,coordPtr[0],coordPtr[1]);
    delete coordPtr;
    
}

void printHighScore(char names[10][6],uint32_t scores[10]);
void sort(char names[10][6],uint32_t scores[10], int pos[10]);
void addScore(char name[6], uint32_t points);
//function to call when going to highscore page
void HighScorePage(){
  setupHighScore();
  getScore();
  printHighScore(nameList,scoreList);
}
//clears the screen, sets the font and text color 
bool setupHighScore(){
  tft.fillScreen(BACKGROUND);
  tft.setFont(&PressStart2P_vaV76pt7b);
  tft.setTextColor(HIGHSCORECOLOR);
  return 1;
}
//gets the EEPROM data, should only be called once every time the arduino starts up
void getScore(){
  char temp[6];
  uint32_t temp2;
  for(int i = 0;i<10;i++){
    EEPROM.get(i*10,temp);
    EEPROM.get(i*10+6,temp2);
    for(int j = 0;j<6;j++){
      nameList[i][j] = temp[j];
    }
    scoreList[i] = temp2;
  }
  sort(nameList,scoreList,posList);
}
//prints the highscores, uses the scoreList and nameList
void printHighScore(char names[10][6],uint32_t scores[10]){
  tft.fillScreen(BACKGROUND);
  sort(nameList,scoreList,posList);
  tft.setCursor(110,25);
  tft.println("Highscore");
  char bufHS[64];
  char tempN[6];
  long tempS;
  for(int i = 9;i>=0;i--){
    for(int j = 0;j<6;j++){
      tempN[j] = names[i][j];
    }
    tempS = scoreList[i];
    sprintf(bufHS,"%2i %-6s %10lu",10-i,tempN,tempS);
    tft.setCursor(50,200-i*15);
    tft.println(bufHS);
  }
}
//sorts the highscores so that they get called in the correct order, uses insertion sort
void sort(char names[10][6],uint32_t scores[10],int pos[10]){
    char temp1[6] = {};
    uint32_t temp2;
    int temp3;
    int j;
    for(int p = 1;p< 10;p++){
        for(int i = 0;i<6;i++){
            temp1[i] = names[p][i];
        }
        temp2 = scores[p];
        temp3 = posList[p];
    for(j = p;j> 0 && temp2<scores[j-1];j--){
        scores[j] = scores[j-1];
        posList[j] = posList[j-1];
    
        for(int i = 0;i<6;i++){
            names[j][i] = names[j-1][i];
        }
    }
    scores[j] = temp2;
    posList[j] = temp3;
    for(int i = 0;i<6;i++){
            names[j][i] = temp1[i];
        }
    }

  }
//fills EEPROM with mock scores
void setupScore(){
  char names[10][6] = {"henk","kees","klaas","piet","jan","wilem","jurre","andor","rick","jay"};
  uint32_t scores[10] ={1000000,1000,8000,4000,3000,6000,2000,7000,9000,5000};
  for(int i = 0;i<10;i++){
    for(int j = 0;j<6;j++){
    EEPROM.put(i*10+j,names[i][j]);
    }
    EEPROM.put(i*10+6,scores[i]);
  }
}
//adds a new score to the EEPROM, also add it to scoreList and nameList so you dont have to read all the EEPROM data again
void addScore(char name[6], uint32_t points){
  if(points > scoreList[0]){
    scoreList[0] = points;
    for(int j = 0;j<6;j++){
    nameList[0][j] = name[j];
    EEPROM.put(posList[0]*10+j,name[j]);
    }
    EEPROM.put(posList[0]*10+6,points);
  }  
}
int main(void)
{
    pacmanTheme.frequencies = notes::pacmanNotes;
    pacmanTheme.durations = notes::pacmanDurations;
    pacmanTheme.length = sizeof(notes::pacmanNotes) / sizeof(uint16_t);
    setupBuzzer();
    loadMusic(&pacmanTheme);
    setVolume(100);
    enableLoop();
    playMusic();

    sei();
    initIR();
    Serial.begin(BAUDRATE);
    Wire.begin();

    if(!setupDisplay()){return 0;}
    
    uint8_t f = true;
    while(!setupNunchuck())
    {
      if (f)
      {
        tft.fillScreen(ILI9341_BLACK);
        tft.print("Please connect Nunchuk");
        f = false;
      }
    }

    drawLevel();

    // Serial.print()
    while(1)
    {
        getNunchukPosition();
        sendCommand(nunchuckWrap(), nunchuckWrap());
        moveOverIR(1);
        movePlayerNunchuk(playablePlayer);  
    } 
    return 0;
}

