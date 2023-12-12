#ifndef SOUND_H
#define SOUND_H

#ifndef F_CPU  // CPU frequency needs to be defined
#define F_CPU 16000000ul
#endif

#include <stdint.h>

#define BUZZPIN 3

struct music{
    uint16_t* frequencies;
    uint16_t* durations;
    uint8_t length;
};

void setupBuzzer();
void setTone(uint16_t frequency);
void setVolume(uint8_t newVolume);
void loadMusic(music* music_t);
void playMusic();
void setDuration(uint16_t newDuration);
void stopMusic();
void enableLoop();
void disableLoop();


#endif // SOUND_H 