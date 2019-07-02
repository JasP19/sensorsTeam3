#include <SPI.h>
#include <WiFi.h>
#include <aREST.h>
#include "Adafruit_APDS9960.h"
#include <xCore.h>
#include <xVersion.h>
#include "xOD01.h"

xOD01 screen;
Adafruit_APDS9960 apds;

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
int distance;
//int temperature;
//int humidity;

// Declare functions to be exposed to the API
int ledControl(String command);
int getProximity(String command);

void setup() {
  Wire.begin();
  screen.begin();

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // Start Serial
  Serial.begin(115200);

  // Init variables and expose them to REST API
  distance = 0;
//  temperature = 24;
//  humidity = 40;
//  rest.variable("temperature", &temperature);
//  rest.variable("humidity", &humidity);
  rest.variable("distance", &distance);

//  rest.function("led", ledControl);
  rest.function("distance", getProximity);

  // Give name and ID to device (ID should be 6 characters long)
  rest.set_id("777008");
  rest.set_name("LED_LIGHT");

  // Function to be exposed
//  rest.function("led", ledControl);
  rest.function("distance", getProximity);

  // Attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // Wait 10 seconds for connection
    delay(5000);
  }

  // Start the server
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

  // Print out the status
  printWifiStatus();
}

void loop() {

  // listen for incoming clients
  WiFiClient client = server.available();
  rest.handle(client);

  if (apds.readProximity() != 0) {
    distance = apds.readProximity();
    digitalWrite(BLUE, HIGH);
    delay(500);
    digitalWrite(BLUE, LOW);
  } else {
    distance = 0; 
  }
  
  delay(1000);

}

int getProximity(String command) {
  int state = command.toInt();

  screen.println(distance);

  return distance;
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
