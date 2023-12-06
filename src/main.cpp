#define F_CPU 16000000ul

#include <sound.h>
#include "notes.h"
#include <avr/interrupt.h>
#include <Arduino.h>

struct music test;
namespace notes
{
  uint16_t testFrequencies[] = {_C3, _D3, _E3, _F3, _G3, _A3, _B3}; 
  uint16_t testDurations[] = {1000, 1000 ,1000 ,1000 , 1000, 1000, 1000};
}


int main()
{
  test.frequencies = notes::testFrequencies;
  test.durations = notes::testDurations;
  test.length = 5;

  setupBuzzer();
  loadMusic(&test);
  setVolume(200);
  playMusic();
  sei();
  while(1);
}


