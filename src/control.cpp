#include <control.h>
#include <stdint.h>


extern void setupGame();
extern void setupScoreBoardVS();
extern void setupScoreBoardGhost();
extern void selectscherm();
extern void selectLevel(uint8_t level);
extern bool runGame;
enum Levels {_LEVEL1,_LEVEL2};
// extern enum _LEVEL2;
extern void initWDT();

uint8_t levelSelect = _LEVEL1;
ControlStates controlState = _PLAYERMENU;

void switchControlState(ControlStates newControlState){
  if(newControlState == _GAME){
    

    switch (levelSelect)
    {
    case _LEVEL1:
      setupGame();
      setupScoreBoardVS();
      break;

    case _LEVEL2:
      // switchControlState(_PLAYERMENU);
      if(!runGame){
        newControlState = _PLAYERMENU;
        selectscherm();
      }else{
        setupGame();
        setupScoreBoardGhost();
        initWDT();
      }
      
      break;
    }
  } else if(newControlState == _PLAYERMENU){
    selectscherm();
  }

  controlState = newControlState;
}
