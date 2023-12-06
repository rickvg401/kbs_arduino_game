#include "sound.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <Arduino.h> // DELETE IN MAIN

// Timer 2 and pin OC2B (pin 3 on arduino) is used for sound

volatile uint32_t duration = 0;
uint32_t maxDuration = 0;
music* cMusic;
volatile uint8_t musicCt = 0;
uint8_t _loop = 0; // loop is already declared in Arduino.h. Using _loop instead of loop.

void setupBuzzer()
{  
  DDRD |= (1 << BUZZPIN); // set OC2B as output

  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); // SET (OC2B in non-inverting mode), set wgm to fastPWM // Timer 2 is used for volume
  TCCR1B = (1 << WGM12); // CTC mode // timer 1 is used for tone generation

  TIMSK1 = (1 << OCIE1A); // OCR1A is used for tone
  TIMSK2 = (1 << TOIE2); // enable overflow interrupt, used for duration of tone.
}

ISR(TIMER2_OVF_vect)
{
  duration++;
  if (duration > maxDuration) // If the maxDuration is reached. Continue with next tone
  {
    musicCt++;
    duration = 0;
    if (musicCt > cMusic->length-1)
    {
      if (_loop)
      {
        musicCt = 0;
      } else 
      {
        stopMusic();
        Serial.println(musicCt);
        Serial.println("stop");
        return;
      }
    }
    TCNT1H = 0; // reset counter registers
    TCNT1L = 0;
    TCNT2 = 0;
    setTone(cMusic->frequencies[musicCt]);
    setDuration(cMusic->durations[musicCt]);
  }
}

void stopMusic() // Unset prescaler to stop the counters
{
  TCCR1B = (TCCR1B & 0xF8);
  TCCR2B = (TCCR2B & 0xF8);
}

ISR(TIMER1_COMPA_vect) // Flip OC2B pin mode, for frequency
{
  TCCR2A ^= (1 << COM2B1);
}

void setVolume(uint8_t newVolume) // Set dutyCycle in timer2 for volume dutycycle is
{
  OCR2B = newVolume;
}

void setTone(uint16_t frequency) // frequency in hertz
{
  uint16_t ocr = (31250 / frequency) - 1;
  OCR1AH = (ocr >> 8);
  OCR1AL = ocr;
  Serial.println(ocr);
}

void loadMusic(music* musicStruct)
{
  cMusic = musicStruct;
}

void playMusic()
{
  TCNT1H = 0; // reset counter registers
  TCNT1L = 0;
  TCNT2 = 0;
  musicCt = 0;

  setTone(cMusic->frequencies[musicCt]); // Set correct ocr value in timer1
  setDuration(cMusic->durations[musicCt]); // set correct 

  TCCR2B = (1 << CS20); // 1x prescalor for volume // start prescaler to start the timer
  TCCR1B |= (1 << CS12); // 256x prescaler for tone // start prescaler to start the timer
}

void enableLoop() // set loop variable
{
  _loop = 1;
}

void disableLoop() // unset loop variable
{
  _loop = 0;
}

void setDuration(uint16_t newDuration) // set the amount of 
{
  maxDuration = 62500 * ((float) newDuration / 1000);
  duration = 0;
}