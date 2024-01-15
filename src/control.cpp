#include <control.h>
#include <stdint.h>


extern void setupGame();
extern void setupScoreBoardVS();
extern void setupScoreBoardGhost();
extern void selectscherm();
extern void selectLevel(uint8_t level);
extern bool runGame;

uint8_t levelSelect = 0;
ControlStates controlState = _PLAYERMENU;

void switchControlState(ControlStates newControlState){
  if(newControlState == _GAME){
    

    switch (levelSelect)
    {
    case 0:
      // setupScoreBoardVS();
      setupGame();
      break;

    case 1:
      // switchControlState(_PLAYERMENU);
      if(!runGame){
        newControlState = _PLAYERMENU;
        selectscherm();
      }else{
        setupGame();
        setupScoreBoardGhost();
      }
      
      break;
    }
  } else if(newControlState == _PLAYERMENU){
    selectscherm();
  }

  controlState = newControlState;
}
