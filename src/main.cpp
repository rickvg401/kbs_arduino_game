#define F_CPU 16000000ul

#include <sound.h>
#include "notes.h"
#include <avr/interrupt.h>
#include <Arduino.h>

struct music test;
struct music pacmanTheme;
namespace notes
{
  uint16_t testNotes[] = {_C3, _D3, _E3, _F3};
  uint16_t testDurations[] = {100, 100, 100, 100};

  uint16_t pacmanNotes[] = {_B4, _A5S, _C5S, _D5, _C5, _C6, _G5, _E5, _C6, _G5, _E5, _B4, _A5S, _C5S, _D5, _D5, _E5, _F5, _E5, _G5, _G5, _G5, _A5, _B5};
  uint16_t pacmanDurations[sizeof(pacmanNotes) / sizeof(uint16_t)] = {100, 100, 100, 100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};
}


int main()
{
  Serial.begin(9600);
  test.frequencies = notes::pacmanNotes;
  test.durations = notes::pacmanDurations;
  test.length = sizeof(notes::pacmanNotes) / sizeof(uint16_t);

  setupBuzzer();
  loadMusic(&test);
  setVolume(100);
  playMusic();
  sei();

  while(1);
}


