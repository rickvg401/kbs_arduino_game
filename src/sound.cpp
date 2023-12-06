#include "sound.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Timer 2 and pin OC2B (pin 3 on arduino) is used for sound

volatile uint16_t duration = 0;
uint32_t maxDuration = 0;
music* cMusic;
volatile uint8_t musicCt = 0;
uint8_t loop = false;

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
  if (duration++ > maxDuration)
  {
    musicCt++;
    if (musicCt > cMusic->length)
    {
      if (loop)
      {
        musicCt = 0;
      } else 
      {
        stopMusic();
        return;
      }
    }
    setTone(cMusic->frequencies[musicCt]);
    setDuration(cMusic->durations[musicCt]);
  }
}

void stopMusic() // Disable prescaler to stop the counters
{
  TCCR1B = (TCCR1B & 0xF8);
  TCCR2B = (TCCR2B & 0xF8);
}

ISR(TIMER1_COMPA_vect) // Flip OC2B pin mode, for frequency
{
  TCCR2A ^= (1 << COM2B1);
}

void setVolume(uint8_t newVolume) // Set dutyCycle in timer2 for volume
{
  OCR2B = newVolume;
}

void setTone(uint16_t frequency) // frequency in hertz, duration in ms.
{
  uint16_t ocr = 31250 / frequency - 1;
  OCR1AH = (ocr >> 8);
  OCR1AL = ocr;
}

void loadMusic(music* musicStruct)
{
  cMusic = musicStruct;
}

void playMusic()
{
  TCNT1H = 0;
  TCNT1L = 0;
  TCNT2 = 0;

  musicCt = 0;
  setTone(cMusic->frequencies[musicCt]);
  setDuration(cMusic->durations[musicCt]);

  TCCR2B = (1 << CS20); // 1x prescalor for volume // start prescaler to start the timer
  TCCR1B |= (1 << CS12); // 256x prescaler for tone // start prescaler to start the timer
}

void enableLoop()
{
  loop = true;
}

void disableLoop()
{
  loop = false;
}

void setDuration(uint16_t newDuration)
{
  maxDuration = 62500 * ((float) newDuration / 1000);
  duration = 0;
}