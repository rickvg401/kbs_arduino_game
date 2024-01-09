#include <avr/interrupt.h>
#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <EEPROM.h>
#include <Adafruit_FT6206.h>
#include <util/delay.h>
#include <sound.h>
#include <notes.h>
#include <screens.h>
// #include "display/fonts/PressStart2P_vaV74pt7b.h"
#include "display/fonts/PressStart2P_vaV74pt7b.h"


//general
#define F_CPU 16000000ul
#define _x_ 0
#define _y_ 1

//Level select
enum ControlStates {_GAME,_MENU,_PLAYERMENU};
ControlStates controlState = _PLAYERMENU;

//serial
#define SerialActive //if defined serial is active
#define BAUDRATE 9600

// Arduino IR address
uint8_t address1 = 0b1010;
uint8_t address2 = 0b1101;

//nunchuk
#define nunchuk_ID 0xA4 >> 1
#define MASK 0b0000

// unsigned char buffer[4];// array to store arduino output
uint8_t NunChuckPosition[4];
bool NunChuckPositionDivided = false;

// Touch screen
Adafruit_FT6206 ctp = Adafruit_FT6206();


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
// uint8_t playablePlayer = 0;
// uint8_t playableGhost = 0;

uint8_t IrId = 1;
uint8_t playerId = 0; 

bool nunchukIsGhost = false;
bool IrIsGhost = false;

bool noCoins = true;
bool noGhost = true;
// uint16_t* playerVector = NULL;
uint8_t nunchuckData = 0b0000;
// uint16_t playerResult[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};

//Scoreboard
uint8_t lives = 3;
uint16_t score = 0;



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

// map character selection variables
bool GhostOfPacman = false; // bool for pacman or ghost
uint8_t lastButtonState = 0;    // previous state of the button for detecting button push once, and so it will not spam clicks

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

//synchroon bool
volatile bool synchroon = 1;
volatile uint16_t currentcountersynchroon = 0;
volatile bool synchroniseer = 0;

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
void syncPlayerLocation(uint16_t* coordPtr, uint8_t playerIndex);
//void setSurrounding(uint8_t, uint8_t);

void HighScorePage();
bool setupHighScore();
void getScore();
void setupScore();
void printHighScore(char names[10][6],uint32_t scores[10]);
void sort(char names[10][6],uint32_t scores[10], int pos[10]);
void addScore(char name[6], uint32_t points);
void valuesVS();
void valuesGhost();

//game level
uint8_t levelSelect = 0;

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
uint16_t playerLives[6] = {0,0,0,0,0,0,};
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

const uint8_t COINS_LENGTH2  = 0;
const uint8_t GHOSTS_LENGTH2 = 1;
const uint8_t PLAYER_LENGTH2 = 2;
uint16_t coins2[0][2] = {};
bool coinsCatched2[0] ={}; 
uint16_t players2[2][2] = {
{6*BLOCK_SIZE,14*BLOCK_SIZE},
{7*BLOCK_SIZE,14*BLOCK_SIZE},
};
uint16_t playerResult2[2] = {0,0,};
uint16_t playerLives2[2] = {0,0,};
uint16_t ghosts2[1][2] = {
// {6*BLOCK_SIZE,7*BLOCK_SIZE},
{2*BLOCK_SIZE,3*BLOCK_SIZE},
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


uint16_t playerResetPosition[2] = {0,0};
uint16_t ghostResetPosition[2] = {0,0};


uint8_t getTileAt (uint8_t x, uint8_t y)
{
  switch (levelSelect)
  {
    case 0:
      return (field[y][x / 8] & (1 << (7-(x % 8))) ? 1 : 0);
      break;

    case 1:
      return (field2[y][x / 8] & (1 << (7-(x % 8))) ? 1 : 0);
      break;
  }
}

void setResetPosition(){
  switch (levelSelect)
  {
  case 0:
    playerResetPosition[0] = players[0][0];
    playerResetPosition[1] = players[0][1];

    ghostResetPosition[0] = ghosts[0][0];
    ghostResetPosition[1] = ghosts[0][1];


    break;
  
  case 1:
    playerResetPosition[0] = players2[0][0];
    playerResetPosition[1] = players2[0][1];

    ghostResetPosition[0] = ghosts2[0][0];
    ghostResetPosition[1] = ghosts2[0][1];
    break;
  }
  
}

void drawField()
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
  Serial.print("collect coin");
  switch (levelSelect)
  {
  case 0:
    // playerResult[playerIndex]+=10;
    coinsCatched[coinIndex] = true;
    // valuesVS();
    break;
  
  case 1:
    // playerResult2[playerIndex]+=10;
    coinsCatched2[coinIndex] = true;
    break;
  }
  
  
}




