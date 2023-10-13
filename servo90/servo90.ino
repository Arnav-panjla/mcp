#include <ESP32Servo.h>

Servo myservo[2];  // create servo object to control a servo
                // 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int AservoPin = 27;
int BservoPin = 26;
int TRR = ;
int TLR = ;
int BRR = ;
int BLR = ;

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
    myservo[1].write(90);
    myservo[2].write(90);
}

void loop() {


  
}
