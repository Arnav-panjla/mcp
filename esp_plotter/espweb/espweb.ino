#include <ESP32Servo.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Replace with your network credentials
const char* ssid = "moto g42";
const char* password = "12345678";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Servo myservo[2];  // create servo object to control a servo (probably an array to control 2 servos) 
                // 16 servo objects can be created on the ESP32

int pos_a = 90;    // variable to store the inital Aservo position
int pos_b = 90;    // variable to store the inital Bservo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int read_val = 34;
int AservoPin = 12;
int BservoPin = 13;
int TRR = 16;
int TLR = 4;
int BRR = 2;
int BLR = 14;
//tolerance factor for each photoresistor (Default value is 1)
int TRRtf = 1;
int TLRtf = 1;
int BRRtf = 1;
int BLRtf = 1;
//variables declaration
int TRRread;
int TLRread;
int BRRread;
int BLRread;

int TRRval;
int TLRval;
int BRRval;
int BLRval;
//

float Vval; // variable for actual voltage reading from solar output 
float Pval; //variable that contains power value in Watt
int Res = 1000 ; //resistor value (3 identical will be connected) (1k??)
int Readval; //value of analog read from voltage divider (solar output)
float temperature;


///////////////////////////////////////website code/////////////////////////////////
AsyncWebServer server(80);
AsyncEventSource events("/events");

unsigned long lastTime = 0;  
unsigned long timerDelay = 250;  // send readings timer

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
        <h4><i style='font-size:24px' class='fas'>&#xf5ba;</i> POWER</h4><p><span class="reading"><span id="temp">%TEMPERATURE%</span> W</span></p>
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

    //////////////////////////////////website code ////////////////////////////////////////////////////
      // Set the device as a Station and Soft Access Point simultaneously
    WiFi.mode(WIFI_AP_STA);
  
    // Set device as a Wi-Fi Station
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Setting as a Wi-Fi Station..");
    }
    Serial.print("Station IP Address: ");
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


    delay(10);
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

    if (( TRRval > TLRval ) or ( BRRval > BLRval )){
        pos_a=pos_a+1;
        myservo[1].write(pos_a);
    }
    if (( TRRval < TLRval ) or ( BRRval < BLRval )){
        pos_a=pos_a-1;
        myservo[1].write(pos_a);
    }
    if (( BRRval > TRRval ) or ( BLRval > TLRval )){
        pos_a=pos_a+1;
        myservo[2].write(pos_a);
    }
    if (( BRRval < TRRval ) or ( BLRval < TLRval )){
        pos_a=pos_a-1;
        myservo[2].write(pos_a);
    }
    else{
      
    }

    ///reads value form solar output
    Readval = analogRead(read_val); //value range from 0-4092
    Vval = Readval*(9.9/4092);
    Pval = (Vval*Vval)/Res;
    temperature = Pval*10;
    Serial.println(temperature);
    Serial.println(Vval);
    delay(100);
    ///website code to display solar output
    //////////////////////////////////////////////website code/////////////////////////////////////////////
    if ((millis() - lastTime) > timerDelay) {
      Serial.printf("Temperature = %.4f ºC \n", temperature);
      Serial.println();

    // Send Events to the Web Server with the Sensor Readings
      events.send("ping",NULL,millis());
      events.send(String(temperature).c_str(),"temperature",millis());
    
      lastTime = millis();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    
  
}