void collectGhost(uint16_t playerIndex,uint16_t ghostIndex){


  // Serial.println("catched a ghost");

  switch (levelSelect)
  {
  case 0:
    ghostsCatched[ghostIndex] = false;
    playerLives[playerIndex]--;

    ghosts[ghostIndex][0] = ghostResetPosition[0];
    ghosts[ghostIndex][1] = ghostResetPosition[1];

    players[playerIndex][0] = playerResetPosition[0];
    players[playerIndex][1] = playerResetPosition[1];
    
    break;
  
  case 1:
    ghostsCatched2[ghostIndex] = false;
    playerLives2[playerIndex]--;
    ghosts2[ghostIndex][0] = ghostResetPosition[0];
    ghosts2[ghostIndex][1] = ghostResetPosition[1];

    players2[playerIndex][0] = playerResetPosition[0];
    players2[playerIndex][1] = playerResetPosition[1];
    break;
  }

  valuesGhost();
}

void setLives(){
  switch (levelSelect)
  {
  case 0:
    for(int l = 0; l<PLAYER_LENGTH;l++){
      playerLives[l] = lives;
    }
    break;
  
  case 1:
    for(int l = 0; l<PLAYER_LENGTH2;l++){
      playerLives2[l] = lives;
    }
    break;
  }
  
}

void drawCoins(){
  switch (levelSelect)
  {
  case 0:
    for(int i=0;i<COINS_LENGTH;i++){
      tft.fillRoundRect(coins[i][0], coins[i][1], BLOCK_SIZE, BLOCK_SIZE,100, COIN_COLOR);
    }
    
    break;
  
  case 1:
      for(int i=0;i<COINS_LENGTH2;i++){
        tft.fillRoundRect(coins2[i][0], coins2[i][1], BLOCK_SIZE, BLOCK_SIZE,100, COIN_COLOR);
      }
      break;
  }

}

void collision(){
  switch (levelSelect)
  {
  case 0:
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
    break;
  
  case 1:
    for(int p = 0;(p<numPlayers&&p<PLAYER_LENGTH2);p++){
    uint16_t x = players2[p][_x_];
    uint16_t y = players2[p][_y_];

    
    for(int c =0;c<COINS_LENGTH2;c++){
      if(coins2[c][_x_] == x && coins2[c][_y_] == y){
        if(!coinsCatched2[c]){
          collectCoin(p,c);
        }
      }
    }

    for(int g=0;g<GHOSTS_LENGTH2;g++){
      if(ghosts2[g][_x_] == x && ghosts2[g][_y_] == y){
        if(!ghostsCatched2[g]){
          collectGhost(p,g);
        }
      }
    }


    }

    break;
  }

  
}

bool endGame(){
  bool coinsExist = false;
  bool ghostsExist = false;
  bool livesExist = true;

  switch (levelSelect)
  {
  case 0:
    // //if coins exist 
    // //set coinsExist to true
    if(!noCoins){
      for(int c =0;c<COINS_LENGTH;c++){
        if(!coinsCatched[c]){
          coinsExist = true;
        }
      }
    }else{
      coinsExist = true;
    }
    
    // //if no ghost exist 
    // //set ghostsExist to False
    if(!noGhost){
      for(int c =0;c<GHOSTS_LENGTH;c++){
        if(!ghostsCatched[c]){
          ghostsExist = true;
        }
      }
    }else{
      ghostsExist = true;
    }

    // //if no lives exist
    // //set livesExist to false 
    for(int p = 0;p<PLAYER_LENGTH;p++){
      if(playerLives[p]<=0){
        livesExist = false;
      }
    }
    break;
  
  case 1:
    // //if coins exist 
    // //set coinsExist to true
    if(!noCoins){
      for(int c =0;c<COINS_LENGTH2;c++){
        if(!coinsCatched2[c]){
          coinsExist = true;
        }
      }
    }else{
      coinsExist = true;
    }
    
    // //if no ghost exist 
    // //set ghostsExist to False
    if(!noGhost){
      for(int c =0;c<GHOSTS_LENGTH2;c++){
        if(!ghostsCatched2[c]){
          ghostsExist = true;
        }
      }
    }else{
      ghostsExist = true;
    }

    // //if no lives exist
    // //set livesExist to false 
    for(int p = 0;p<PLAYER_LENGTH2;p++){
      if(playerLives2[p]<=0){
        livesExist = false;
      }
    }
    break;
  }

  

  if(!coinsExist || !ghostsExist || !livesExist){
    return true;
  }else{
    return false;
  }
}


