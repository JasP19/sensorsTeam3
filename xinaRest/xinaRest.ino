/*
  This a simple example of the aREST Library for the ESP32 WiFi chip.
  See the README file for more details.

  Written in 2017 by Marco Schwartz under a GPL license.
  
*/

// Import required libraries
#include <WiFi.h>
#include <aREST.h>

#include "Adafruit_APDS9960.h" //include Adafruit library @ https://github.com/adafruit/Adafruit_APDS9960
#include <xCore.h> //include core library @ https://github.com/xinabox/xCore

#define WIFI_SSID "ATC"
#define WIFI_PASSWORD "1nchby1nch"

#define RED_PIN 25
#define GREEN_PIN 26
#define BLUE_PIN 27

//create the APDS9960 object
Adafruit_APDS9960 apds;

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "ATC";
const char* password = "1nchby1nch";

// Create an instance of the server
WiFiServer server(5000);

// Custom function accessible by the API
int getProximity(String command) {

  Serial.println("Server call");
  double dist = apds.readProximity();

//  String output = "{Distance: " + String(dist,4) + "}";
  
  return 1;
}

void initWifi() {
  IPAddress ip(192, 168, 47, 200);
  IPAddress gateway(192, 168, 47, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.config(ip, gateway, subnet);

  digitalWrite(RED_PIN, HIGH);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(5000);
  }
  digitalWrite(RED_PIN, LOW);
}

void setup()
{

  // Function to be exposed
  rest.function("getProximity",getProximity);

  Wire.begin();
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.begin(115200);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("1");
  rest.set_name("esp32");

  // Connect to WiFi
  initWifi();

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else {
    Serial.println("device initialized.");
  }

  //enable proximity mode
  apds.enableProximity(true);

  //set the interrupt threshold to fire when proximity reading goes above 175
//  apds.setProximityInterruptThreshold(0, 175);

  //enable the proximity interrupt
//  apds.enableProximityInterrupt();

}

void loop() {
  
  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  rest.handle(client);
}
