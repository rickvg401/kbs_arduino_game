#include <EEPROM.h>
#include <Arduino.h>
#include "main.h"
#include "display/Adafruit-GFX/Adafruit_GFX.h"
#include "display/Adafruit_ILI9341.h"
#include <Fonts/PressStart2P_vaV76pt7b.h>

char name [10][6] = {};
uint32_t score [10] = {};
int pos[10] = {0,1,2,3,4,5,6,7,8,9};
bool setupHighScore();
void getScore();
void setupScore();
void printHighScore(char names[10][6],uint32_t scores[10]);
void sort(char name[10][6],uint32_t score[10], int pos[10]);
void addScore(char naam[6], uint32_t points);
#define TFT_DC 9
#define TFT_CS 10
#define BACKGROUND ILI9341_BLACK
#define TEXTCOLOR ILI9341_YELLOW
#define NAAM "Jurre"
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS,TFT_DC);
bool setupDisplay(){
  tft.begin();
  tft.setRotation(1);
  return true;
}
int main(){
  setupDisplay();
  setupHighScore();
  getScore();
  printHighScore(name,score);
  return 1;
}
bool setupHighScore(){
  tft.fillScreen(BACKGROUND);
  tft.setFont(&PressStart2P_vaV76pt7b);
  tft.setTextColor(TEXTCOLOR);
  return 1;
}
void getScore(){
  char temp[6];
  uint32_t temp2;
  for(int i = 0;i<10;i++){
    EEPROM.get(i*10,temp);
    EEPROM.get(i*10+6,temp2);
    for(int j = 0;j<6;j++){
      name[i][j] = temp[j];
    }
    score[i] = temp2;
  }
  sort(name,score,pos);
}
void printHighScore(char names[10][6],uint32_t scores[10]){
  tft.fillScreen(BACKGROUND);
  sort(name,score,pos);
  tft.setCursor(110,25);
  tft.println("Highscore");
  char bufHS[64];
  char tempN[6];
  long tempS;
  for(int i = 9;i>=0;i--){
    for(int j = 0;j<6;j++){
      tempN[j] = names[i][j];
    }
    tempS = score[i];
    sprintf(bufHS,"%2i %-6s %10lu",10-i,tempN,tempS);
    tft.setCursor(50,200-i*15);
    tft.println(bufHS);
  }
}
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
        temp3 = pos[p];
    for(j = p;j> 0 && temp2<scores[j-1];j--){
        scores[j] = scores[j-1];
        pos[j] = pos[j-1];
    
        for(int i = 0;i<6;i++){
            names[j][i] = names[j-1][i];
        }
    }
    scores[j] = temp2;
    pos[j] = temp3;
    for(int i = 0;i<6;i++){
            names[j][i] = temp1[i];
        }
    }

  }
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
void addScore(char naam[6], uint32_t points){
  if(points > score[0]){
    score[0] = points;
    for(int j = 0;j<6;j++){
    name[0][j] = naam[j];
    EEPROM.put(pos[0]*10+j,naam[j]);
    }
    EEPROM.put(pos[0]*10+6,points);
  }  
}