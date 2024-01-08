#include <avr/interrupt.h>
#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <EEPROM.h>
#include "Adafruit_STMPE610.h"
#include <util/delay.h>
#include <sound.h>
#include <notes.h>
#include <screens.h>

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
//game
const int GAMECLOCK = 15; //ball updates every x times per second
// uint16_t playerPosX;
// uint16_t playerPosY;

const uint8_t numPlayers = 2;
uint8_t playablePlayer = 0;
// uint16_t* playerVector = NULL;
uint8_t nunchuckData = 0b0000;
// uint16_t playerResult[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};

const uint16_t FIELD_WIDTH = 16;
const uint16_t FIELD_HEIGHT = 16; // could be uint8_t
const uint8_t BLOCK_SIZE = 15;
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
void drawPlayer(uint8_t playerIndex,uint16_t x, uint16_t y);
void drawPath(uint16_t x, uint16_t y);
void movePlayer(uint8_t playerIndex, uint16_t newX,uint16_t newY);
void movePlayerNunchuk(uint8_t playerIndex);
void drawLevel();
void getNunchukPosition();
void PCF8574_write(byte bytebuffer);
void addToBuffer();
uint8_t nunchuckWrap();
void moveOverIR();
void setupTimer();
// void drawField(uint8_t [FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION]);
void walkPlayer(uint8_t, uint8_t);
void drawPlayer(uint16_t color);
void drawCoins();
void drawGhosts();
bool canWalk(uint8_t, uint8_t);

//void setSurrounding(uint8_t, uint8_t);

void HighScorePage();
bool setupHighScore();
void getScore();
void setupScore();
void printHighScore(char names[10][6],uint32_t scores[10]);
void sort(char names[10][6],uint32_t scores[10], int pos[10]);
void addScore(char name[6], uint32_t points);


//game level
const uint8_t COINS_LENGTH  = 7;
const uint8_t GHOSTS_LENGTH = 0;
const uint8_t PLAYER_LENGTH = 6;
uint16_t coins[7][2] = {
{1*BLOCK_SIZE,1*BLOCK_SIZE},
{1*BLOCK_SIZE,4*BLOCK_SIZE},
{5*BLOCK_SIZE,1*BLOCK_SIZE},
{7*BLOCK_SIZE,1*BLOCK_SIZE},
{6*BLOCK_SIZE,1*BLOCK_SIZE},
{11*BLOCK_SIZE,1*BLOCK_SIZE},
{1*BLOCK_SIZE,10*BLOCK_SIZE},

};
bool coinsCatched[7] = {false,false,false,false,false,false,false,};
uint16_t players[6][2] = {
{1*BLOCK_SIZE,13*BLOCK_SIZE},
{1*BLOCK_SIZE,12*BLOCK_SIZE},
{1*BLOCK_SIZE,14*BLOCK_SIZE},
{2*BLOCK_SIZE,12*BLOCK_SIZE},
{2*BLOCK_SIZE,13*BLOCK_SIZE},
{2*BLOCK_SIZE,14*BLOCK_SIZE},
};
uint16_t playerResult[6] = {0,0,0,0,0,0,};
uint16_t ghosts[0][2] = {
};
bool ghostsCatched[0] = {};
uint8_t field[FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION] = {
{0xFF,0xFF,},
{0x80,0x01,},
{0xA1,0x01,},
{0xA1,0x01,},
{0xA1,0x09,},
{0xA1,0x1D,},
{0xBF,0x2B,},
{0x80,0x09,},
{0x80,0x09,},
{0x80,0x09,},
{0x80,0x09,},
{0x80,0x09,},
{0x80,0x3F,},
{0x80,0x3F,},
{0x80,0x3F,},
{0xFF,0xFF,},
};

