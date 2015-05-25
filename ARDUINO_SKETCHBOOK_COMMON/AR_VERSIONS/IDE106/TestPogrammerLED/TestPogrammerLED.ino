int boardLed = A2; //

void setup(){
    pinMode(boardLed, OUTPUT);
 }

void loop() {
  
   delay(300);
    digitalWrite(boardLed, LOW);
   delay(300);
    digitalWrite(boardLed, HIGH); 
 }

