/*
  This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
  using the WiFi library (for example to be used with the Arduino WiFi shield).
  See the README file for more details.
  Written in 2014 by Marco Schwartz under a GPL license.
*/

#include <SPI.h>
#include <WiFi.h>
#include <aREST.h>
#include <xCore.h>
#include <xVersion.h>
#include "xOD01.h"

xOD01 screen;

#define RED 25
#define GREEN 26
#define BLUE 27

char ssid[] = "ATC"; //  your network SSID (name)
char pass[] = "1nchby1nch";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Create aREST instance
aREST rest = aREST();

// Initialize the WiFi server library
WiFiServer server(80);

// Variables to be exposed to the API
int temperature;
int humidity;

// Declare functions to be exposed to the API
int ledControl(String command);

void setup() {
  Wire.begin();
  screen.begin();
//  screen.set2X();
//  screen.print("Hello ");
//  screen.set1X();
//  screen.println("World");
//  delay(5000);
//  screen.clear();

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // Start Serial
  Serial.begin(115200);

  // Init variables and expose them to REST API
  temperature = 24;
  humidity = 40;
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);

  rest.function("led", ledControl);

  // Give name and ID to device (ID should be 6 characters long)
  rest.set_id("777008");
  rest.set_name("LED_LIGHT");

  // Function to be exposed
  rest.function("led", ledControl);

  // Attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // Wait 10 seconds for connection
    delay(5000);
  }

//  screen.set2X();
//  screen.println("CONNECTED");

  // Start the server
  server.begin();

  // Print out the status
  printWifiStatus();
}

void loop() {

  // listen for incoming clients
  WiFiClient client = server.available();
  rest.handle(client);

}

// Custom function accessible by the API
int ledControl(String command) {

  // Get state from command
  int state = command.toInt();

  screen.println(state);
  digitalWrite(RED, state);
  return 1;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
//  screen.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  screen.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}