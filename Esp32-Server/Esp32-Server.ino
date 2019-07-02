//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Adafruit_APDS9960.h"
#include <xCore.h>
#include <xVersion.h>
#include "xOD01.h"

xOD01 screen;
Adafruit_APDS9960 apds;
AsyncWebServer server(8080);

#define RED 25
#define GREEN 26
#define BLUE 27

const char* ssid = "ATC";
const char* password = "1nchby1nch";
const char* PARAM_MESSAGE = "message";

int distance;

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not ewfasfasdgd fgfdsgsdfh found");
}

void setup() {
  Wire.begin();
  screen.begin();

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "Hello, world");
  });

  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String message;
      screen.println(distance);
      
//      if (request->hasParam(PARAM_MESSAGE)) {
//          message = request->getParam(PARAM_MESSAGE)->value();
//      } else {
//        message = distance;
//      }
      
      message = distance;
      request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
    String message;
      
    if (request->hasParam(PARAM_MESSAGE, true)) {
        message = request->getParam(PARAM_MESSAGE, true)->value();
    } else {
      message = "No message sent";
    }
     
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.onNotFound(notFound);

  server.begin();

  if (!apds.begin()) {
    Serial.println("failed to initialize device! Please check your wiring.");
  } else Serial.println("Device initialized!");

  //enable proximity mode
  apds.enableProximity(true);

  //set the interrupt threshold to fire when proximity reading goes above 175
  apds.setProximityInterruptThreshold(0, 175);

  //enable the proximity interrupt
  apds.enableProximityInterrupt();
}

void loop() {
  AsyncWebSocket ws("/ws");

  
  if (apds.readProximity() != 0) {
    distance = apds.readProximity();
    
    digitalWrite(BLUE, HIGH);
    delay(500);
    digitalWrite(BLUE, LOW);
    screen.println(distance);

    ws.textAll((char*) distance);
    screen.println("Success");
  
  } else {
  
    distance = 0; 
  
  }
  
  delay(100);
  
}
