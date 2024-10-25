#include <ESP32Servo.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Replace with your network credentials
const char* ssid = "G42";
const char* password = "244466666";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Servo servoA;
Servo servoB;  // create servo object to control a servo (probably an array to control 2 servos) 
                // 16 servo objects can be created on the ESP32
                
int pos_b_min = 25;
int pos_b_max = 75;
int pos_a = 90;                         // variable to store the inital Aservo position
int pos_b = (pos_b_min+pos_b_max)/2;    // variable to store the inital Bservo position

// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
//That means you can't use the ADC on any of the ADC2 channels while WiFi is on: GPIO4, GPIO0, GPIO2, GPIO15, GPIO13, GPIO12, GPIO14, GPIO27, GPIO25 and GPIO26.
//But you can use ADC1, which uses pins GPIO36(VP), GPIO37, GPIO38, GPIO39(VN), GPIO32, GPIO33, GPIO34 and GPIO35.
int red = 23;
int green =22;
int read_val = 34;
int servoApin = 32;
int servoBpin = 18;
int TRR = 39;//green
int TLR = 35;//yellow
int BRR = 36;//blue
int BLR = 33;//orange
int Rtol = 200;//tolerance value for photo resistor
//tolerance factor for each photoresistor (Default value is 1)
//int TRRtf = 1;
//int TLRtf = 1;
//int BRRtf = 1;
//int BLRtf = 1;
//variables declaration
int TRRread;
int TLRread;
int BRRread;
int BLRread;

//int TRRval;
//int TLRval;
//int BRRval;
//int BLRval;
//
float Cof = (3.3/1230); // using hit and trial 
float Vval; // variable for actual voltage reading from solar output 
float Pval; //variable that contains power value in Watt
int Res = 1000 ; //resistor value (3 identical will be connected) (1k??)
int Readval; //value of analog read from voltage divider (solar output)



void setup() {
    Serial.begin(115200); //begin serial monitor 
    //initialising all input output pins
    pinMode(27,OUTPUT);////////////////////////only this code (backup_2)
    pinMode(read_val,INPUT);
    pinMode(servoApin,OUTPUT);
    pinMode(servoBpin,OUTPUT);
    pinMode(TRR,INPUT);
    pinMode(TLR,INPUT);
    pinMode(BRR,INPUT);
    pinMode(BLR,INPUT);
    pinMode(red,OUTPUT);
    pinMode(green,OUTPUT);
    servoA.attach(servoApin);
    servoB.attach(servoBpin);   

    //setting position of all servoes to 90 initially 
    servoA.write(pos_a);
    servoB.write(pos_b);

    //turing blue on 
    digitalWrite(27,HIGH);
    delay(1000);
    digitalWrite(27,LOW);
    delay(1000);
    digitalWrite(27,HIGH);
}



void loop() {
  
  //reading photoresistor values 
  TRRread = analogRead(TRR);
  TLRread = analogRead(TLR);
  BRRread = analogRead(BRR);
  BLRread = analogRead(BLR);

  //calibrating read values
  //TRRval = TRRread*TRRtf;
  //TLRval = TLRread*TLRtf;
  //BRRval = BRRread*BRRtf;
  //BLRval = BLRread*BLRtf;
  //Serial.println("TRR-");
  //Serial.print(TRRread);
  //Serial.println("BRR-");
  Serial.println(BLRread);
  if ( TRRread > TLRread+Rtol ){
    if (pos_a>0){
      //Serial.println("left");
      pos_a=pos_a-1;
      servoA.write(pos_a);
    }
  }
  if ( TRRread < TLRread-Rtol ){
    if(pos_a<180){
      //Serial.println("Right");
      pos_a=pos_a+1;
      servoA.write(pos_a);
    }
   }
  if ( BRRread > TRRread+Rtol ){
    if (pos_b<pos_b_max){
      //Serial.print("up");
      pos_b=pos_b+1;
      servoB.write(pos_b);
    }
    else{}
  }
  if ( BRRread < TRRread-Rtol ){
    if (pos_b>pos_b_min){
      //Serial.print("Down");
      pos_b=pos_b-1;
      servoB.write(pos_b);
    }
    else{}
  }
  else{}

    ///reads value form solar output
  Readval = analogRead(read_val); //value range from 0-4092
  Vval = Readval*Cof;
  Pval = (Vval*Vval)/Res;
  delay(10);
}
