#include <ESP32Servo.h>

Servo servoA;  // create servo object to control a servo
Servo servoB;  // create servo object to control a servo
                // 16 servo objects can be created on the ESP32

int posB = 10;    // variable to store the servoB initial position
int posA = 0;     // variable to store the servoB initial position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoApin = 33;
int servoBpin = 32;

void setup() {
  Serial.begin(115200);
  myservo.attach(servoApin);   // attaches the servo on pin 33 to the servo object
  myservo.attach(servoApin);   // attaches the servo on pin 32 to the servo object

}

void loop() {
  for (posB = 13; posB <= 75; posB += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servoB.write(posB);              // tell servo to go to position in variable 'posB'
    Serial.print("servo B position-")
    Serial.println(posB);
    delay(100);                       // waits 15ms for the servo to reach the position
  }
                       // waits 15ms for the servo to reach the position
}