#include <avr/interrupt.h>
#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#include "SPI.h"
#include "display/Adafruit-GFX/Adafruit_GFX.h"
#include "display/Adafruit_ILI9341.h"
#include <util/delay.h>

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

#define nunchuk_ID 0xA4 >> 1

// unsigned char buffer[4];// array to store arduino output
uint8_t NunChuckPosition[4];
bool NunChuckPositionDivided = false;

/*display*/
#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//game
const int GAMECLOCK = 15; //ball updates every x times per second
// uint16_t playerPosX;
// uint16_t playerPosY;
uint8_t numPlayers = 2;
uint8_t playablePlayer = 0;
uint16_t* playerVector = NULL;
uint8_t nunchuckData = 0b0000;

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


// infrared vars
volatile uint16_t prevCounterValue = 0;
volatile uint16_t currentCounterValue = 0;
volatile uint16_t pulseDuration = 0;
volatile uint8_t eenofnull = 2;
volatile uint16_t counter = 0;


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
void drawField(uint8_t [FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION]);
void walkPlayer(uint8_t, uint8_t);
void drawPlayer(uint16_t color);
void drawCoins();
void drawGhosts();
bool canWalk(uint8_t, uint8_t);
// void setSurrounding(uint8_t, uint8_t);





//game level
const uint8_t COINS_LENGTH  = 36;
const uint8_t GHOSTS_LENGTH = 13;
const uint8_t PLAYER_LENGTH = 9;
uint16_t coins[36][2] = {
{9*BLOCK_SIZE,3*BLOCK_SIZE},
{8*BLOCK_SIZE,3*BLOCK_SIZE},
{9*BLOCK_SIZE,4*BLOCK_SIZE},
{9*BLOCK_SIZE,5*BLOCK_SIZE},
{8*BLOCK_SIZE,4*BLOCK_SIZE},
{8*BLOCK_SIZE,5*BLOCK_SIZE},
{8*BLOCK_SIZE,14*BLOCK_SIZE},
{3*BLOCK_SIZE,15*BLOCK_SIZE},
{10*BLOCK_SIZE,13*BLOCK_SIZE},
{9*BLOCK_SIZE,15*BLOCK_SIZE},
{14*BLOCK_SIZE,7*BLOCK_SIZE},
{14*BLOCK_SIZE,15*BLOCK_SIZE},
{18*BLOCK_SIZE,13*BLOCK_SIZE},
{21*BLOCK_SIZE,9*BLOCK_SIZE},
{23*BLOCK_SIZE,8*BLOCK_SIZE},
{22*BLOCK_SIZE,8*BLOCK_SIZE},
{24*BLOCK_SIZE,4*BLOCK_SIZE},
{19*BLOCK_SIZE,13*BLOCK_SIZE},
{16*BLOCK_SIZE,7*BLOCK_SIZE},
{23*BLOCK_SIZE,5*BLOCK_SIZE},
{20*BLOCK_SIZE,6*BLOCK_SIZE},
{11*BLOCK_SIZE,4*BLOCK_SIZE},
{20*BLOCK_SIZE,3*BLOCK_SIZE},
{18*BLOCK_SIZE,4*BLOCK_SIZE},
{15*BLOCK_SIZE,4*BLOCK_SIZE},
{28*BLOCK_SIZE,3*BLOCK_SIZE},
{26*BLOCK_SIZE,18*BLOCK_SIZE},
{24*BLOCK_SIZE,18*BLOCK_SIZE},
{26*BLOCK_SIZE,20*BLOCK_SIZE},
{14*BLOCK_SIZE,18*BLOCK_SIZE},
{15*BLOCK_SIZE,18*BLOCK_SIZE},
{18*BLOCK_SIZE,19*BLOCK_SIZE},
{20*BLOCK_SIZE,19*BLOCK_SIZE},
{8*BLOCK_SIZE,20*BLOCK_SIZE},
{6*BLOCK_SIZE,18*BLOCK_SIZE},
{9*BLOCK_SIZE,18*BLOCK_SIZE},
};
uint16_t players[9][2] = {
{29*BLOCK_SIZE,21*BLOCK_SIZE},
{29*BLOCK_SIZE,22*BLOCK_SIZE},
{29*BLOCK_SIZE,20*BLOCK_SIZE},
{30*BLOCK_SIZE,20*BLOCK_SIZE},
{30*BLOCK_SIZE,21*BLOCK_SIZE},
{30*BLOCK_SIZE,22*BLOCK_SIZE},
{28*BLOCK_SIZE,22*BLOCK_SIZE},
{28*BLOCK_SIZE,21*BLOCK_SIZE},
{28*BLOCK_SIZE,20*BLOCK_SIZE},
};
uint16_t ghosts[13][2] = {
{3*BLOCK_SIZE,5*BLOCK_SIZE},
{5*BLOCK_SIZE,4*BLOCK_SIZE},
{2*BLOCK_SIZE,3*BLOCK_SIZE},
{1*BLOCK_SIZE,6*BLOCK_SIZE},
{2*BLOCK_SIZE,4*BLOCK_SIZE},
{2*BLOCK_SIZE,5*BLOCK_SIZE},
{3*BLOCK_SIZE,9*BLOCK_SIZE},
{1*BLOCK_SIZE,9*BLOCK_SIZE},
{3*BLOCK_SIZE,11*BLOCK_SIZE},
{2*BLOCK_SIZE,11*BLOCK_SIZE},
{6*BLOCK_SIZE,10*BLOCK_SIZE},
{8*BLOCK_SIZE,9*BLOCK_SIZE},
{5*BLOCK_SIZE,8*BLOCK_SIZE},
};
uint8_t field[FIELD_HEIGHT][FIELD_WIDTH / FIELD_DIVISION] = {
{0xFF,0xFF,0xFF,0xFF,},
{0x80,0x00,0x00,0x01,},
{0xFF,0xE3,0xFF,0xF1,},
{0x80,0x22,0x04,0x01,},
{0x80,0x22,0x04,0x41,},
{0x80,0x20,0x04,0x41,},
{0x84,0x60,0xC4,0xC1,},
{0x87,0x80,0x07,0x9F,},
{0x80,0x00,0x00,0x1D,},
{0x80,0x00,0x00,0x17,},
{0xB4,0x80,0x00,0x1E,},
{0x84,0x80,0x00,0x1B,},
{0x85,0x83,0x80,0x1F,},
{0x84,0x0F,0x80,0x01,},
{0x84,0x18,0xFC,0x01,},
{0x80,0x10,0x00,0x01,},
{0x80,0x10,0x00,0x01,},
{0xC7,0x80,0x01,0xF1,},
{0xE0,0x80,0xE1,0x11,},
{0xF0,0xC7,0x81,0x01,},
{0xF8,0x00,0x01,0x01,},
{0xFC,0x00,0x00,0x01,},
{0xFC,0x00,0x00,0x01,},
{0xFF,0xFF,0xFF,0xFF,},
};

