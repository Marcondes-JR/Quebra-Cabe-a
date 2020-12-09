#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
// Initialize all libraries for remote support 
#include <IRremoteESP8266.h>
#include <IRsend.h>
IRsend irsend(4);  // IR LED Pin as per our circuit.
// prototypes
boolean connectWifi();

//on/off callbacks 
bool arcondicionadoOn();
bool arcondicionadoOff();
bool tvOn();
bool tvOff();

// Change this before you flash
const char* ssid = "brisa-389632";
const char* password = "cxq6w5gz";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *arcondicionado = NULL;
Switch *tv = NULL;

bool isarcondicionadoOn = false;
bool istvtsOn = false;

void setup()
{
  Serial.begin(9600);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 10
    // Format: Alexa invocation name, local port no, on callback, off callback
    arcondicionado = new Switch("arcondicionado", 80, arcondicionadoOn, arcondicionadoOff);
    tv = new Switch("tv", 81, tvOn, tvOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*arcondicionado);
    upnpBroadcastResponder.addDevice(*tv);
  }
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      tv->serverLoop();
      arcondicionado->serverLoop();
	 }
}

bool arcondicionadoOn() {
    Serial.println("ARCONDICIONADO LIGADO");
    
    isarcondicionadoOn = true;
    irsend.sendNEC(0x1F1B5E27); 
    Serial.println("irsend.sendNEC(0x1F1B5E27);");   
    return isarcondicionadoOn;
}

bool arcondicionadoOff() {
    Serial.println("ARCONDICIONADO DESLIGADO");

    isarcondicionadoOn = false;
    irsend.sendNEC(0x1F1B5E27); 
    return isarcondicionadoOn;
}

bool tvOn() {
    Serial.println("TV LIGADA");

    istvtsOn = true;
    irsend.sendNEC(0x1F1B5E27);
    return istvtsOn; //VER PQ TA ISTVTSON????????????????????????????????????????????????????????????
}

bool tvOff() {
  Serial.println("TV DESLIGADA");

  istvtsOn = false;
  irsend.sendNEC(0x1F1B5E27);
  
  return istvtsOn;
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}
