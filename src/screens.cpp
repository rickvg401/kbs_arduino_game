#include <screens.h>
#include <control.h>
#include <score.h>

screens nScreen = LOADING_SCREEN;
screens cScreen = NO_SCREEN;
uint32_t touchX = 0;
uint32_t touchY = 0;


// general buttons
buttons BTNreturn{0, 0, 40, 40, 5, 11, "<-", 2, ILI9341_WHITE, ILI9341_RED, []()-> void 
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
buttons BTNpressTooStart{60, 200, 200, 40, 15, 5, "press to start", 2, ILI9341_BLACK, ILI9341_BLUE, []() -> void 
{
    nScreen = MENU_SCREEN;
}};

// menu screen buttons
buttons BTNselectLevels{20, 60, 180, 40, 5, 10, "Select levels", 2, ILI9341_WHITE, ILI9341_BLACK, []() -> void
{
    nScreen = LEVEL_SCREEN;
}};

buttons BTNseeHighscores{20, 120, 180, 40, 5, 10, "See highscores", 2, ILI9341_WHITE, ILI9341_BLACK, []() -> void
{
    nScreen = HIGHSCORE_SCREEN;
}};

// level screen buttons
buttons BTNlvl1{20, 60, 50, 50, 10, 10, "1", 2, ILI9341_WHITE, ILI9341_BLACK, []()->void{
    // start level 1
    selectLevel(0);
    switchControlState(_GAME);
}};
buttons BTNlvl2{80, 60, 50, 50, 10, 10, "2", 2, ILI9341_WHITE, ILI9341_BLACK, []()->void{
    // start level 2
    selectLevel(1);
    switchControlState(_GAME);
}};

// buttons in appropriate array
buttons loadingScreenButtons[] = {BTNpressTooStart};
buttons menuScreenButtons[] = {BTNreturn, BTNselectLevels, BTNseeHighscores};
buttons levelScreenButtons[] = {BTNreturn, BTNlvl1, BTNlvl2};
buttons scoreScreenButtons[] = {BTNreturn};

// sizeof array
uint8_t loadingScreenButtonsSize = sizeof(loadingScreenButtons) / sizeof(*loadingScreenButtons);
uint8_t menuScreenButtonsSize = sizeof(menuScreenButtons) / sizeof(*menuScreenButtons);
uint8_t levelScreenButtonSize = sizeof(levelScreenButtons) / sizeof(*levelScreenButtons);
uint8_t scoreScreenButtonsSize = sizeof(scoreScreenButtons) / sizeof(*scoreScreenButtons);

uint8_t activeButton;

bool inBound(buttons b)
{
    if (b.x < touchX && touchX < b.x + b.width && b.y < touchY && touchY < b.y + b.height)
        return true;
    return false;
}

void drawButton(Adafruit_ILI9341 &tft, buttons b)
{
    tft.setTextSize((uint8_t) b.textSize);
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

void globalInit(Adafruit_ILI9341 &tft)
{
    tft.setTextSize(2);
    activeButton = 1;
}

void initLoadingScreen(Adafruit_ILI9341 &tft)
{
    globalInit(tft);
    tft.fillScreen(ILI9341_DARKCYAN);
    tft.setTextSize(4);
    tft.setCursor(10,10);
    tft.print("PACMAN");
    tft.setCursor(10,45);
    tft.setTextSize(1);
    tft.print("Made by ICTMCB group 3.");
    tft.setCursor(10,55);
    tft.print("By order of TamTam.");
    tft.setTextSize(2);
    drawButtons(tft, loadingScreenButtons, loadingScreenButtonsSize);
}

void initScoreScreen(Adafruit_ILI9341 &tft)
{
    globalInit(tft);
    HighScorePage();
    drawButtons(tft, scoreScreenButtons, scoreScreenButtonsSize);
}

void initMenuScreen(Adafruit_ILI9341 &tft)
{
    globalInit(tft);
    tft.fillScreen(ILI9341_CYAN);
    tft.setTextSize(1);
    drawButtons(tft, menuScreenButtons, menuScreenButtonsSize);
}

void initLevelScreen(Adafruit_ILI9341 &tft)
{
    globalInit(tft);
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
        case TOUCH:
            for (size_t i = 0; i < loadingScreenButtonsSize; i++)
            {
                if (inBound((buttons) loadingScreenButtons[i]))
                {
                    ((buttons) loadingScreenButtons[i]).action();
                }
            }
            
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
        case TOUCH:
            for (size_t i = 0; i < menuScreenButtonsSize; i++)
            {
                if (inBound((buttons) menuScreenButtons[i]))
                {
                    if (i == activeButton)
                        ((buttons) menuScreenButtons[i]).action();
                    else
                    {
                        activeButton = i;
                        drawButtons(tft, menuScreenButtons, menuScreenButtonsSize);
                    }
                }
            }
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
        case TOUCH:
            for (size_t i = 0; i < levelScreenButtonSize; i++)
            {
                if (inBound((buttons) levelScreenButtons[i]))
                {
                    if (i == activeButton)
                        ((buttons) levelScreenButtons[i]).action();
                    else
                    {
                        activeButton = i;
                        drawButtons(tft, levelScreenButtons, levelScreenButtonSize);
                    }
                }
            }
    }
}

void handleScoreScreen(Adafruit_ILI9341 &tft, actions action)
{
    switch (action)
    {
        case SELECTBUTTON:
            scoreScreenButtons[activeButton%scoreScreenButtonsSize].action();
            break;
        case NEXTBUTTON:
            activeButton++;
            drawButtons(tft, scoreScreenButtons, scoreScreenButtonsSize);
            break;
        case TOUCH:
            for (size_t i = 0; i < scoreScreenButtonsSize; i++)
            {
                if (inBound((buttons) scoreScreenButtons[i]))
                {
                    if (i == activeButton)
                        ((buttons) scoreScreenButtons[i]).action();
                    else
                    {
                        activeButton = i;
                        drawButtons(tft, scoreScreenButtons, scoreScreenButtonsSize);
                    }
                }
            }
            break;
    } 
}