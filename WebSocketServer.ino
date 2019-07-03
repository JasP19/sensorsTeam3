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
AsyncWebServer server(5000);

#define RED 25
#define GREEN 26
#define BLUE 27

const char* ssid = "ATC";
const char* password = "1nchby1nch";
const char* PARAM_MESSAGE = "message";

int distance;

AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if(info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not ewfasfasdgd fgfdsgsdfh found");
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

void setup() {
  Wire.begin();
  screen.begin();

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  initWifi();
  
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
      
      message = distance;
      request->send(200, "text/plain", "Hello, GET: " + message);
  });

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient *client) {
    client->send("hello!",NULL,millis(),1000);
  });
  
  server.addHandler(&events);

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
  
  if (apds.readProximity() != 0) {
    distance = apds.readProximity();
    
    digitalWrite(BLUE, HIGH);
    delay(500);
    digitalWrite(BLUE, LOW);
    screen.println(distance);
    char text[5];

    sprintf(text, "%ld", distance);
    
//    ws.textAll((char*) text);
  
  } else {
  
    distance = 0; 
  
  }
  
  delay(100);
  
}
