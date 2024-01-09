#include <avr/interrupt.h>
#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#include "SPI.h"
#include "display/Adafruit-GFX/Adafruit_GFX.h"
#include "display/Adafruit_ILI9341.h"
#include <EEPROM.h>
#include "display/fonts/PressStart2P_vaV76pt7b.h"
#include <util/delay.h>
#include <sound.h>
#include <notes.h>
#include "display/fonts/PressStart2P_vaV74pt7b.h"
#define HIGHLIGHT 0x0861
#define TFT_DC 9
#define TFT_CS 10
#define nunchuk_ID 0xA4 >> 1
#define BACKGROUND  ILI9341_BLACK
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
uint8_t nunchuckData = 0b0000;
uint8_t NunChuckPosition[4];
uint8_t PrevNunChuckPosition[4];
bool NunChuckPositionDivided = false;
char letters[27] = {' ', 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
char input[5] = "";
uint8_t index[5] = {0,0,0,0,0};
int idx = 0;
bool filled = false;
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
void printHeader(){
    tft.setFont(&PressStart2P_vaV74pt7b);
    tft.setTextSize(2);
    tft.setCursor(50,20);
    tft.println("NEW HIGHSCORE!");
    tft.setTextSize(1);
    tft.setCursor(90,35);
    tft.println("Fill in your name");
    tft.setFont(&PressStart2P_vaV76pt7b);
}
void printName(){
    tft.fillRect(115,105,80,30,BACKGROUND);
    tft.setTextSize(1);
    tft.setCursor(200,120);
    tft.println("CONFIRM");
    for(int i = 0;i<5;i++){
        tft.fillRect(118+15*i,107,14,14,HIGHLIGHT);
        tft.setCursor(120+15*i,120);
        tft.print(letters[index[i]]);
        if(i == idx){
            tft.setCursor(120+15*i,121);
            tft.println("_");
        }
    }
}
void addScore(){
    tft.fillScreen(BACKGROUND);
    _delay_ms(5000);
    filled = true;
}
bool naaminvullen(){
    tft.drawLine(200,121,282,121,TFT_YELLOW);
    while(idx == 5){
        getNunchukPosition();
        if(NunChuckPosition[3] == 1 && PrevNunChuckPosition[3] == 0){

            tft.drawLine(200,121,282,121,BACKGROUND);
            return true;
    } else if(NunChuckPosition[2] == 1 && PrevNunChuckPosition[2] == 0){
        idx = 0;
        for(int i = 0;i<5;i++){
            index[i] = 0;
            input[i] = ' ';
        }
        addScore();
        return true;
    }
    for(int i = 0; i<4;i++){
        PrevNunChuckPosition[i] = NunChuckPosition[i];
    }
    }
}
void inputLetters(){
    if(NunChuckPosition[3] == 1 && PrevNunChuckPosition[3] == 0){
        if(idx > 0){
            idx--;
        } else {
            idx = 0;
        }
        printName();
    } 
    if(NunChuckPosition[1] > 128 && PrevNunChuckPosition[1] == 128){
        if(index[idx] < 26){
            index[idx]++;
        } else {
            index[idx] = 0;
        }
        printName();
    } else if(NunChuckPosition[1] < 128 && PrevNunChuckPosition[1] == 128){
        if(index[idx] > 0){
            index[idx]--;
        } else {
            index[idx] = 26;
        }
        printName();
    } 
    if(NunChuckPosition[2] == 1 && PrevNunChuckPosition[2] == 0){
        if(idx < 5){
            idx++;
            printName();
        }
        if(idx == 5){
            for(int i = 0; i<4;i++){
        PrevNunChuckPosition[i] = NunChuckPosition[i];
    }
            if(naaminvullen()){
                return;
            }
        }
    }
    for(int i = 0; i<4;i++){
        PrevNunChuckPosition[i] = NunChuckPosition[i];
    }
}
bool setupDisplay(){
    //returned if setup is correctly completed
    tft.begin();
    tft.setRotation(1);//origin top left 
    return true;
}
void newHighscore(){
    tft.fillScreen(BACKGROUND);
    tft.setFont(&PressStart2P_vaV76pt7b);
    tft.setTextColor(TFT_YELLOW);
    printHeader();
    printName();
    while(!filled){
        getNunchukPosition();
        inputLetters();
    }
}
int main(){
    sei();
    if(!setupDisplay()){return 0;}
    newHighscore();

}