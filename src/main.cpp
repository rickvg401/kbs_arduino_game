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
uint16_t playerPosX;
uint16_t playerPosY;


// 7 segment display
#define PCF8574_i2cAdr 0x21
void PCF8574_write(byte bytebuffer);
byte PCF8574_read();
byte bits;


// infrared vars
volatile uint16_t prevCounterValue = 0;
volatile uint16_t currentCounterValue = 0;
volatile uint16_t pulseDuration = 0;
volatile uint8_t eenofnull = 2;
volatile uint16_t counter = 0;

// ir 
volatile bool logicalone = false;
volatile bool logicalzero = false;
volatile uint16_t counterone = 0;

volatile bool end = false;

volatile uint16_t prevcounteronevalue = 0;
volatile uint16_t prevcounterzerovalue = 0;
volatile bool resetone = false;
volatile bool resetzero = false;
volatile uint16_t currentcounterone = 0;
volatile uint16_t currentcounterzero = 0;


bool setupDisplay();
bool setupNunchuck();
void drawPlayer(uint16_t x, uint16_t y);
void drawPath(uint16_t x, uint16_t y);
void movePlayer(uint16_t newX,uint16_t newY);
void movePlayerNunchuk();
void drawLevel();
void getNunchukPosition();
void PCF8574_write(byte bytebuffer);
void addToBuffer();
uint8_t clearbuffer();

void sendNEC(uint8_t data) {
 
      if(data == 1 ) // && !logicalzero && !logicalone
      {
        // Verzend een logische '1'
        // begin counter van 0 en tot die tijd zet infrared aan 
        counter = 0;
        logicalone = true;
        TCCR0A |= (1<< COM0A0); // toggle pin met 38Khz
      }
      else if (data == 0 ) 
      {
        // Verzend een logische '0'
        // begin counter van 0 en tot die tijd zet infrared aan
        counter = 0;
        logicalzero = true;
        TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
      }
      while(logicalone || logicalzero)
      {
            
      }

}

void sendEnd()
{
    TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
    PORTD |= (1<< PD6); // pin naar high zetten
    _delay_us(3000); 
    TCCR0A &= ~(1<< COM0A0);
    PORTD &= ~(1<<PD6);
}

void sendBegin()
{
    TCCR0A |= (1<<COM0A0); // toggle de pin met 38KHZ
    PORTD |= (1<< PD6); // pin naar high zetten
    _delay_us(5500); 
    TCCR0A &= ~(1<< COM0A0);
    PORTD &= ~(1<<PD6);
}

const uint8_t bufferSize = 8; // Grootte van de buffer in bytes
uint8_t buffer[bufferSize]; // Array om de bits op te slaan
uint8_t bufferIndex = 0; // Huidige index in de buffer

void addToBuffer() {
  if (bufferIndex < 8) { // Controleer of de buffer niet vol is
   
    
    // Voeg het bit toe aan het juiste byte in de buffer
    if (eenofnull == 1) {
      buffer[bufferIndex] = 1; // Zet het bit op 1
      Serial.println("adding 1 to buffer");
    } else if(eenofnull == 0){
      buffer[bufferIndex] = 0; // Zet het bit op 0
      Serial.println("adding 0 to buffer");

    }
    
    bufferIndex++; // Verhoog de bufferindex voor het volgende bit
  } else {
    
    // Serial.println("buffer full");
    clearbuffer();
  }
}

uint8_t clearbuffer()
{
  uint8_t data = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    data |= buffer[i] << i;
  }
  uint8_t buffer[bufferSize]; 
  return data;
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
    if(pulseDuration > 160 && pulseDuration < 190) {
      // Voeg '1' toe aan buffer
      eenofnull = 1;  
      
    } else if (pulseDuration < 85 && pulseDuration > 70) {
      // Voeg '0' toe aan buffer
      eenofnull = 0;

    } else if (pulseDuration < 280 && pulseDuration > 350)
    {
      end = true;
      
    } else if (pulseDuration < 545 && pulseDuration > 560)
    {
      end = false;

    }

 } else {
    
    // bij een neergaande flank onthouden counter van timer 1
    prevCounterValue = counter;
    eenofnull = 2;
 }
}

