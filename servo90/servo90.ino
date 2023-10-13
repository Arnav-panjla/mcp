#include <ESP32Servo.h>

Servo myservo[2];  // create servo object to control a servo (probably an array to control 2 servos) 
                // 16 servo objects can be created on the ESP32

int pos = 90;    // variable to store the inital servo position
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
    myservo[1].write(pos);
    myservo[2].write(pos);

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

    If ()
  
}
