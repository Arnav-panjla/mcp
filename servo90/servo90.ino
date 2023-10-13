#include <ESP32Servo.h>

Servo myservo[2];  // create servo object to control a servo
                // 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int AservoPin = 27;
int BservoPin = 26;

void setup() {
  myservo[1].attach(AservoPin);
  myservo[2].attach(AservoPin);   
}

void loop() {
    //setting position of all servoes to 90
    myservo[1].write(90);
    myservo[2].write(90);
  
}
