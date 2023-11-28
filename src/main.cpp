#include <avr/interrupt.h>
#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#include "SPI.h"
#include "display/Adafruit-GFX/Adafruit_GFX.h"
#include "display/Adafruit_ILI9341.h"
#include <util/delay.h>


//serial
#define SerialActive //if defined serial is active
#define BAUDRATE 9600

//nunchuk
#define nunchuk_ID 0xA4 >> 1
// unsigned char buffer[4];// array to store arduino output
uint8_t NunChuckPosition[4];
bool NunChuckPositionDivided = false;

/*display*/
#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//game
uint8_t playerPosX;
uint8_t playerPosY;


// 7 segment display
#define PCF8574_i2cAdr 0x21
void PCF8574_write(byte bytebuffer);
byte PCF8574_read();
byte bits;

// boolean voor het printen straks niet meer nodig
volatile bool flag = false;

// infrared vars
volatile uint16_t prevCounterValue = 0;
volatile uint16_t currentCounterValue;
volatile uint16_t pulseDuration;
volatile uint8_t eenofnull;
volatile uint16_t counter = 0;

bool setupDisplay();
bool setupNunchuck();
void drawPlayer(uint8_t x, uint8_t y);
void drawPath(uint8_t x, uint8_t y);
void movePlayer(uint8_t newX,uint8_t newY);
void drawLevel();
void getNunchukPosition();
void PCF8574_write(byte bytebuffer);


void sendNEC(uint8_t data) {
  uint8_t data1 = data;

    // for (uint8_t i = 0; i < 8; i++)
    // {

      // if (data1 & (0x01)) voor bytes
      if(data1 == 1)
      {
        // Verzend een logische '1'
        TCCR0A |= (1<< COM0A0);
        PORTD |= (1<< PD6);
        _delay_us(1700); // houd dit bit voor 1700 us aan // BUSY WAITING WORDT VERANDERT IN SPRINT 2

      }
      else
      {
        // Verzend een logische '0'
        TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
        PORTD |= (1<< PD6); // pin naar high zetten
        _delay_us(700); // houd dit bit voor 700 us aan // BUSY WAITING WORDT VERANDERT IN SPRINT 2

      }
      // data1 = data1 >> 1; // voor bytes
      // Verzend een logische '0'

        TCCR0A &= ~(1<< COM0A0);
        PORTD &= ~(1<<PD6);
    // }
    Serial.println(" einde ");
  
}

ISR(INT0_vect)
{
  uint8_t pinstatus = (PIND & (1<<PD2));
  
 if(pinstatus)
 {

    
    // opgaande flank bepaal je het verschil tussen huidige counterstand en de onthouden counterstand
    // Je hebt dan gemeten hoe lang de puls duurde. Daarmee kan je
    // bepalen of het uitgezonden bit een 0 of een 1 was. Schuif dit
    // bit in het resulterende buffer.
     currentCounterValue = counter;
     pulseDuration = currentCounterValue - prevCounterValue;
    if(pulseDuration > 169 && pulseDuration < 178) {
      // Voeg '1' toe aan buffer
      flag = true;
      eenofnull = 1;
    } else if (pulseDuration < 76 && pulseDuration > 70) {
      // Voeg '0' toe aan buffer
      flag = false;
      eenofnull = 0;
    }

 } else {
    
    
    // bij een neergaande flank onthouden counter van timer 1
    prevCounterValue = counter;
    eenofnull = -1;
 }
}

ISR(TIMER0_COMPA_vect)
{
  // counter altijd ophogen 
  // counter hier altijd ophogen 
  counter++;
  if(counter > 100000){
    counter = 0;
  }
}

void initIR()
{
    DDRD |= (1<<DDD6); // ir led op gameshield

    // TCCR1A &= ~(1<<COM1A0); // disable  -> toggle OC1A pin 9 on compare match

    // timer 0 voor irled
    OCR0A = 208;
    TCCR0A |= (1<<WGM01);
    TCCR0B |= (1<<CS00); // no prescaler
    TIMSK0 |= (1<<OCIE0A); // enable timer compare interrupt

    TCCR0A &= ~(1<<COM0A0);// toggle OC0A pin 6

    // external interrupt
    EIMSK |= (1<<INT0); // int0 external interrupt aan
    EICRA |= (1<<ISC00); // any logical change generate interrupt

}


