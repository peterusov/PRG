/*
devider:
9.992K
1.02K
*/

int boardLed = A2;           // pin3
const int analogInPin = A3;  // pin2

int HALF_LEVEL  =  85; //  2.85  V | 10 - 0.38
int MIN_LEVEL   =  70;  //  2.3  V  | 10 - 0.38


void setup(){
    pinMode(boardLed, OUTPUT);
    pinMode(analogInPin, INPUT);
 }

void loop() {
  
  int stat = analogRead(analogInPin);
  delay(5);
  
  if(stat > MIN_LEVEL ){
     digitalWrite(boardLed, HIGH); 
  }
  else{digitalWrite(boardLed, LOW);}
  
   delay(300);
  
 }
