
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
bool NunChuckPositionDivided = true;

/*display*/
#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//game
uint8_t playerPosX;
uint8_t playerPosY;




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



void drawPlayer(uint8_t x, uint8_t y){
    tft.fillRect(x,y,25,25,ILI9341_DARKCYAN);
    playerPosX = x;
    playerPosY = y;
}

void drawPath(uint8_t x, uint8_t y){
    tft.fillRect(x,y,25,25,ILI9341_BLACK);
}

void movePlayer(uint8_t newX,uint8_t newY){
    drawPath(playerPosX,playerPosY);
    drawPlayer(newX,newY);
}

void drawLevel(){
    // for(int i=0;i<100;i+=25){
    //     drawPath(i,200);
    // }
}


void getNunchukPosition(){
    if(!Nunchuk.getState(nunchuk_ID)){
        return;
    } 
    uint8_t x = Nunchuk.state.joy_x_axis;
    uint8_t y = Nunchuk.state.joy_y_axis;
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


int main(){


    sei();
    #ifdef SerialActive
        Serial.begin(BAUDRATE);
    #endif
    Wire.begin();

    if(!setupNunchuck()){return 0;}
    if(!setupDisplay()){return 0;}


    tft.fillScreen(ILI9341_RED);
    // drawLevel();
    drawPlayer(128,128);

    

    /*
    while(1){
        if(!Nunchuk.getState(nunchuk_ID)){
            return (1);
        } 


        int x = Nunchuk.state.joy_x_axis;
	    int y = Nunchuk.state.joy_y_axis;
        int c = Nunchuk.state.c_button;
        int z = Nunchuk.state.z_button;
        const char* uitkomst[4];
        if(x != 128){
            uitkomst[0] = x<128 ? "links-" : "rechts-";}
        else{
            uitkomst[0] = "midden-";
        }
        if(y != 128){
                uitkomst[1] = y>128 ? "boven-" : "onder-";} 
        else{
                uitkomst[1] = "midden-";
        }
        uitkomst[2] = c ? "cin-" : "cout-";
        uitkomst[3] = z ? "zin" : "zout";
        for(int i = 0;i<4;i++){
            Serial.print(uitkomst[i]);
        }
        Serial.println(" ");
    }*/



    while(true){
        getNunchukPosition();

        movePlayer(NunChuckPosition[1],NunChuckPosition[0]);

    }  
    return 0;
}