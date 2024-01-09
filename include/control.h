#ifndef CONTROL_H
#define CONTROL_H

typedef unsigned char uint8_t;

enum ControlStates {_GAME, _MENU, _PLAYERMENU};
void switchControlState(ControlStates);
void selectLevel(uint8_t);


#endif // CONTROL_H
