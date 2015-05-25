#include <avr/sleep.h>
#include <avr/interrupt.h>

int NORMAL_LEVEL = 1000; 
int HALF_LEVEL = 800; 
int QUARTER_LEVEL = 500; 

const int ledNormal  =  0;  //Pin 5
const int ledHalf    =  1;  //Pin 6
const int ledQuater  =  2;  //Pin 7
const int switchPin  =  3;  //Pin 2  
const int analogInPin = A2; //PIn 4

int analogIn;

void setup()
{
   
  pinMode(ledNormal, OUTPUT);
  pinMode(ledHalf, OUTPUT);
  pinMode(ledQuater, OUTPUT);    
  pinMode(analogInPin, INPUT);  
  analogIn = 0;   
}


void loop() {    
   analogIn = analogRead(analogInPin); 
   delay(5);
   DisplayLevel();
   delay(8000);
   sleep();
}

void DisplayLevel(){
 
 
    if (analogIn > NORMAL_LEVEL){
      digitalWrite(ledNormal, HIGH);
      digitalWrite(ledHalf, HIGH); 
      digitalWrite(ledQuater, HIGH);
    }    
    if (analogIn <  NORMAL_LEVEL ){digitalWrite(ledNormal, LOW); } 
  
  if (analogIn > HALF_LEVEL ){digitalWrite(ledHalf, HIGH); digitalWrite(ledQuater, HIGH); }  
  if (analogIn <  HALF_LEVEL ){digitalWrite(ledHalf, LOW); } 
  
    if (analogIn > QUARTER_LEVEL){digitalWrite(ledQuater, HIGH); }  
    else{      
         // Flash quick sequence so we know setup has started
          for (int k = 0; k < 100; k = k + 1) {
              if (k % 2 == 0) { digitalWrite(ledQuater, HIGH);}
              else { digitalWrite(ledQuater, LOW);}
              delay(250);
           } // for      
    }
    
    //if (analogIn < 250){  sleep(); }
}

void ShowCharging(int delayDuration){
  
   delay(delayDuration);
   digitalWrite(ledNormal, LOW); 
   digitalWrite(ledHalf, LOW);
   digitalWrite(ledQuater, LOW);
   
   delay(delayDuration);
   digitalWrite(ledNormal, HIGH);
   
   delay(delayDuration);
   digitalWrite(ledHalf, HIGH);
   
   delay(delayDuration);
   digitalWrite(ledQuater, HIGH);
}

void sleep() {
  
   digitalWrite(ledNormal, LOW); 
   digitalWrite(ledHalf, LOW);
   digitalWrite(ledQuater, LOW);

    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
    } // sleep
    
    //TODO: This is called when the interrupt occurs, need to determine is battery charging or not?
    ISR(PCINT0_vect) {
    // This is called when the interrupt occurs,
      ShowCharging(500);
    }

