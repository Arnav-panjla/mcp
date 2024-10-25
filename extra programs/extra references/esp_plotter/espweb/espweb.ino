#include <ESP32Servo.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

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
float temperature;


///////////////////////////////////////website code/////////////////////////////////
AsyncWebServer server(80);
AsyncEventSource events("/events");

unsigned long lastTime = 0;  
unsigned long timerDelay = 500;  // send readings timer

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #4B1D3F; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .card.temperature { color: #0e7c7b; }
    .card.humidity { color: #17bebb; }
    .card.pressure { color: #3fca6b; }
    .card.gas { color: #d62246; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>AUOTOMATED SUN TRACKING SOLAR PANEL</h3>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card temperature">
        <h4><i style='font-size:24px' class='fas'>&#xf5ba;</i> POWER</h4><p><span class="reading"><span id="temp">%POWER%</span> mW</span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";

String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temperature);
  }
}

////////////////////////////////////////////////////////////////////////////////////

void setup() {
    Serial.begin(115200); //begin serial monitor 
    //initialising all input output pins
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

    //turing all led off 
    digitalWrite(red,LOW);
    digitalWrite(green,LOW);

    //////////////////////////////////website code ////////////////////////////////////////////////////
      // Set the device as a Station and Soft Access Point simultaneously
    WiFi.mode(WIFI_AP_STA);
  
    // Set device as a Wi-Fi Station
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(red,HIGH);//turning on red led
      delay(1000);
      Serial.println("Setting as a Wi-Fi Station..");

    }
    Serial.print("Station IP Address: ");
    digitalWrite(red,LOW);
    digitalWrite(green,HIGH);
    Serial.println(WiFi.localIP());
    Serial.println();

    // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();

    //////////////////////////////////////////////////////////////////////////////////////////


    delay(10000);
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
  if ( TRRread > TLRread ){
    if (pos_a>0){
      //Serial.println("left");
      pos_a=pos_a-1;
      servoA.write(pos_a);
    }
  }
  if ( TRRread < TLRread ){
    if(pos_a<180){
      //Serial.println("Right");
      pos_a=pos_a+1;
      servoA.write(pos_a);
    }
   }
  if ( BRRread > TRRread ){
    if (pos_b<pos_b_max){
      //Serial.print("up");
      pos_b=pos_b+1;
      servoB.write(pos_b);
    }
    else{}
  }
  if ( BRRread < TRRread ){
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
  temperature = Pval*1000;
  //Serial.println(Readval);
  //Serial.println(Vval);
  delay(100);
  ///website code to display solar output
  //////////////////////////////////////////////website code/////////////////////////////////////////////
  if ((millis() - lastTime) > timerDelay) {
    //Serial.printf("Power = %.4f \n", temperature);
    //Serial.println();
   // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    events.send(String(temperature).c_str(),"temperature",millis());
   
    lastTime = millis();
  }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    
  
}
