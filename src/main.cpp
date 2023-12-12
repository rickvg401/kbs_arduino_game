#define F_CPU 16000000ul

#include <sound.h>
#include "notes.h"
#include <avr/interrupt.h>
#include <Arduino.h>

struct music test;
struct music pacmanTheme;
namespace notes
{

  uint16_t pacmanNotes[] = {_C4, _C5, _G4, _E4, _C5, _G4, _C4S, _C5S, _G4S, _F4, _G4S, _F4, _C4, _C4, _C5, _G4, _E4, _C5, _G4, _E4, _D4, _D4S,_E4, _E4, _F4, _F4S, _F4, _F4S, _G4, _C5}; 
  uint16_t pacmanDurations[sizeof(pacmanNotes) / sizeof(uint16_t)] = {300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
}

int main()
{
  Serial.begin(9600);
  test.frequencies = notes::pacmanNotes;
  test.durations = notes::pacmanDurations;
  test.length = sizeof(notes::pacmanNotes) / sizeof(uint16_t);

  setupBuzzer();
  loadMusic(&test);
  setVolume(50);
  enableLoop();
  playMusic();
  sei();

  while(1);
}


