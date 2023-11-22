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
const uint8_t ball_radius = 50;//testing
bool testBool1 = true;//testing 
bool testBool2 = true;//testing



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


int main(){
    setupDisplay();
    tft.fillScreen(ILI9341_RED);
    while(true){
        
        //scrappy bouncing ball
        tft.fillScreen(ILI9341_BLUE);
        tft.fillCircle(ball_Pos_X,ball_Pos_Y,ball_radius,ILI9341_GREEN);
        updateBallPos();
        _delay_ms(100);

    }  
return 0;
}