
#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#define BAUDRATE 9600
#define nunchuk_ID 0xA4 >> 1
unsigned char buffer[4];// array to store arduino output

#include "SPI.h"
#include "display/Adafruit-GFX/Adafruit_GFX.h"
#include "display/Adafruit_ILI9341.h"
#include <util/delay.h>


/*display*/
#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//testing
uint8_t ball_Pos_X = 100;//testing
uint8_t ball_Pos_Y = 100;//testing
const uint8_t ball_radius = 10;//testing
bool testBool1 = true;//testing 
bool testBool2 = true;//testing

uint8_t playerPosX;
uint8_t playerPosY;

uint8_t NunChuckPosition[4];



bool setupDisplay(){
    //returned if setup is correctly completed

    tft.begin();
    

    return true;
}

void updateDisplay(){

}

void updateBallPos(){//testing
    if(testBool1){
        ball_Pos_X+=4;
        // ball_Pos_Y+=5;
    }else{
        ball_Pos_X-=8;
        // ball_Pos_Y-=5;
    }

    if(testBool2){
        // ball_Pos_X+=5;
        ball_Pos_Y+=10;
    }else{
        // ball_Pos_X-=5;
        ball_Pos_Y-=3;
    }

    if((ball_Pos_X < 50 || ball_Pos_X > 240)){
        if(testBool1){
            testBool1 = false;
        }else{testBool1 = true;}
        // testBool1 != testBool1;
    }
    if((ball_Pos_Y < 50 || ball_Pos_Y > 240)){
        // testBool2 != testBool2;
        if(testBool2){
            testBool2 = false;
        }else{testBool2 = true;}
    }

    // if((ball_Pos_X < 50 || ball_Pos_X > 240) || (ball_Pos_Y < 50 || ball_Pos_Y > 240)){
    //     testBool1 != testBool1;
    // }


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
    for(int i=0;i<100;i+=25){
        drawPath(i,200);
    }
}

bool setupNunchuck(){
    if(!Nunchuk.begin(nunchuk_ID)){
        Serial.println("******** No nunchuk found");
        Serial.flush();
        return(false);
    }
    Serial.print("-------- Nunchuk with Id: ");
	Serial.println(Nunchuk.id);
    return true;
}


void getNunchukPosition(){
    if(!Nunchuk.getState(nunchuk_ID)){
        return;
    } 

    NunChuckPosition[0] = Nunchuk.state.joy_x_axis;
    NunChuckPosition[1] = Nunchuk.state.joy_y_axis;
    NunChuckPosition[2] = Nunchuk.state.c_button;
    NunChuckPosition[3] = Nunchuk.state.z_button;
    
}


int main(){


    sei();
    Serial.begin(BAUDRATE);
    Wire.begin();

    if(!setupNunchuck()){return 0;}
    setupDisplay();


    tft.fillScreen(ILI9341_RED);
    drawLevel();
    drawPlayer(100,200);

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
        int start = millis();
        getNunchukPosition();

        // Serial.print(NunChuckPosition[0]);
        // Serial.print(":");
        // Serial.print(NunChuckPosition[1]);
        // Serial.print(":");
        // Serial.print(NunChuckPosition[2]);
        // Serial.print(":");
        // Serial.print(NunChuckPosition[3]);
        // Serial.println(" ");


        
        movePlayer(NunChuckPosition[1],NunChuckPosition[0]);
        int end = millis();
        Serial.println(end-start);
        // _delay_ms(50);
        // movePlayer(400,300);
        // _delay_ms(50);

        // movePlayer(200,200);
        // _delay_ms(50);

        // movePlayer(100,200);
        // _delay_ms(50);



        // updateBallPos();

    }  
    return 0;
}