#include <control.h>
#include <stdint.h>

extern void setupGame();
extern void setupScoreBoardVS();
extern void setupScoreBoardGhost();
extern void selectscherm();
extern void selectLevel(uint8_t level);

uint8_t levelSelect = 0;
ControlStates controlState = _PLAYERMENU;

void switchControlState(ControlStates newControlState){
  if(newControlState == _GAME){
    setupGame();
    
    switch (levelSelect)
    {
    case -1:
      setupScoreBoardVS();
      break;
    
    case 0:
      setupScoreBoardGhost();
      break;
    }

  }else if(newControlState == _MENU){
    selectscherm();
  }

  controlState = newControlState;
}
