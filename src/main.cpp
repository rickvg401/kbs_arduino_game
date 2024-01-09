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
#include <control.h>
// #include "display/fonts/PressStart2P_vaV74pt7b.h"
#include "display/fonts/PressStart2P_vaV74pt7b.h"
//general
#define F_CPU 16000000ul
#define _x_ 0
#define _y_ 1

#include "notMain"

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


