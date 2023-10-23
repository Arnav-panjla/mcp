// ---------------------------------------------------------------------------------------
#include <ESP32Servo.h>
#include <WiFi.h>                                     // needed to connect to WiFi
#include <WebServer.h>                                // needed to create a simple webserver (make sure tools -> board is set to ESP32, otherwise you will get a "WebServer.h: No such file or directory" error)
#include <WebSocketsServer.h>                         // needed for instant communication between client and server through Websockets
#include <ArduinoJson.h>                              // needed for JSON encapsulation (send multiple variables with one string)

// SSID and password of Wifi connection:
const char* ssid = "APANJLA";
const char* password = "Asdf@35237";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Servo servoA;  // create servo object to control a servo (probably an array to control 2 servos) 
Servo servoB;                // 16 servo objects can be created on the ESP32
                
int pos_b_min = 25;
int pos_b_max = 75;
int pos_a = 90;                         // variable to store the inital Aservo position
int pos_b = (pos_b_min+pos_b_max)/2;    // variable to store the inital Bservo position

// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
//That means you can't use the ADC on any of the ADC2 channels while WiFi is on: GPIO4, GPIO0, GPIO2, GPIO15, GPIO13, GPIO12, GPIO14, GPIO27, GPIO25 and GPIO26.
//But you can use ADC1, which uses pins GPIO36(VP), GPIO37, GPIO38, GPIO39(VN), GPIO32, GPIO33, GPIO34 and GPIO35.
int red = 23;
int green =22;
int blue = 27;
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
float Cof = (3.3/1230); // co-factor decided using hit and trial 
float Vval; // variable for actual voltage reading from solar output 
float Pval; //variable that contains power value in Watt
float Eval; //energy value Pval * interval
int Res = 1000 ; //resistor value (3 identical will be connected) (1k??)
int Readval; //value of analog read from voltage divider (solar output)

// The String below "webpage" contains the complete HTML code that is sent to the client whenever someone connects to the webserver
String webpage = "<!DOCTYPE HTML><html>\n<head>\n<title>SOLAR TRACKER</title>\n<meta name=viewport content='width=device-width, initial-scale=1'>\n<link rel=stylesheet href=https://use.fontawesome.com/releases/v5.7.2/css/all.css integrity=sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr crossorigin=anonymous>\n<link rel=icon href=data:,>\n<style>html{font-family:Arial;display:inline-block;text-align:center}p{font-size:1.2rem}body{margin:0}.topnav{overflow:hidden;background-color:#4b1d3f;color:white;font-size:1.7rem}.content{padding:20px}.card{background-color:white;box-shadow:2px 2px 12px 1px rgba(140,140,140,.5)}.cards{max-width:700px;margin:0 auto;display:grid;grid-gap:2rem;grid-template-columns:repeat(auto-fit,minmax(300px,1fr))}.reading{font-size:2.8rem}.card.temperature{color:#0e7c7b}.card.humidity{color:#17bebb}.card.pressure{color:#3fca6b}.card.gas{color:#d62246}</style>\n</head>\n<body>\n<div class=topnav>\n<h3>BME680 WEB SERVER</h3>\n</div>\n<div class=content>\n<div class=cards>\n<div class='card temperature'>\n<h4><i class='fas fa-thermometer-half'></i> POWER</h4><p><span class=reading><span id=temp>%POWER%</span> &deg;C</span></p>\n</div>\n<div class='card humidity'>\n<h4><i class='fas fa-tint'></i> ENERGY</h4><p><span class=reading><span id=hum>%ENERGY%</span> &percnt;</span></p>\n</div>\n</div>\n</div>\n<script>var Socket;function init(){Socket=new WebSocket('ws://'+window.location.hostname+':81/');Socket.onmessage=function(a){processCommand(a)}}function processCommand(a){var b=JSON.parse(a.data);document.getElementById('temp').innerHTML=b.rand1;document.getElementById('hum').innerHTML=b.rand2;console.log(b.rand1);console.log(b.rand2)}window.onload=function(a){init()};</script>\n</body>\n</html>";

// The JSON library uses static memory, so this will need to be allocated:
// -> in the video I used global variables for "doc_tx" and "doc_rx", however, I now changed this in the code to local variables instead "doc" -> Arduino documentation recomends to use local containers instead of global to prevent data corruption

// We want to periodically send values to the clients, so we need to define an "interval" and remember the last time we sent data to the client (with "previousMillis")
int interval = 1000;                                  // send data to the client every 1000ms -> 1s
unsigned long previousMillis = 0;                     // we use the "millis()" command for time reference and this will output an unsigned long

// Initialization of webserver and websocket
WebServer server(80);                                 // the server uses port 80 (standard port for websites
WebSocketsServer webSocket = WebSocketsServer(81);    // the websocket uses port 81 (standard port for websockets

void setup() {
  Serial.begin(115200);                               // init serial port for debugging
    
  //initialising all input output pins
  pinMode(blue,OUTPUT);////////////////////////only this code (backup_2)
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

  //setting position of all servoes to default value initially 
  servoA.write(pos_a);
  servoB.write(pos_b);
  //turning all leds off initially
  digitalWrite(blue,LOW);
  digitalWrite(red,LOW);
  digitalWrite(green,LOW);

  WiFi.begin(ssid, password);                         // start WiFi interface
  Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));     // print SSID to the serial interface for debugging
 
  while (WiFi.status() != WL_CONNECTED) {             // wait until WiFi is connected
    delay(1000);
    Serial.print(".");
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);
  }
  Serial.print("Connected to network with IP address: ");
  Serial.print(WiFi.localIP());
  digitalWrite(red,LOW);
  digitalWrite(green,HIGH);   
  Serial.println("");                  // show IP address that the ESP32 has received from router
  
  server.on("/", []() {                               // define here wat the webserver needs to do
    server.send(200, "text/html", webpage);           //    -> it needs to send out the HTML string "webpage" to the client
  });
  server.begin();                                     // start server
  
  webSocket.begin();                                  // start websocket
  //webSocket.onEvent(webSocketEvent);                  // define a callback function -> what does the ESP32 need to do when an event from the websocket is received? -> run function "webSocketEvent()"
}

void loop() {
  //reading photoresistor values 
  TRRread = analogRead(TRR);
  TLRread = analogRead(TLR);
  BRRread = analogRead(BRR);
  BLRread = analogRead(BLR);

  //initialising web-server
  server.handleClient();                              // Needed for the webserver to handle all clients
  webSocket.loop();                                   // Update function for the webSockets 
  
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
  //delay(10);

  unsigned long now = millis();                       // read out the current "time" ("millis()" gives the time in ms since the Arduino started)
  if ((unsigned long)(now - previousMillis) > interval) { // check if "interval" ms has passed since last time the clients were updated
    
    String jsonString = "";                           // create a JSON string for sending data to the client
    StaticJsonDocument<200> doc;                      // create a JSON container
    JsonObject object = doc.to<JsonObject>();         // create a JSON Object
    object["rand1"] = Pval;                    // write data into the JSON object -> I used "rand1" and "rand2" here, but you can use anything else
    Eval = Pval*interval/1000;
    object["rand2"] = Eval;
    serializeJson(doc, jsonString);                   // convert JSON object to string
    Serial.println(jsonString);                       // print JSON string to console for debug purposes (you can comment this out)
    webSocket.broadcastTXT(jsonString);               // send JSON string to clients
    previousMillis = now;                             // reset previousMillis
  }
  delay(10);
}
