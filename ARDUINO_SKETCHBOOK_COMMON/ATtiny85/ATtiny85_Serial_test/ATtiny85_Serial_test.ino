/*
Takes an input on the AtTiny85 from the VERY TRICKY analogue input pin 2 (as labeled in HLT tutorial)
 and outputs these with Software serial to the arduino uno, or Duemillanove
 Hookup
 ATTINY85 pin 3 -> Arduino Uno pin 0
 ATTINY85 pin 4 -> Arduino Uno pin 1

http://www.instructables.com/id/Easy-ATTiny-Serial-Communication-with-Tiny-AVR-Pro/
 */


#include <SoftwareSerial.h>
// Definitions
#define rxPin 3
#define txPin 4
SoftwareSerial mySerial(rxPin, txPin);



// the setup routine runs once when you press reset:
void setup() {
   pinMode(1, OUTPUT);
  mySerial.begin(2400);
  analogWrite(0,1); // This is kind a heart beat to show the ATTINY is "alive"
}

// the loop routine runs over and over asensorpingain forever:
void loop() {
  mySerial.println("Test Peter");
  digitalWrite(1, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(300);              // wait for a second
  digitalWrite(1, LOW);    // turn the LED off by making the voltage LOW
  delay(300);              // wait for a second
}

