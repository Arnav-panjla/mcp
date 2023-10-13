#include <ESP32Servo.h>

Servo myservo[2];  // create servo object to control a servo (probably an array to control 2 servos) 
                // 16 servo objects can be created on the ESP32

int pos_a = 90;    // variable to store the inital Aservo position
int pos_b = 90;    // variable to store the inital Bservo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int AservoPin = 27;
int BservoPin = 26;
int TRR = ;
int TLR = ;
int BRR = ;
int BLR = ;
//tolerance factor for each photoresistor (Default value is 1)
int TRRtf = 1;
int TLRtf = 1;
int BRRtf = 1;
int BLRtf = 1;

void setup() {
    Serial.begin(115200); //begin serial monitor 
    //initialising all input output pins
    pinMode(AservoPin,OUTPUT);
    pinMode(BservoPin,OUTPUT);
    pinMode(TRR,INPUT);
    pinMode(TLR,INPUT);
    pinMode(BRR,INPUT);
    pinMode(BLR,INPUT);
    myservo[1].attach(AservoPin);
    myservo[2].attach(BservoPin);   

    //setting position of all servoes to 90 initially 
    myservo[1].write(pos_a);
    myservo[2].write(pos_b);

    delay(1)
}

void loop() {
    //reading photoresistor values 
    TRRread = analogRead(TRR);
    TLRread = analogRead(TLR);
    BRRread = analogRead(BRR);
    TLRread = analogRead(BLR);

    //calibrating read values
    TRRval = TRRread*TRRtf;
    TLRval = TLRread*TLRtf;
    BRRval = BRRread*BRRtf;
    BLRval = BLRread*BLRtf;

    If ( TRRval > TLRval ) or ( BRRval > BLRval ){
        pos_a=pos_a+1;
        myservo[1].write(pos_a);
    }
    Elif ( TRRval < TLRval ) or ( BRRval < BLRval ){
        pos_a=pos_a-1;
        myservo[1].write(pos_a);
    }
    Elif ( BRRval > TRRval ) or ( BLRval > TLRval ){
        pos_a=pos_a+1;
        myservo[2].write(pos_a);
    }
    Elif ( BRRval < TRRval ) or ( BLRval < TLRval ){
        pos_a=pos_a-1;
        myservo[2].write(pos_a);
    }

    Else{}
  
}