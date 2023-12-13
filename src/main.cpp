#include <EEPROM.h>
#include <Arduino.h>
#include "display/Adafruit-GFX/Adafruit_GFX.h"
#include "display/Adafruit_ILI9341.h"
#include <Fonts/PressStart2P_vaV76pt7b.h>

#define TFT_DC 9 
#define TFT_CS 10
#define BACKGROUND ILI9341_BLACK //Background color
#define HIGHSCORECOLOR ILI9341_YELLOW //color of highscore text
#define PLAYERNAME "Jurre" //player name for adding highscores

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS,TFT_DC);

char nameList [10][6] = {}; //list to store playernames from EEPROM
uint32_t scoreList [10] = {};//list to store scores from EEPROM
int posList[10] = {0,1,2,3,4,5,6,7,8,9}; //list to keep track of indexes in EEPROM

void HighScorePage();
bool setupHighScore();
void getScore();
void setupScore();
void printHighScore(char names[10][6],uint32_t scores[10]);
void sort(char names[10][6],uint32_t scores[10], int pos[10]);
void addScore(char name[6], uint32_t points);
//function to call when going to highscore page
void HighScorePage(){
  setupHighScore();
  getScore();
  printHighScore(nameList,scoreList);
}
//clears the screen, sets the font and text color 
bool setupHighScore(){
  tft.fillScreen(BACKGROUND);
  tft.setFont(&PressStart2P_vaV76pt7b);
  tft.setTextColor(HIGHSCORECOLOR);
  return 1;
}
//gets the EEPROM data, should only be called once every time the arduino starts up
void getScore(){
  char temp[6];
  uint32_t temp2;
  for(int i = 0;i<10;i++){
    EEPROM.get(i*10,temp);
    EEPROM.get(i*10+6,temp2);
    for(int j = 0;j<6;j++){
      nameList[i][j] = temp[j];
    }
    scoreList[i] = temp2;
  }
  sort(nameList,scoreList,posList);
}
//prints the highscores, uses the scoreList and nameList
void printHighScore(char names[10][6],uint32_t scores[10]){
  tft.fillScreen(BACKGROUND);
  sort(nameList,scoreList,posList);
  tft.setCursor(110,25);
  tft.println("Highscore");
  char bufHS[64];
  char tempN[6];
  long tempS;
  for(int i = 9;i>=0;i--){
    for(int j = 0;j<6;j++){
      tempN[j] = names[i][j];
    }
    tempS = scoreList[i];
    sprintf(bufHS,"%2i %-6s %10lu",10-i,tempN,tempS);
    tft.setCursor(50,200-i*15);
    tft.println(bufHS);
  }
}
//sorts the highscores so that they get called in the correct order, uses insertion sort
void sort(char names[10][6],uint32_t scores[10],int pos[10]){
    char temp1[6] = {};
    uint32_t temp2;
    int temp3;
    int j;
    for(int p = 1;p< 10;p++){
        for(int i = 0;i<6;i++){
            temp1[i] = names[p][i];
        }
        temp2 = scores[p];
        temp3 = posList[p];
    for(j = p;j> 0 && temp2<scores[j-1];j--){
        scores[j] = scores[j-1];
        posList[j] = posList[j-1];
    
        for(int i = 0;i<6;i++){
            names[j][i] = names[j-1][i];
        }
    }
    scores[j] = temp2;
    posList[j] = temp3;
    for(int i = 0;i<6;i++){
            names[j][i] = temp1[i];
        }
    }

  }
//fills EEPROM with mock scores
void setupScore(){
  char names[10][6] = {"henk","kees","klaas","piet","jan","wilem","jurre","andor","rick","jay"};
  uint32_t scores[10] ={1000000,1000,8000,4000,3000,6000,2000,7000,9000,5000};
  for(int i = 0;i<10;i++){
    for(int j = 0;j<6;j++){
    EEPROM.put(i*10+j,names[i][j]);
    }
    EEPROM.put(i*10+6,scores[i]);
  }
}
//adds a new score to the EEPROM, also add it to scoreList and nameList so you dont have to read all the EEPROM data again
void addScore(char name[6], uint32_t points){
  if(points > scoreList[0]){
    scoreList[0] = points;
    for(int j = 0;j<6;j++){
    nameList[0][j] = name[j];
    EEPROM.put(posList[0]*10+j,name[j]);
    }
    EEPROM.put(posList[0]*10+6,points);
  }  
}