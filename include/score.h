#ifndef SCORE_H
#define SCORE_H

#include <stdint.h>
#include <Adafruit_GFX.h>

void HighScorePage();
bool setupHighScore();
void getScore();
void setupScore();
void printHighScore(char names[10][6],uint32_t scores[10]);
void sort(char names[10][6],uint32_t scores[10], int pos[10]);
void addScore(char name[6], uint32_t points);

#endif // SCRORE_H