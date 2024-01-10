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
#include <score.h>
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
    selectLevel(0);
    switchControlState(_MENU);

    while(1)
    {
      getNunchukPosition();
      nunchuckWrap();
      switch(controlState)
      {
        case _GAME:
          // sendCommand(nunchuckWrap(), nunchuckWrap());

          if(IrIsGhost){
            moveGhostOverIR(IrId);
          }else{
            moveOverIR(IrId);
          }

          if(nunchukIsGhost){
            moveGhostNunchuk(playerId);
            
            switch (levelSelect)
            {
            case 0:
              sendCommand(nunchuckWrap(), encodeGridPosition(ghosts[playerId])); 
              break;
            
            case 1:
              sendCommand(nunchuckWrap(), encodeGridPosition(ghosts2[playerId])); 
              break;
            }
            
          }else{
            movePlayerNunchuk(playerId);
            switch (levelSelect)
            {
            case 0:
              sendCommand(nunchuckWrap(), encodeGridPosition(players[playerId])); 
              break;
            
            case 1:
              sendCommand(nunchuckWrap(), encodeGridPosition(players2[playerId])); 
              break;
            }
          }
            
          //  
          collision();
          // if(endGame()){switchControlState(_MENU);}
          if(endGame()){runGame=false; switchControlState(_MENU);nScreen=LOADING_SCREEN;}
          break;
        case _PLAYERMENU:
          if (NunChuckPosition[2] != lastButtonState)
          {
            Serial.println("switch");
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
                nunchukIsGhost = true;
                playerMenuSet = true;
                playerId = 0;//ghosts Id
              }
              else
              {
                tft.println("PacMan");
                GhostOfPacman = !GhostOfPacman;
                sendCommand(0b00000000, 0b00110011);
                nunchukIsGhost = false;
                playerMenuSet = true;
                playerId = 0;//players Id
              }
            }
          }
          lastButtonState = NunChuckPosition[2];
          if(buffer == 204){
            Serial.println("switching 1");

            tft.setTextColor(TFT_WHITE);
            tft.setTextSize(1);
            tft.setCursor(180,120);
            tft.fillRect(180,120, 50,25,TFT_BLACK);
            tft.println("Ghost");
            GhostOfPacmanIR = false;
            IrIsGhost = true;
            IrId = 0;//ghosts Id
            buffer = 0;
          }
          if(buffer == 51){
            Serial.println("switching 2");

            tft.setTextColor(TFT_WHITE);
            tft.setTextSize(1);
            tft.setCursor(180,120);
            tft.fillRect(180,120, 50,25,TFT_BLACK);
            tft.println("PacMan"); 
            IrIsGhost = false;
            IrId = 1;//players Id
            GhostOfPacmanIR = true;

            buffer = 0; 
          }

          if(nunchukIsGhost != IrIsGhost){  
            if(NunChuckPosition[3] !=lastButtonState2) // als de onderste knop op de nunchuk is ingedrukt
            {
              Serial.println("confirm");

              sendCommand(0b0, 0b11110000); // stuur infrarood naar de andere dat de game start
            }
            if(buffer == 240) // als de arduino dit binnenkrijgt dan starten
            {
              Serial.println("confirming");
              Serial.print("_");
              Serial.print(nunchukIsGhost);
              Serial.print(":");
              Serial.print(playerId);
              Serial.print("_");
              Serial.print(IrIsGhost);
              Serial.print(":");
              Serial.print(IrId);


              sendCommand(0b0, 0b11110000);
              runGame = true;
              switchControlState(_GAME);
              
            }
            lastButtonState2 = NunChuckPosition[3];
          }
        
        
        
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
          case HIGHSCORE_SCREEN:
            if (cScreen != nScreen)
            {
              initScoreScreen(tft);
              cScreen = HIGHSCORE_SCREEN;
            }
            else
            {
              handleScoreScreen(tft, action);
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


