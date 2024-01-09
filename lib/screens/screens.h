#ifndef SCREEN_H
#define SCREEN_H

#include <Adafruit_ILI9341.h>
#include <stdint.h>
#include <score.h>

const uint16_t ACTIVE_BUTTON_COLOR = ILI9341_ORANGE;

enum screens {NO_SCREEN, LOADING_SCREEN, LEVEL_SCREEN, MENU_SCREEN, HIGHSCORE_SCREEN};

enum actions {NO_ACTION, SELECTBUTTON, NEXTBUTTON, RETURN, TOUCH};


struct buttons 
{
    uint16_t x, y, width, height;
    uint8_t textOffsetX, textOffsetY;
    const char* text;
    uint16_t textColor;
    uint16_t backgroundColor;
    void (*action)();
};

void initLoadingScreen(Adafruit_ILI9341 &);
void initMenuScreen(Adafruit_ILI9341 &);
void initLevelScreen(Adafruit_ILI9341 &);
void initScoreScreen(Adafruit_ILI9341 &);

void handleLoadingScreen(Adafruit_ILI9341 &, actions);
void handleMenuScreen(Adafruit_ILI9341 &, actions);
void handleLevelScreen(Adafruit_ILI9341 &, actions);
void handleScoreScreen(Adafruit_ILI9341 &, actions);

#endif // SCREEN_H