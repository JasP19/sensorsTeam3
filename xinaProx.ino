#include "Adafruit_APDS9960.h" //include Adafruit library @ https://github.com/adafruit/Adafruit_APDS9960
#include <xCore.h> //include core library @ https://github.com/xinabox/xCore

//the pin that the interrupt is attached to
#define INT_PIN 3

#define WIFI_SSID "ATC"
#define WIFI_PASSWORD "1nchby1nch"

#define RED_PIN 25
#define GREEN_PIN 26
#define BLUE_PIN 27

//create the APDS9960 object
Adafruit_APDS9960 apds;

void setup() {

//  Wire.begin(2, 14);
  Wire.begin();
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.begin(115200);
  pinMode(INT_PIN, INPUT_PULLUP);

  digitalWrite(BLUE_PIN, HIGH);
  delay(5000);

  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
      digitalWrite(BLUE_PIN, LOW);
      digitalWrite(RED_PIN, HIGH);
      delay(500);
      digitalWrite(RED_PIN, LOW);
  }
  else {
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    delay(500);
    digitalWrite(GREEN_PIN, LOW);
  }

  //enable proximity mode
  apds.enableProximity(true);

  //set the interrupt threshold to fire when proximity reading goes above 175
  apds.setProximityInterruptThreshold(0, 175);

  //enable the proximity interrupt
  apds.enableProximityInterrupt();
}

void loop() {
    digitalWrite(GREEN_PIN, HIGH);
    delay(500);
    digitalWrite(GREEN_PIN, LOW);

  //print the proximity reading when the interrupt pin goes low
//  if(!digitalRead(INT_PIN)){
//    digitalWrite(BLUE_PIN, HIGH);
//    delay(1000);
//    //clear the interrupt
//    apds.clearInterrupt();
//  }
//
//  digitalWrite(BLUE_PIN, LOW);

    if (apds.readProximity() < 1) {
      digitalWrite(BLUE_PIN, HIGH);
      delay(500);
      digitalWrite(BLUE_PIN, LOW);
    }
    delay(1000);
}
