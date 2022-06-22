//  TITLE: Project02
//  AUTHOR: Ike Callaghan
//  DATE: 21/06/2022
//
//------------------------------------------------------------------------------------------------------------
//  PURPOSE:
//    This script is designed to routinely measure the UV index rating of the local area.
//    It will then post the UV rating, along with health and safety recommendations to 
//    '@DunedinUV' on Twitter.com, using the ifttt Applet platform. 
//
//------------------------------------------------------------------------------------------------------------
//  COMPONENTS:
//    Arduino NODEMCU ESP8266
//    9g micro servos
//    Servo pan and tilt housing
//    XC4518 Ultraviolet Sensor
//    Jumper leads
//
//------------------------------------------------------------------------------------------------------------
//  INCLUDED FILES:
//    ESP8266Wifi.h
//    ESP8266HTTPClient.h
//    WifiClient.h
//    Servo.h
//
//------------------------------------------------------------------------------------------------------------
//  FUNCTIONS:
//
//    readUV()
//      Reads output from XC4518 sensor, converts reading to voltage. Voltage is then converted
//      to UV index.
//      If the UV index reading is higher than the UV post variable, it is assigned to UV post. 
//
//    spfCalc()
//      Reads UV post value to decide which message to send to twitter. 
//         0 - 3: "apply SPF 15".
//         3 - 5: "apply SPF 15+ and wear protective clothing".
//         5 - 7: "apply SPF 30+ and wear protective clothing".
//        7 - 10: "apply SPF 50, wear protective clothing and seek shade".
//           10+: "apply SPF 50+, wear protective clothing and avoid the sun between 10am and 4pm".
//           15+:  Calls watchdog timer to restart device, highest recorded NZ UV index is 14.6.
//
//    actionTilt()
//      Called after every completed pan motion.
//      Moves the tilt servo up and down while reading UV data from readUV(). 
//
//    twitterRequest()
//      Makes a twitter post request using an IFTTT applet.
//      Posts UV post variable from readUV() and UV post message from spfCalc()
//
//    actionPan()
//      Attaches both servos to their respective pins at beginning of function call.
//      Pans 90 degrees in one direction while reading UV data from readUV().
//      Once panned 90 degrees, actionTilt() is called. 
//      The servo will make two 180 degree motions before calling spfCalc(), detatching servo motors, 
//      resetting the uvPost value to 0 and sets a delay for 1 hour. 
//
//------------------------------------------------------------------------------------------------------------
//
//  RESOURCES: 
//    https://twitter.com/DunedinUv
//    https://ifttt.com/explore
//    https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay
//    Example sketches for NodeMCU ESP8266
//
//------------------------------------------------------------------------------------------------------------

#include <ESP8266WiFi.h>       
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h> 
#include <Servo.h> 

//Pins
#define ServoPin1 14   //D5 is GPIO14
#define ServoPin2 12   //D6 is GPIO12
#define UV_METER A0

//WiFi 
const char* ssid     = "NETWORK NAME";  // SSID (Name of WiFi network)
const char* password = "PASSWORD";      // Wi-Fi network password

//API
const char* serverName = "http://maker.ifttt.com/trigger/YOUR_API/with/key/YOUR_KEY";

//Servos
Servo servoBase;
Servo servoHead;
int pos = 0; 

//UV
float uvRead = 0;
float uvIndex = 0;
float uvPost = 0;
float uvThresh = 0.30;
String uvSPF = "";

void setup() {
  Serial.begin(115200);   
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);               // Connect to network
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) {   // Wait for connection
    delay(500);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Print the localIP (Connection verification)
  WiFi.setAutoReconnect(true);            // Set WiFi auto reconnect. 
  WiFi.persistent(true);
}

void loop() { 
  actionPan();
}

//------------------------------------------------------------------------------------------------------------
//  FUNCTIONS
//------------------------------------------------------------------------------------------------------------

void actionTilt() {
   for (pos = 0; pos <= 180; pos += 1) {  // Goes from 180 degrees to 0 degrees
    // in steps of 1 degree
    servoHead.write(pos);                 // Tell servo to go to position in variable 'pos'
    delay(20);                            // Movement Speed
    readUV();
  }
  
  for (pos = 180; pos >= 0; pos -= 1) { 
    servoHead.write(pos);             
    delay(20);                       
    readUV();
  }
}

void actionPan() {
  
  servoBase.attach(ServoPin1); 
  servoHead.attach(ServoPin2);
  
  for (int i = 1; i <= 2; ++i) {     
    
    for (pos = 0; pos <= 90; pos += 1) { 
      // in steps of 1 degree
      servoBase.write(pos);              
      delay(20);                       
      readUV();
    }
    actionTilt();
  
    for (pos = 90; pos <= 180; pos += 1) { 
      // in steps of 1 degree
      servoBase.write(pos);              
      delay(20);                      
      readUV();
    }
    actionTilt();
  
    for (pos = 180; pos >= 90; pos -= 1) { 
      servoBase.write(pos);             
      delay(20);                      
      readUV();
    }
    actionTilt();
    
    for (pos = 90; pos >= 0; pos -= 1) { 
      servoBase.write(pos);             
      delay(20);                       
      readUV();
    }
    actionTilt();
  }
  
  spfCalc();
  twitterRequest();
  servoBase.detach();           //Servos were making noise when idling, detatch them while not detecting UV.
  servoHead.detach();
  uvPost = 0;
  delay(1 * 60 * 1000UL);       //1 Minute Delay (For testing purposes)
}

void readUV() {
  int uvRead = analogRead(UV_METER);
  float uvVolt = uvRead * (5.0 / 1023.0);
  float uvIndex = uvVolt/.1;
  
  if (uvIndex > uvPost) {       //Set uvPost value to highest attained value during capture phase. 
    uvPost = uvIndex;
    Serial.print("UV Index: ");         
    Serial.println(uvPost);  
  }
}

void spfCalc() {
  if (uvPost < 3) {
    uvSPF = "apply SPF 15";
  }
    else if (uvPost >= 3 && uvPost < 5) {
      uvSPF = "apply SPF 15+ and wear protective clothing";
    }
      else if (uvPost >= 5 && uvPost < 7) {
        uvSPF = "apply SPF 30+ and wear protective clothing";
      }
          else if (uvPost >= 7 && uvPost < 10)  {
            uvSPF = "apply SPF 50, wear protective clothing and seek shade";
          }
            else if (uvPost >= 10 && uvPost < 17)  {
              uvSPF = "apply SPF 50+, wear protective clothing and avoid the sun between 10am and 4pm";
            }
             else if (uvPost > 15)  {
              ESP.restart();   //NodeMCU library uses ESP.restart() instead of wdt_enable()
                               //Next addition would be email notification if NODEMCU has to restart.
             }
}

void twitterRequest(){
  if (uvPost >= uvThresh) {                                                             //Post if detected value is below the threshold
    HTTPClient http;          
    http.begin(serverName);                                                             //Begin HTTP client
    Serial.println(uvPost);
    Serial.println(uvSPF);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  
    String httpRequestData = "value1=" + String(uvPost) + "&value2=" + String(uvSPF);   //Assign variables to IFTTT variables    
    int httpResponseCode = http.POST(httpRequestData);    //Make a post request
    Serial.print("HTTP Response code: ");  
    Serial.println(httpResponseCode);                                                   //Print the response code (200 = posted)
    http.end();
  }
}