const uint8_t COINS_LENGTH2  = 64;
const uint8_t GHOSTS_LENGTH2 = 1;
const uint8_t PLAYER_LENGTH2 = 2;
uint16_t coins2[64][2] = {
{1*BLOCK_SIZE,1*BLOCK_SIZE},
{3*BLOCK_SIZE,1*BLOCK_SIZE},
{5*BLOCK_SIZE,1*BLOCK_SIZE},
{7*BLOCK_SIZE,1*BLOCK_SIZE},
{7*BLOCK_SIZE,3*BLOCK_SIZE},
{9*BLOCK_SIZE,3*BLOCK_SIZE},
{9*BLOCK_SIZE,1*BLOCK_SIZE},
{11*BLOCK_SIZE,1*BLOCK_SIZE},
{13*BLOCK_SIZE,1*BLOCK_SIZE},
{14*BLOCK_SIZE,2*BLOCK_SIZE},
{14*BLOCK_SIZE,4*BLOCK_SIZE},
{14*BLOCK_SIZE,6*BLOCK_SIZE},
{14*BLOCK_SIZE,8*BLOCK_SIZE},
{12*BLOCK_SIZE,8*BLOCK_SIZE},
{12*BLOCK_SIZE,6*BLOCK_SIZE},
{12*BLOCK_SIZE,4*BLOCK_SIZE},
{12*BLOCK_SIZE,2*BLOCK_SIZE},
{11*BLOCK_SIZE,3*BLOCK_SIZE},
{5*BLOCK_SIZE,3*BLOCK_SIZE},
{3*BLOCK_SIZE,3*BLOCK_SIZE},
{4*BLOCK_SIZE,2*BLOCK_SIZE},
{1*BLOCK_SIZE,3*BLOCK_SIZE},
{3*BLOCK_SIZE,5*BLOCK_SIZE},
{5*BLOCK_SIZE,5*BLOCK_SIZE},
{6*BLOCK_SIZE,4*BLOCK_SIZE},
{7*BLOCK_SIZE,5*BLOCK_SIZE},
{9*BLOCK_SIZE,5*BLOCK_SIZE},
{10*BLOCK_SIZE,4*BLOCK_SIZE},
{3*BLOCK_SIZE,7*BLOCK_SIZE},
{1*BLOCK_SIZE,7*BLOCK_SIZE},
{2*BLOCK_SIZE,8*BLOCK_SIZE},
{3*BLOCK_SIZE,9*BLOCK_SIZE},
{4*BLOCK_SIZE,10*BLOCK_SIZE},
{6*BLOCK_SIZE,10*BLOCK_SIZE},
{8*BLOCK_SIZE,9*BLOCK_SIZE},
{8*BLOCK_SIZE,7*BLOCK_SIZE},
{8*BLOCK_SIZE,6*BLOCK_SIZE},
{10*BLOCK_SIZE,7*BLOCK_SIZE},
{10*BLOCK_SIZE,9*BLOCK_SIZE},
{12*BLOCK_SIZE,9*BLOCK_SIZE},
{11*BLOCK_SIZE,10*BLOCK_SIZE},
{9*BLOCK_SIZE,10*BLOCK_SIZE},
{3*BLOCK_SIZE,11*BLOCK_SIZE},
{2*BLOCK_SIZE,12*BLOCK_SIZE},
{1*BLOCK_SIZE,11*BLOCK_SIZE},
{1*BLOCK_SIZE,13*BLOCK_SIZE},
{1*BLOCK_SIZE,10*BLOCK_SIZE},
{2*BLOCK_SIZE,14*BLOCK_SIZE},
{4*BLOCK_SIZE,14*BLOCK_SIZE},
{5*BLOCK_SIZE,13*BLOCK_SIZE},
{4*BLOCK_SIZE,12*BLOCK_SIZE},
{8*BLOCK_SIZE,13*BLOCK_SIZE},
{9*BLOCK_SIZE,12*BLOCK_SIZE},
{10*BLOCK_SIZE,11*BLOCK_SIZE},
{9*BLOCK_SIZE,14*BLOCK_SIZE},
{11*BLOCK_SIZE,14*BLOCK_SIZE},
{13*BLOCK_SIZE,14*BLOCK_SIZE},
{14*BLOCK_SIZE,13*BLOCK_SIZE},
{13*BLOCK_SIZE,12*BLOCK_SIZE},
{14*BLOCK_SIZE,11*BLOCK_SIZE},
{14*BLOCK_SIZE,10*BLOCK_SIZE},
{12*BLOCK_SIZE,11*BLOCK_SIZE},
{13*BLOCK_SIZE,5*BLOCK_SIZE},
{6*BLOCK_SIZE,12*BLOCK_SIZE},
};
bool coinsCatched2[64] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,};
uint16_t players2[2][2] = {
{6*BLOCK_SIZE,14*BLOCK_SIZE},
{7*BLOCK_SIZE,14*BLOCK_SIZE},
};
uint16_t playerResult2[2] = {0,0,};
uint16_t ghosts2[1][2] = {
{6*BLOCK_SIZE,7*BLOCK_SIZE},
};
bool ghostsCatched2[1] = {false,};
uint8_t field2[FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION] = {
{0xFF,0xFF,},
{0x80,0x81,},
{0xB6,0xB5,},
{0x80,0x05,},
{0xEC,0xD5,},
{0xA0,0x11,},
{0xE7,0x75,},
{0x8D,0x15,},
{0x8F,0x51,},
{0xE6,0x47,},
{0xA0,0x05,},
{0x87,0xD5,},
{0x91,0x11,},
{0xB9,0x7D,},
{0x80,0x01,},
{0xFF,0xFF,},
};