ISR(TIMER0_COMPA_vect)
{
  // counter hier altijd ophogen 
  counter++;

  if (logicalone == true)
  {
    currentcounterone = counter;
    if(currentcounterone > 200)
    {
      logicalone = false;
    }
    if ((currentcounterone ) >= (175)) // 175 // - prevcounteronevalue
    {
      prevcounteronevalue = currentcounterone;
      
      TCCR0A &= ~(1 << COM0A0);
      // PORTD &= ~(1 << PD6);
      

    }
  }

  if (logicalzero == true)
  {
    currentcounterzero = counter;
    if(currentcounterzero > 100)
    {
      logicalzero = false;
    }
    if ((currentcounterzero ) >= (75)) // 75 // - prevcounterzerovalue
    {
      prevcounterzerovalue = currentcounterzero;

      TCCR0A &= ~(1 << COM0A0);
      // PORTD &= ~(1 << PD6);
     
      
    }
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
    EIFR |= (1 << INTF0);

}


bool setupDisplay(){
    //returned if setup is correctly completed
    tft.begin();
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



void drawPlayer(uint16_t x, uint16_t y){
    tft.fillRect(x,y,25,25,ILI9341_DARKCYAN);
    playerPosX = x;
    playerPosY = y;
}

void drawPath(uint16_t x, uint16_t y){
    tft.fillRect(x,y,25,25,ILI9341_BLACK);
}

void movePlayer(uint16_t newX,uint16_t newY){
    drawPath(playerPosX,playerPosY);
    drawPlayer(newX,newY);
}


void movePlayerNunchuk(){
    drawPath(playerPosX,playerPosY);
    
    
    uint16_t newX = playerPosX + ((NunChuckPosition[0]-128)/100*1);
    uint16_t newY = playerPosY + ((NunChuckPosition[1]-128)/100*1);

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
                NunChuckPosition[1] = y>128 ? 50 : 200;} 
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
    initIR();
    #ifdef SerialActive
    Serial.begin(BAUDRATE);
    #endif
    Wire.begin();

    if(!setupNunchuck()){return 0;}
    if(!setupDisplay()){return 0;}


    tft.fillScreen(ILI9341_RED);
    // drawLevel();
    drawPlayer(128,128);
    // drawPath(100,128);

    while(1)
    {
        // uint8_t data = 0b00001101;
        // sendNEC(data); // Voorbeeld: Verstuur een testsignaal met waarde 0x00FF  
        // sendnec in een for loop 8 keer aanroepen !!!!
      // nunchuck en display
        getNunchukPosition();
        // addToBuffer();
        // sendNEC(2);
        // if(!end)
        // {
        //   bufferIndex = 0;
        //   for (uint8_t i = 0; i < 8; i++)
        //   {
        //     buffer[i] = 0;
        //   }
        // }
        
        if(NunChuckPosition[2])
        {
          // sendNEC(0);
          // sendBegin();

          uint8_t bittosend = 0b00101110;
          for (uint8_t i = 0; i < 8 ; i++)
          {
            uint8_t lsb = bittosend & 0x01;
            sendNEC(lsb);
            bittosend = bittosend >> 1;
            Serial.println(pulseDuration);
          }
          Serial.println("einde");
         
          // for (int i = 0; i < 8; i++)
          // {
          //   Serial.print(buffer[i]);
          // }
          // Serial.println("\n");

        } 
        else if(NunChuckPosition[3]) {
          sendNEC(0);
          Serial.println(pulseDuration);

        }
       

        if(eenofnull == 1)
        {
          PCF8574_write(0b11111111);
        } else if(eenofnull == 0) {
          PCF8574_write((0b00000000));
        }

        // Serial.println(pulseDuration);
        // movePlayer(NunChuckPosition[1],NunChuckPosition[0]);
        movePlayerNunchuk();
        
    } 
    return 0;
}
