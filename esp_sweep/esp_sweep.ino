#include <ESP32Servo.h>

Servo servoA;  // create servo object to control a servo
Servo servoB;  // create servo object to control a servo
                // 16 servo objects can be created on the ESP32
int posBin = 20;
int posB = 20;    //(10-75) variable to store the servoB initial position
int posA = 0;     //(0-180) variable to store the servoB initial position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoApin = 32;
int servoBpin = 33;

void setup() {
  Serial.begin(115200);
  servoA.attach(servoApin);   // attaches the servo on pin 33 to the servo object
  servoB.attach(servoBpin);   // attaches the servo on pin 32 to the servo object

}

void loop() {
  servoA.write(90);
  for (posB = posBin; posB <= 75; posB += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servoB.write(posB);              // tell servo to go to position in variable 'posB'
    Serial.println("servo B position-");
    Serial.print(posB);
    delay(100);                       // waits 100ms for the servo to reach the position
  }
  for (posB = 75; posB >= posBin; posB -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servoB.write(posB);              // tell servo to go to position in variable 'posB'
    Serial.println("servo B position-");
    Serial.print(posB);
    delay(100);                       // waits 100ms for the servo to reach the position
  }


  for (posA = 0; posA <= 180; posA += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servoA.write(posA);              // tell servo to go to position in variable 'posB'
    Serial.println("servo A position-");
    Serial.print(posA);
    delay(50);                       // waits 15ms for the servo to reach the position
  }
  for (posA = 180; posA >= 0; posA -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servoA.write(posA);              // tell servo to go to position in variable 'posB'
    Serial.println("servo A position-");
    Serial.print(posA);
    delay(50);                       // waits 15ms for the servo to reach the position
  }
}