uint8_t getTileAt (uint8_t x, uint8_t y)
{
  return (field[y][x / 8] & (1 << (7-(x % 8))) ? 1 : 0);
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


// void drawPlayer(uint16_t color)
// {
//   tft.fillRect(playerX*BLOCK_SIZE, playerY*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, PLAYER_COLOR);
// }

uint8_t encodeGridPosition(uint16_t* coords){//max 16x16 grid with 8bits

  uint8_t coordX =  coords[_x_] / BLOCK_SIZE;
  uint8_t coordy =  coords[_y_] / BLOCK_SIZE;

  

  //4bit x pos , 4bit y pos
  uint8_t gridpos = (coordX << 4)|(coordy & 0xF);

  return gridpos;
}

uint16_t* decodeGridPosition(uint8_t gridpos){//max 16x16 grid with 8bits
  uint16_t *coords= new uint16_t[2];
  
  coords[0] = ((gridpos>>4))*BLOCK_SIZE;
  coords[1] = (gridpos & 0xF)*BLOCK_SIZE;

  return coords;
}

void collectCoin(uint16_t playerIndex,uint16_t coinIndex){
  // Serial.print("collect coin");
  playerResult[playerIndex]+=1;
  coinsCatched[coinIndex] = true;
  // Serial.print(playerIndex);
  // Serial.print(":");
  // Serial.print(playerResult[playerIndex]);
  // Serial.println(" ");

}

void collectGhost(uint16_t playerIndex,uint16_t ghostIndex){
  Serial.println("catched a ghost");
}

void drawCoins(){
  for(int i=0;i<COINS_LENGTH;i++){
    tft.fillRoundRect(coins[i][0], coins[i][1], BLOCK_SIZE, BLOCK_SIZE,100, COIN_COLOR);
  }
}

void collision(){
  for(int p = 0;(p<numPlayers&&p<PLAYER_LENGTH);p++){
    uint16_t x = players[p][_x_];
    uint16_t y = players[p][_y_];

    
    for(int c =0;c<COINS_LENGTH;c++){
      if(coins[c][_x_] == x && coins[c][_y_] == y){
        if(!coinsCatched[c]){
          collectCoin(p,c);
        }
      }
    }

    for(int g=0;g<GHOSTS_LENGTH;g++){
      if(ghosts[g][_x_] == x && ghosts[g][_y_] == y){
        if(!ghostsCatched[g]){
          collectGhost(p,g);
        }
      }
    }


  }

  // 
}

bool endGame(){
  bool noCoinsExist = true;
  for(int c =0;c<COINS_LENGTH;c++){
      if(!coinsCatched[c]){
        noCoinsExist = false;
      }
    }

  return noCoinsExist;  
}

// void moveGhost(){
//   for(int i=0;i<GHOSTS_LENGTH;i++){
//     tft.fillRoundRect(coins[i][0]*BLOCK_SIZE, coins[i][1]*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE,5, BACKGROUND);
//   }
// }



void drawGhosts(){
  for(int i=0;i<GHOSTS_LENGTH;i++){
    tft.fillRoundRect(coins[i][0], coins[i][1], BLOCK_SIZE, BLOCK_SIZE,5, GHOST_COLOR);
  }
}
// void setSurrounding(uint8_t x, uint8_t y)
// {
//   surrounding[0] = getTileAt(x, y+1); //north
//   surrounding[1] = getTileAt(x+1, y); //east
//   surrounding[2] = getTileAt(x, y-1); //south
//   surrounding[3] = getTileAt(x-1, y); //west
// }

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
  
  // Serial.println(" ");
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

    if(pulseDuration > 160 && pulseDuration < 190) {

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
        #endif
        return(false);
    }
    #ifdef SerialActive
    #endif
    return true;
}

