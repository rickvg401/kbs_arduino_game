#include <screens.h>

screens nScreen = LOADING_SCREEN;
screens cScreen = NO_SCREEN;

// general buttons
buttons BTNreturn{0, 0, 15, 10, 1, 2, "<-", ILI9341_WHITE, ILI9341_RED, []()-> void 
{
    switch(nScreen)
    {
        case MENU_SCREEN:
            nScreen = LOADING_SCREEN;
            break;
        case LEVEL_SCREEN:
            nScreen = MENU_SCREEN;
            break;
        case HIGHSCORE_SCREEN:
            nScreen = MENU_SCREEN;
            break;
        default:
            nScreen = LOADING_SCREEN;
    }
}};

// loading screen buttons
buttons BTNpressTooStart{115, 200, 90, 20, 3, 5, "press to start", ILI9341_BLACK, ILI9341_BLUE, []() -> void 
{
    nScreen = MENU_SCREEN;
}};

// menu screen buttons
buttons BTNselectLevels{20, 20, 90, 20, 5, 5, "Select levels", ILI9341_WHITE, ILI9341_BLACK, []() -> void
{
    nScreen = LEVEL_SCREEN;
}};

buttons BTNseeHighscores{20, 50, 90, 20, 5, 5, "See highscores", ILI9341_WHITE, ILI9341_BLACK, []() -> void
{
    // show highscore
}};

// level screen buttons
buttons BTNlvl1{20, 20, 50, 50, 10, 10, "1", ILI9341_WHITE, ILI9341_BLACK, []()->void{
    // start level 1
}};
buttons BTNlvl2{80, 20, 50, 50, 10, 10, "2", ILI9341_WHITE, ILI9341_BLACK, []()->void{
    // start level 2
}};
buttons BTNlvl3{140, 20 ,50, 50, 10, 10, "3", ILI9341_WHITE, ILI9341_BLACK, []()->void{
    // start level 3
}};

// buttons in appropriate array
buttons loadingScreenButtons[] = {BTNpressTooStart};
buttons menuScreenButtons[] = {BTNreturn, BTNselectLevels, BTNseeHighscores};
buttons levelScreenButtons[] = {BTNreturn, BTNlvl1, BTNlvl2, BTNlvl3};

// sizeof array
uint8_t loadingScreenButtonsSize = sizeof(loadingScreenButtons) / sizeof(*loadingScreenButtons);
uint8_t menuScreenButtonsSize = sizeof(menuScreenButtons) / sizeof(*menuScreenButtons);
uint8_t levelScreenButtonSize = sizeof(levelScreenButtons) / sizeof(*levelScreenButtons);

uint8_t activeButton;

void drawButton(Adafruit_ILI9341 &tft, buttons b)
{
    tft.fillRect(b.x, b.y, b.width, b.height, b.backgroundColor);
    tft.setCursor(b.x + b.textOffsetX, b.y + b.textOffsetY);
    tft.setTextColor(b.textColor);
    tft.print(b.text);
}

void drawButtons(Adafruit_ILI9341 &tft, buttons* bList, uint8_t bSize)
{
    for (uint8_t i = 0; i < bSize; i++)
        if (activeButton % bSize == i)
        {
            buttons b = (buttons) bList[i];
            b.backgroundColor = ACTIVE_BUTTON_COLOR;
            drawButton(tft, b);
        } 
        else
        {
            drawButton(tft, (buttons) bList[i]);
        }
}

void globalInit()
{
    activeButton = 1;
}

void initLoadingScreen(Adafruit_ILI9341 &tft)
{
    globalInit();
    tft.fillScreen(ILI9341_DARKCYAN);
    drawButtons(tft, loadingScreenButtons, loadingScreenButtonsSize);
}

void initMenuScreen(Adafruit_ILI9341 &tft)
{
    globalInit();
    tft.fillScreen(ILI9341_CYAN);
    drawButtons(tft, menuScreenButtons, menuScreenButtonsSize);
}

void initLevelScreen(Adafruit_ILI9341 &tft)
{
    globalInit();
    tft.fillScreen(ILI9341_GREEN);
    drawButtons(tft, levelScreenButtons, levelScreenButtonSize);
}

void handleLoadingScreen(Adafruit_ILI9341 &tft, actions action)
{
    switch (action)
    {
        case SELECTBUTTON:
            loadingScreenButtons[activeButton%loadingScreenButtonsSize].action();
            break;
        case NEXTBUTTON:
            activeButton++;
            drawButtons(tft, loadingScreenButtons, loadingScreenButtonsSize);
            break;
    }
}

void handleMenuScreen(Adafruit_ILI9341 &tft, actions action)
{
    switch (action)
    {
        case SELECTBUTTON:
            menuScreenButtons[activeButton%menuScreenButtonsSize].action();
            break;
        case NEXTBUTTON:
            activeButton++;
            drawButtons(tft, menuScreenButtons, menuScreenButtonsSize);
            break;
    }
}

void handleLevelScreen(Adafruit_ILI9341 &tft, actions action)
{
    switch (action)
    {
        case SELECTBUTTON:
            levelScreenButtons[activeButton%levelScreenButtonSize].action();
            break;
        case NEXTBUTTON:
            activeButton++;
            drawButtons(tft, levelScreenButtons, levelScreenButtonSize);
            break;
    }
}


