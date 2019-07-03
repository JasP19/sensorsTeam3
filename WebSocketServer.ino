/*
 * WebSocketServer.ino
 *
 *  Created on: 22.05.2015
 *
 */

#include <Arduino.h>
#include <string>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "Adafruit_APDS9960.h"
#include <xCore.h>
#include <xVersion.h>
#include "xOD01.h"
#include <WebSocketsServer.h>

WiFiMulti WiFiMulti;
WebSocketsServer webSocket = WebSocketsServer(5000);
xOD01 screen;
Adafruit_APDS9960 apds;

const char* ssid = "ATC";
const char* password = "1nchby1nch";
int distance;

#define RED 25
#define GREEN 26
#define BLUE 27

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        
                // send message to client
                webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            getProx();

            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary length: %u\n", num, length);

            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    }

}

void initWifi() {
  IPAddress ip(192, 168, 47, 201);
  IPAddress gateway(192, 168, 47, 55);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.config(ip, gateway, subnet);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(5000);
  }

}

void getProx() {

  Serial.println("getProx()");
  
  while (true) {
    
    if (apds.readProximity() != 0) {
      distance = apds.readProximity();

      uint8_t num;

      webSocket.sendTXT(num, "BLAH");
    
      digitalWrite(BLUE, HIGH);
      delay(500);
      digitalWrite(BLUE, LOW);
      screen.println(distance);

      break;
    
    } else {
  
      distance = 0; 
  
    } 
  
  }

}

void setup() {
  Wire.begin();
  screen.begin();

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  // Serial.begin(921600);
  Serial.begin(115200);

  //Serial.setDebugOutput(true);
  Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    initWifi();

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

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
  
    webSocket.loop();
  
}