// volatile direction cd = EAST;

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

void drawCoins(){
  for(int i=0;i<COINS_LENGTH;i++){
    tft.fillRoundRect(coins[i][0], coins[i][1], BLOCK_SIZE, BLOCK_SIZE,100, COIN_COLOR);
  }
}

void collectCoins(){
  

  for(int i =0;i<COINS_LENGTH;i++){

  }
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
  
  Serial.println(" ");
  return coords;
}



void sendNEC(uint8_t data) {
 
      if(data == 1 ) // && !logicalzero && !logicalone
      {
        // Verzend een logische '1'
        // begin counter van 0 en tot die tijd zet infrared aan 
        counter = 0;
        logicalone = true;
        TCCR0A |= (1<< COM0A0); // toggle pin met 38Khz
      }
      else if (data == 0 ) 
      {
        // Verzend een logische '0'
        // begin counter van 0 en tot die tijd zet infrared aan
        counter = 0;
        logicalzero = true;
        TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
      }
      while(logicalone || logicalzero || logicalend || logicalbegin)
      {     
      }

}

void sendEnd()
{
    counter = 0;
    logicalend = true;
    TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
    while(logicalone || logicalzero || logicalend || logicalbegin)
    {      
    }
}

void sendBegin()
{
    counter = 0;
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

    // opgaande flank bepaal je het verschil tussen huidige counterstand en de onthouden counterstand
    // Je hebt dan gemeten hoe lang de puls duurde. Daarmee kan je
    // bepalen of het uitgezonden bit een 0 of een 1 was. Schuif dit
    // bit in het resulterende buffer.
     currentCounterValue = counter;
     pulseDuration = currentCounterValue - prevCounterValue;
    if (pulseDuration > 290 && pulseDuration < 320)
    {
      end = 1;
      bufferdata = buffer >> 4;
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

    } else if (pulseDuration < 95 && pulseDuration > 65) {
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

  if (logicalone == true)
  {
    currentcounterone = counter;
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
    currentcounterzero = counter;
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
    currentcounterbegin = counter;
    if(currentcounterbegin > 750)
    {
      logicalbegin = false;
    }
    if((currentcounterbegin) >= (268)) 
    {
      TCCR0A &= ~(1 << COM0A0);
    }
  }

  //end 310
  if(logicalend == true)
  {
    currentcounterend = counter;
    if(currentcounterend > 5000) // 325
    {
      logicalend = false;
    }
    if((currentcounterend) >= (310))
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
  for(int i =0;i<numPlayers;i++){
    drawPlayer(i,players[i][_x_],players[i][_y_]);
  }
  // drawPlayer(0,players[0][_x_]*BLOCK_SIZE,players[0][_y_]*BLOCK_SIZE);
  // drawPlayer(1,players[1][_x_]*BLOCK_SIZE,players[1][_y_]*BLOCK_SIZE);
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
      nunchuckData = (1<<1) | nunchuckData;
  } else if( NunChuckPosition[1] < 128 ){
      nunchuckData = (1<<0) | nunchuckData;
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
  sendByte(command,false);
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
    sei();
    initIR();
    Serial.begin(BAUDRATE);
    Wire.begin();

    if(!setupNunchuck()){return 0;}
    if(!setupDisplay()){return 0;}

    drawLevel();

    // Serial.print()
    while(1)
    {
        // uint8_t data = 0b00001101;
        // sendNEC(data); // Voorbeeld: Verstuur een testsignaal met waarde 0x00FF  
        // sendnec in een for loop 8 keer aanroepen !!!!
      // nunchuck en display
        getNunchukPosition();
        sendCommand(0b1101, nunchuckWrap());
        // Serial.print("buffer result-> ");
        // Serial.println(buffer);

        moveOverIR(1);
        movePlayerNunchuk(playablePlayer);
        // delay(100);

        
    } 
    return 0;
}

