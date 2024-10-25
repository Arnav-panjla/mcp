#include <ESP32Servo.h>

Servo servoA;   // create servo object to control a servo
Servo servoB;   // create servo object to control a servo
                // 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoApin = 33;
int servoBpin = 32;

void setup() {
  myservo[1].attach(servoApin);
  myservo[2].attach(servoBpin);   
}

void loop() {
  //setting position of all servoes to 90
  servoA.write(90);
  servoB.write(90);
    

}
