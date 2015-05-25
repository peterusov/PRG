#include <Ultrasonic.h>
#include <RFduinoBLE.h>

#define US_TRIG_GPIO    5
#define US_ECHO_GPIO    6

Ultrasonic curentUltarasonic(US_TRIG_GPIO, US_ECHO_GPIO);   // (Trig PIN,Echo PIN)

void setup() {
  Serial.begin(9600);

}

void loop() {
  Serial.println(curentUltarasonic.Ranging(CM));

}
