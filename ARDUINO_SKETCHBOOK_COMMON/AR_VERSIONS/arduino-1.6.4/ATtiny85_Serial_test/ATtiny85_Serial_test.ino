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
#define rxPin 4
#define txPin 3
SoftwareSerial mySerial(rxPin, txPin);



// the setup routine runs once when you press reset:
void setup() {

  mySerial.begin(9600);
  analogWrite(0,1); // This is kind a heart beat to show the ATTINY is "alive"
}

// the loop routine runs over and over asensorpingain forever:
void loop() {
  mySerial.println("Test");
}

