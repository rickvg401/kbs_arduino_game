#include <avr/interrupt.h>
#include <HardwareSerial.h>
#include <Arduino.h>
#include <IRremote.h>


volatile bool flag = false;

void sendNEC(uint8_t data) {
  uint8_t data1 = data;

    for (uint8_t i = 0; i < 8; i++)
    {
      

      if (data1 & (0b00000001))
      {
        // Verzend een logische '1'
        TCCR1A |= (1 << COM1A0); // Zet de pin op HIGH (mark)
        // digitalWrite(9, HIGH);
        PORTB |= (1 << PB1);
        // delayMicroseconds(500);

        _delay_us(500);
      }
      else
      {
        // Verzend een logische '0'
        TCCR1A &= ~(1 << COM1A0); // Zet de pin op LOW (space)
        // digitalWrite(9,LOW);
        PORTB &= ~(1 << PB1);
        // delayMicroseconds(1500); // Bijvoorbeeld: houd de bit voor 500 µs aan (kan aangepast worden)

        _delay_us(500);
      }
      data1 = data1 >> 1;
      if (flag)
      {
        Serial.print("1");
      }
      else
      {
        Serial.print("0");
      }
    }
    Serial.println(" einde ");

  
}

ISR(INT0_vect)
{
  uint8_t pinstatus = (PIND & (1<<PD2));
 if(pinstatus)
 {

    flag = true;
    // counter opslaan 
 } else {

    flag = false;
 }
}

ISR(TIMER1_COMPA_vect)
{
  // counter altijd ophogen 
  // 
}

int main(void)
{
    Serial.begin(9600);

    DDRB |= (1<<DDB1); // ir led
    
    OCR1A = 208; // compare match for 38KHZ increments
    TCCR1B |= (1<<WGM12); // ctc mode
    TCCR1B |= (1<<CS10); // no prescaler
    TIMSK1 |= (1<<OCIE1A); // enable timer compare interrupt

    TCCR1A &= ~(1<<COM1A0); // disable  -> toggle OC1A pin 9 on compare match

    // external interrupt

    EIMSK |= (1<<INT0); // int0 external interrupt aan
    EICRA |= (1<<ISC00); // any logical change generate interrupt

    sei(); // enable global interrupts

    while(1)
    {
        uint8_t data = 0b00001101;
        sendNEC(data); // Voorbeeld: Verstuur een testsignaal met waarde 0x00FF  
        _delay_ms(1000);
        // delay(10); //
        // sendNEC(1);
        // delay(10);
        

    }
    return 0;
}
