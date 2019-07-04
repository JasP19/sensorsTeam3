/*
 * WebSocketServer.ino
 *
 *  Created on: 22.05.2015
 *
 */

#include <Arduino.h>
#include <string>
#include <WiFi.h>
#include "Adafruit_APDS9960.h"
#include <xCore.h>
#include <xVersion.h>
#include "xOD01.h"
#include <WebSocketClient.h>

xOD01 screen;
Adafruit_APDS9960 apds;
char path[] = "/";
char host[] = "192.168.47.197";

const char* ssid = "ATC";
const char* password = "1nchby1nch";
int distance;

WebSocketClient webSocketClient;
WiFiClient client;

#define RED 25
#define GREEN 26
#define BLUE 27

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

void initConnect() {
  
  if (client.connect("192.168.47.197", 8080)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
  }
  
}

void initHandshake() {
  webSocketClient.path = path;
  webSocketClient.host = host;
  
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
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

  initWifi();
  initConnect();

  while (!client.connected()) {
    initConnect();
  }

  initHandshake();

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
  String data;

  if (client.connected()) {
    
    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data);
    }
    
    if (apds.readProximity() != 0) {
      distance = apds.readProximity();

      digitalWrite(BLUE, HIGH);
      delay(500);
      digitalWrite(BLUE, LOW);
      screen.println(distance);

      data = String(distance);
    
      webSocketClient.sendData(data);
  
    }
    
  } else {
    Serial.println("Client disconnected.");
    
    while (!client.connected()) {
      initConnect();
    }

    initHandshake();
  }

  delay(1000);
  
}