void drawGhost(uint8_t ghostIndex , uint16_t x, uint16_t y,uint16_t color){

// void drawGhost(uint16_t x,uint16_t y,uint16_t color){
  tft.fillRoundRect(x, y, BLOCK_SIZE, BLOCK_SIZE,5, color);

  switch (levelSelect)
    {
    case 0:
      ghosts[ghostIndex][_x_] = x;
      ghosts[ghostIndex][_y_] = y;
      break;
    
    case 1:
      ghosts2[ghostIndex][_x_] = x;
      ghosts2[ghostIndex][_y_] = y;
      break;
    }

}

void moveGhost(uint8_t ghostIndex,uint16_t newX,uint16_t newY,uint16_t color){
  switch (levelSelect)
    {
    case 0:
      drawPath(ghosts[ghostIndex][_x_],ghosts[ghostIndex][_y_]);
      break;
    
    case 1:
      drawPath(ghosts2[ghostIndex][_x_],ghosts2[ghostIndex][_y_]);
      break;
    }
  drawGhost(ghostIndex,newX,newY,color);
}

void drawGhosts(){
  switch (levelSelect)
  {
  case 0:
    for(int i=0;i<GHOSTS_LENGTH;i++){
      // tft.fillRoundRect(ghosts[i][0], ghosts[i][1], BLOCK_SIZE, BLOCK_SIZE,5, GHOST_COLOR);
      drawGhost(i,ghosts[i][0],ghosts[i][1],GHOST_COLOR);
    }
    break;
  
  case 1:
    for(int i=0;i<GHOSTS_LENGTH2;i++){
      // tft.fillRoundRect(ghosts2[i][0], ghosts2[i][1], BLOCK_SIZE, BLOCK_SIZE,5, GHOST_COLOR);
      drawGhost(i,ghosts2[i][0],ghosts2[i][1],GHOST_COLOR);

    }
    break;
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
  float dec = x-((int)(x));//only decimals //define sepperate 
  float newX = x;

  if(dec>0.5){
     newX = floor(x);
  }else{
    newX = ceil(x);
  }
  
  

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

  bool xgmb = getTileAt(xg_m,yg_m);

  coords[0] = xFrom;
  coords[1] = yFrom;



  if(!xgmb){
    coords[0] = xg_m*BLOCK_SIZE;
    coords[1] = yg_m*BLOCK_SIZE;
  }


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

     if(bufferIndex == 12)
     {
      bufferIndex = 0;
     }
    if (pulseDuration > 290 && pulseDuration < 320)
    {
      end = 1;
      // bufferdata = buffer >> 4;
      // buffer = buffer >> 4; // logical end
    }

    if (pulseDuration > 260 && pulseDuration < 280) 
    {
      end = 0;
      buffer = 0;
      bufferIndex = 0; // logical begin
      
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
  if(synchroon)
  {
    currentcountersynchroon = counter69;
    if(currentcountersynchroon > 5000)
    {
      synchroniseer = true;
      synchroon = false;
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

    switch (levelSelect)
    {
    case 0:
      players[playerIndex][_x_] = x;
      players[playerIndex][_y_] = y;
      break;
    
    case 1:
      players2[playerIndex][_x_] = x;
      players2[playerIndex][_y_] = y;
      break;
    }
}

void drawPath(uint16_t x, uint16_t y){
    tft.fillRect(x,y,BLOCK_SIZE,BLOCK_SIZE,BACKGROUND);
}

void movePlayer(uint8_t playerIndex,uint16_t newX,uint16_t newY){
    switch (levelSelect)
    {
    case 0:
      drawPath(players[playerIndex][_x_],players[playerIndex][_y_]);
      break;
    
    case 1:
      drawPath(players2[playerIndex][_x_],players2[playerIndex][_y_]);
      break;
    }
    
    drawPlayer(playerIndex,newX,newY);
}


void movePlayerNunchuk(uint8_t playerIndex){
    // Serial.println(playerIndex);
    uint16_t newX;
    uint16_t newY;

    switch (levelSelect)
    {
    case 0:
      newX = players[playerIndex][_x_];
      newY = players[playerIndex][_y_];
      break;
    
    case 1:
      newX = players2[playerIndex][_x_];
      newY = players2[playerIndex][_y_];
      break;
    } 
    
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

  uint16_t* coordPtr;// = NULL;
  switch (levelSelect)
    {
    case 0:
      coordPtr = walkTo(players[playerIndex][_x_],players[playerIndex][_y_],newX,newY);
      break;
    
    case 1:
      coordPtr = walkTo(players2[playerIndex][_x_],players2[playerIndex][_y_],newX,newY);
      break;
    }
    


    movePlayer(playerIndex,coordPtr[0],coordPtr[1]);
    delete coordPtr;
    
    

    
    

}


void moveGhostNunchuk(uint8_t ghostIndex){
    uint16_t newX;
    uint16_t newY;

    switch (levelSelect)
    {
    case 0:
      newX = ghosts[ghostIndex][_x_];
      newY = ghosts[ghostIndex][_y_];
      break;
    
    case 1:
      newX = ghosts2[ghostIndex][_x_];
      newY = ghosts2[ghostIndex][_y_];
      break;
    } 
    
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

  uint16_t* coordPtr;// = NULL;
  switch (levelSelect)
    {
    case 0:
      coordPtr = walkTo(ghosts[ghostIndex][_x_],ghosts[ghostIndex][_y_],newX,newY);
      break;
    
    case 1:
      coordPtr = walkTo(ghosts2[ghostIndex][_x_],ghosts2[ghostIndex][_y_],newX,newY);
      break;
    }
    
    moveGhost(ghostIndex,coordPtr[0],coordPtr[1],GHOST_COLOR);
    delete coordPtr;
}

void selectLevel(uint8_t level){

  switch (level)
  {
  case 0:
    levelSelect = 0;
    noCoins = false;
    noGhost = true;
    nunchukIsGhost = false;
    IrIsGhost = false;

    playerId = 0;
    IrId = 1;
    break;
  case 1:
    levelSelect = 1;
    noCoins = true;
    noGhost = false;
    nunchukIsGhost = false;
    IrIsGhost = true;

    playerId = 0;
    IrId = 0;
    
    break;
  }


}

void drawLevel(){
  tft.fillScreen(BACKGROUND);
  drawField();
  drawCoins();
  drawGhosts();
  switch (levelSelect)
  {
  case 0:
    for(int i =0;i<numPlayers;i++){
      drawPlayer(i,players[i][_x_],players[i][_y_]);
    }
    break;
  
  case 1:
    for(int i =0;i<numPlayers;i++){
      drawPlayer(i,players2[i][_x_],players2[i][_y_]);
    }
    break;
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


void sendCommand(uint8_t nunchukdata, uint8_t command)
{
  sendBegin();
  // sendByte(nunchukdata,true);
  sendByte(command,false);
  sendEnd();
}


void moveOverIR(uint8_t playerIndex)
{

    uint16_t newX;
    uint16_t newY;

    switch (levelSelect)
    {
    case 0:
      newX = players[playerIndex][_x_];
      newY = players[playerIndex][_y_];
      break;
    
    case 1:
      newX = players2[playerIndex][_x_];
      newY = players2[playerIndex][_y_];
      break;
    } 

  
    if(buffer > 10 && buffer < 256 && synchroniseer){
          syncPlayerLocation(decodeGridPosition(buffer), 1);
          synchroon = true;
    }
}
void syncPlayerLocation(uint16_t* coordPtr, uint8_t playerIndex)
{
  uint16_t* coordPtr2;// = NULL;
  switch (levelSelect)
  {
  case 0:
    coordPtr2 = walkTo(players[playerIndex][_x_],players[playerIndex][_y_],coordPtr[_x_],coordPtr[_y_]);
    break;
  
  case 1:
    coordPtr2 = walkTo(players2[playerIndex][_x_],players2[playerIndex][_y_],coordPtr[_x_],coordPtr[_y_]);
    break;
  }
  
  movePlayer(playerIndex, coordPtr2[_x_], coordPtr2[_y_]);
  delete coordPtr;
  delete coordPtr2;
}
//Scoreboard page
void showLevel(){
  tft.fillRect(245,190,60,40,BACKGROUND);
  if(levelSelect == 1){
    tft.setCursor(260,215);
  tft.println("Ghost");
  } else if(levelSelect == 0) {
    tft.setCursor(265,215);
    tft.println("1v1");
  }
}
void textGhost(){
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(260,10);
  tft.println("SCORE");
  tft.setCursor(245,70);
  tft.println("HIGHSCORE");
  tft.setCursor(260,130);
  tft.println("LIVES");
  tft.setCursor(260,190);
  tft.println("LEVEL");
}
void updateScore(uint16_t score){
    tft.fillRect(255,10,50,40,BACKGROUND);
    tft.setCursor(265,35);
    char printValue[10];
    sprintf(printValue,"%04u",score);
    tft.println(printValue);
  }
void getHighscore(){
  tft.fillRect(245,75,60,40,BACKGROUND);
  tft.setCursor(265,95);
  char printValue[10];
  sprintf(printValue,"%04lu",scoreList[9]);
  tft.println(printValue);
}
void showLives(){
  tft.fillRect(245,140,70,40,BACKGROUND);
  switch (levelSelect)
  {
  case 0:
    for(int i = 0;i<playerLives[playerId];i++){
      tft.fillCircle(260+i*20,160,8,TFT_RED);
    }
    break;
  
  case 1:
    for(int i = 0;i<playerLives2[playerId];i++){
      tft.fillCircle(260+i*20,160,8,TFT_RED);
    }
    break;
  }
}
void valuesGhost(){
  tft.setTextColor(TFT_WHITE);
  updateScore(0);
  getHighscore();
  showLives();
  showLevel();
}
void setupScoreBoardGhost(){
  tft.fillRect(240,0,80,240,BACKGROUND);
  tft.setFont(&PressStart2P_vaV74pt7b);
  textGhost();
  valuesGhost();
}
//VS score bord functies
void textVS(){
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(270,10);
  tft.println("P1");
  tft.setCursor(270,70);
  tft.println("P2");
  tft.setCursor(255,130);
  tft.println("POINTS");
  tft.setCursor(260,190);
  tft.println("LEVEL");
}
void updateP1(uint8_t points){
  tft.setCursor(270,35);
  char printValue[2];
  sprintf(printValue,"%02i",points);
  tft.println(printValue);


}
void updateP2(uint8_t points){
  tft.setCursor(270,95);
  char printValue[2];
  sprintf(printValue,"%02i",points);
  tft.println(printValue);

}
void updateTT(uint8_t points){
  tft.setCursor(270,155);
  char printValue[2];
  sprintf(printValue,"%02i",points);
  tft.println(printValue);
}
void valuesVS(){
  tft.setTextColor(TFT_WHITE);
  updateP1(0);
  updateP2(0);
  // switch (levelSelect)
  // {
  // case 0:
  //   updateP1(playerResult[0]);
  //   updateP2(playerResult[1]);
  //   break;
  // case 1:
  //   updateP1(playerResult2[0]);
  //   updateP2(playerResult2[1]);
  //   break;
  // }
  
  // updateTT(0);
  showLevel();
}
void setupScoreBoardVS(){
  tft.fillRect(240,0,80,240,BACKGROUND);
  tft.setFont(&PressStart2P_vaV74pt7b);
  textVS();
  valuesVS();
}

void moveGhostOverIR(uint8_t ghostIndex)
{
    uint16_t newX;
    uint16_t newY;

    switch (levelSelect)
    {
    case 0:
      newX = ghosts[ghostIndex][_x_];
      newY = ghosts[ghostIndex][_y_];
      break;
    
    case 1:
      newX = ghosts2[ghostIndex][_x_];
      newY = ghosts2[ghostIndex][_y_];
      break;
    } 


    uint16_t* coordPtr;// = NULL;
    switch (levelSelect)
    {
      case 0:
        coordPtr = walkTo(ghosts[ghostIndex][_x_],ghosts[ghostIndex][_y_],newX,newY);
        break;
      
      case 1:
        coordPtr = walkTo(ghosts2[ghostIndex][_x_],ghosts2[ghostIndex][_y_],newX,newY);
        break;
    }
    moveGhost(ghostIndex,coordPtr[0],coordPtr[1],GHOST_COLOR);
    delete coordPtr;
}


//Highscore Page
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
  tft.setFont(&PressStart2P_vaV74pt7b);
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

void printbits(uint8_t byte)
{
  for (int16_t i = 7; i >= 0; i--)
  {
    if(byte & (1<<i))
    {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
    
  }
  Serial.println(" ");
  
}




void selectscherm()
{
  tft.fillScreen(BACKGROUND);
  tft.setCursor(50,65);
  tft.setTextColor(TFT_PURPLE);
  tft.setTextSize(3);
  tft.println("P1");

  tft.setCursor(180,65);
  tft.println("P2");

}

void setupGame(){
  selectLevel(levelSelect);
  setLives();
  setResetPosition();
  drawLevel();
}

void switchControlState(ControlStates newControlState){
  if(newControlState == _GAME){
    setupGame();
    
    switch (levelSelect)
    {
    case 0:
      setupScoreBoardVS();
      break;
    
    case 1:
      setupScoreBoardGhost();
      break;
    }

  }else if(newControlState == _MENU){
    selectscherm();
  }

  controlState = newControlState;
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
  extern screens cScreen; // current screen // extern variable in header file gives linker error, this works, not ideal...
  extern screens nScreen; // new screen
  extern uint32_t touchX;
  extern uint32_t touchY;
  actions action = NO_ACTION;
  
   
    initIR();
    Serial.begin(BAUDRATE);
    Wire.begin();

    if(!setupDisplay()){return 0;}
    if (!ctp.begin(40, &Wire)) { return 1; }
  
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



    // HighScorePage();
    // _delay_ms(2000);

   


    // drawLevel();
    // setupScoreBoardVS();
    
    
    // drawLevel();

    selectLevel(0);
    switchControlState(_MENU);
    // Serial.print()
    while(1)
    {
      getNunchukPosition();
      nunchuckWrap();
      switch(controlState){
        case _GAME:
          
          sendCommand(nunchuckWrap(), nunchuckWrap());

          if(IrIsGhost){
            moveGhostOverIR(IrId);
          }else{
            moveOverIR(IrId);
          }

          if(nunchukIsGhost){
            moveGhostNunchuk(playerId); 
          }else{
            movePlayerNunchuk(playerId);
          }
            
          //  
          collision();
          if(endGame()){switchControlState(_MENU);}
          break;
        case _PLAYERMENU:
          // character selection /////////////////////////////////////////
          if (NunChuckPosition[2] != lastButtonState)
          {
            tft.setTextColor(TFT_WHITE);
            tft.setTextSize(1);
            
            if (!NunChuckPosition[2])
            {
              tft.setCursor(50, 120);
              tft.fillRect(50, 120, 50, 25, TFT_BLACK);
              if (GhostOfPacman)
              {
                tft.println("Ghost");
                GhostOfPacman = !GhostOfPacman;
                sendCommand(0b00000000, 0b11001100);
              }
              else
              {
                tft.println("PacMan");
                GhostOfPacman = !GhostOfPacman;
                sendCommand(0b00000000, 0b00110011);
              }
            }
          }
          lastButtonState = NunChuckPosition[2];
          if(buffer == 204){
            tft.setTextColor(TFT_WHITE);
            tft.setTextSize(1);
            tft.setCursor(180,120);
            tft.fillRect(180,120, 50,25,TFT_BLACK);
            tft.println("Ghost");
            buffer = 0;  
          }
          if(buffer == 51){
            tft.setTextColor(TFT_WHITE);
            tft.setTextSize(1);
            tft.setCursor(180,120);
            tft.fillRect(180,120, 50,25,TFT_BLACK);
            tft.println("PacMan"); 
            buffer = 0; 
          }
        // character selection /////////////////////////////////////////

        break;
      case _MENU:
      
        // get events for screen
        getNunchukPosition(); // Should be setNunchukPosition
        if (NunChuckPosition[3]) // if Z is pressed. click button
        {
          action = SELECTBUTTON;
        } else if (NunChuckPosition[2]) // if c is pressed. move to next button
        {
          action = NEXTBUTTON;
          while(NunChuckPosition[2]){getNunchukPosition();} // wait for c release, otherwise too fast
        } else if (ctp.touched())
        {
          TS_Point p = ctp.getPoint();
          touchX = 320 - p.y;
          touchY = p.x;
          action = TOUCH;
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
      break;   
    } 
  }

    HighScorePage();
    return 0;
}