bool setupDisplay(){
    //returned if setup is correctly completed
    tft.begin();
    // tft.setRotation(1);
    return true;
}

bool setupNunchuck(){
    if(!Nunchuk.begin(nunchuk_ID)){
        #ifdef SerialActive
        Serial.println("******** No nunchuk found");
        Serial.flush();
        #endif
        return(false);
    }
    #ifdef SerialActive
    Serial.print("-------- Nunchuk with Id: ");
	Serial.println(Nunchuk.id);
    #endif
    return true;
}



void drawPlayer(uint8_t x, uint8_t y){
    tft.fillRect(x,y,25,25,ILI9341_DARKCYAN);
    playerPosX = x;
    playerPosY = y;
}

void drawPath(uint8_t x, uint8_t y){
    tft.fillRect(x,y,25,25,ILI9341_BLACK);
}

void movePlayer(uint8_t newX,uint8_t newY){
    drawPath(playerPosX,playerPosY);
    drawPlayer(newX,newY);
}


void movePlayerNunchuk(){
    drawPath(playerPosX,playerPosY);
    
    
    uint8_t newX = playerPosX + ((NunChuckPosition[0]-128)/100*1);
    uint8_t newY = playerPosY + ((NunChuckPosition[1]-128)/100*1);

    drawPlayer(newX,newY);
}

void drawLevel(){
    // for(int i=0;i<100;i+=25){
    //     drawPath(i,200);
    // }
}


void getNunchukPosition(){
    if(!Nunchuk.getState(nunchuk_ID)){
        return;
    } 
    
    // uint8_t x = Nunchuk.state.joy_x_axis;
    // uint8_t y = Nunchuk.state.joy_y_axis;
    //flipped
    uint8_t x = Nunchuk.state.joy_y_axis;
    uint8_t y = Nunchuk.state.joy_x_axis;
    
    

    uint8_t c = Nunchuk.state.c_button;
    uint8_t z = Nunchuk.state.z_button;
    
    

    if(NunChuckPositionDivided){
        if(x != 128){
            NunChuckPosition[0] = x<128 ? 50 : 200;}
        else{
            NunChuckPosition[0] = 128;
        }
        if(y != 128){
                NunChuckPosition[1] = y>128 ? 200 : 50;} 
        else{
                NunChuckPosition[1] = 128;
        }
        NunChuckPosition[2] = c ? 0 : 1;
        NunChuckPosition[3] = z ? 0 : 1;


    }else{
        NunChuckPosition[0] = x;
        NunChuckPosition[1] = y;
        NunChuckPosition[2] = c;
        NunChuckPosition[3] = z;
    }

    
    
}


void PCF8574_write(byte bytebuffer)
{
  Wire.beginTransmission(PCF8574_i2cAdr);
  Wire.write(bytebuffer);
  Wire.endTransmission();
}


int main(void)
{
    sei();
    #ifdef SerialActive
    Serial.begin(BAUDRATE);
    #endif
    Wire.begin();

    if(!setupNunchuck()){return 0;}
    if(!setupDisplay()){return 0;}


    tft.fillScreen(ILI9341_RED);
    // drawLevel();
    drawPlayer(128,128);
    while(1)
    {
        // uint8_t data = 0b00001101;
        // sendNEC(data); // Voorbeeld: Verstuur een testsignaal met waarde 0x00FF  
        
      // nunchuck en display
        getNunchukPosition();
        if(!NunChuckPosition[2])
        {
          sendNEC(1);
        } else if( !NunChuckPosition[3]) {
          sendNEC(0);
        }
        if(eenofnull == 1)
        {
          PCF8574_write(0b11111111);
        } else if(eenofnull == 0) {
          PCF8574_write((0b00000000));
        } 

        Serial.println(pulseDuration);
        // movePlayer(NunChuckPosition[1],NunChuckPosition[0]);
        movePlayerNunchuk();
        
    } 
    return 0;
}
