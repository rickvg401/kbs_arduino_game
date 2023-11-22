#include <Wire.h>
#include <Arduino.h>
#include <Nunchuk.h>
#define BAUDRATE 9600
#define nunchuk_ID 0xA4 >> 1
unsigned char buffer[4];// array to store arduino output

int main(){

    sei();
    Serial.begin(BAUDRATE);
    Wire.begin();
if(!Nunchuk.begin(nunchuk_ID)){
    Serial.println("******** No nunchuk found");
	Serial.flush();
	return(1);
}
    Serial.print("-------- Nunchuk with Id: ");
	Serial.println(Nunchuk.id);
    while(1){
        if(!Nunchuk.getState(nunchuk_ID)){
            return (1);
        } 
        int x = Nunchuk.state.joy_x_axis;
	    int y = Nunchuk.state.joy_y_axis;
        int c = Nunchuk.state.c_button;
        int z = Nunchuk.state.z_button;
        const char* uitkomst[4];
        if(x != 128){
            uitkomst[0] = x<128 ? "links-" : "rechts-";} else{
            uitkomst[0] = "midden-";
    }
    if(y != 128){
            uitkomst[1] = y>128 ? "boven-" : "onder-";} else{
            uitkomst[1] = "midden-";
    }
    uitkomst[2] = c ? "cin-" : "cout-";
    uitkomst[3] = z ? "zin" : "zout";
    for(int i = 0;i<4;i++){
        Serial.print(uitkomst[i]);
    }
    Serial.println(" ");
}
}