void drawPacmen(uint16_t x,uint16_t y){
  uint16_t color  = PLAYER_COLOR;
  uint8_t r = (BLOCK_SIZE/2)-1;//radius of pacman
  uint8_t maxBite = BLOCK_SIZE*0.4;//height of pacmen bite
  uint8_t bite = maxBite;
  // tft.fillRect(x,y,BLOCK_SIZE,BLOCK_SIZE,PLAYER_COLOR);
  tft.fillCircle(x+(BLOCK_SIZE/2),y+(BLOCK_SIZE/2),r,color);
  tft.fillTriangle(x+(BLOCK_SIZE/2),y+(BLOCK_SIZE/2),(x+BLOCK_SIZE)-1,(y+(BLOCK_SIZE/2))+(bite/2),(x+BLOCK_SIZE)-1,(y+(BLOCK_SIZE/2))-(bite/2),BACKGROUND);

}


void drawPlayer(uint8_t playerIndex , uint16_t x, uint16_t y){
    drawPacmen(x,y);

    players[playerIndex][_x_] = x;
    players[playerIndex][_y_] = y;
}

void drawPath(uint16_t x, uint16_t y){
    tft.fillRect(x,y,BLOCK_SIZE,BLOCK_SIZE,BACKGROUND);
}

void movePlayer(uint8_t playerIndex,uint16_t newX,uint16_t newY){
    drawPath(players[playerIndex][_x_],players[playerIndex][_y_]);
    drawPlayer(playerIndex,newX,newY);
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

    uint8_t gridpos=  encodeGridPosition(coordPtr); // arduino 0 verstuurd gridpos
    uint16_t* gridPtr = decodeGridPosition(gridpos);// arduino 1 ontvangt gridpos

    movePlayer(playerIndex,coordPtr[0],coordPtr[1]);
    delete gridPtr;
    delete coordPtr;
    
    

    
    

}

void drawLevel(){
  tft.fillScreen(BACKGROUND);
  drawField(&field[0]);
  drawCoins();
  drawGhosts();
  for(int i =0;i<numPlayers;i++){
    drawPlayer(i,players[i][_x_],players[i][_y_]);
  }
}


void getNunchukPosition(){

    if(!Nunchuk.getState(nunchuk_ID)){
        return;
    } 
    
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
  if(NunChuckPosition[0] > 128){
      nunchuckData = (1<<3) | nunchuckData;
  } else if( NunChuckPosition[0] < 128 ){
      nunchuckData = (1<<2) | nunchuckData;
  } 
  
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

int main(void)
{
  extern screens cScreen; // current screen // extern variable in header file gives linker error, this works, not ideal...
  extern screens nScreen; // new screen

  actions action = NO_ACTION;
  uint16_t touchX, touchY;

  if(!setupDisplay()) return 1;

  Wire.begin();
  sei();
  if(!Nunchuk.begin(nunchuk_ID))
  {
      tft.fillScreen(ILI9341_BLACK);
      tft.print("Please connect Nunchuk");
    while(!Nunchuk.begin(nunchuk_ID)){};
  }

  while (1)
  {
    // get events for screen
    getNunchukPosition(); // Should be setNunchukPosition
    if (NunChuckPosition[3]) // if Z is pressed. click button
    {
      action = SELECTBUTTON;
    } else if (NunChuckPosition[2]) // if c is pressed. move to next button
    {
      action = NEXTBUTTON;
      while(NunChuckPosition[2]){getNunchukPosition();} // wait for c release, otherwise too fast
    }

    switch (nScreen)
    {
      case LOADING_SCREEN:
        if (cScreen != nScreen)
        {
          initLoadingScreen(tft);
          cScreen = LOADING_SCREEN;
        }
        else
        {
          handleLoadingScreen(tft, action);
        }
        break;
        
      case MENU_SCREEN:
        if (cScreen != nScreen)
        {
          initMenuScreen(tft);
          cScreen = MENU_SCREEN;
        } else
        {
          handleMenuScreen(tft, action);
        }
        break;

      case LEVEL_SCREEN:
        if (cScreen != nScreen)
        {
          initLevelScreen(tft);
          cScreen = LEVEL_SCREEN;
        }
        else
        {
          handleLevelScreen(tft, action);
        }
        break;
    }
    action = NO_ACTION;
  }
}

