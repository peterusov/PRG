/*
LiPo battery voltage monitor with ATtiny85 and ower Management System
SGL-FM-07
====================================================================
Schematics: SGL-FM-07 Version 2015.4.8.Rev 2.0
--------------------------------------------------------------------

 V:2015.2.6  corrected when USB disconnected:
             when USB disconnected than STAT = 1.9 V ( 279 value )
        
V:2015.2.5  added Charge Status Output (STAT) functionality
	    STAT is a tri-state logic output on the MCP73831
            SGL-FM-07 schematics v2015.2.5 REV 1.1:
              when USB powering off than STAT = 0 V
              when Charging is ON  than  STAT = 0.5 V ( 155 value )
              when Charging if OFF than  STAT > 1.9 V                      
                   
            devider: R1/R22 <-> 1 K / 1.02K 
====================================================================
 */

#include <avr/sleep.h>
#include <avr/interrupt.h>

int NORMAL_LEVEL  =  560; //  Max = 3.6  V 
int HALF_LEVEL = 445;      //  Nom = 3.0  V, Half = 2.85V
int QUARTER_LEVEL = 325;   //  Min = 2.1  V

const int ledNormal = 0;	//Pin 5
const int ledHalf = 1;		//Pin 6
const int ledQuater = 2;	//Pin 7
const int statPin = 3;	        //Pin 2  
const int analogInPin = A2;	//Pin 3

int analogIn;
bool chargeStatus;

void setup()
{

	pinMode(ledNormal, OUTPUT);
	pinMode(ledHalf, OUTPUT);
	pinMode(ledQuater, OUTPUT);
	pinMode(analogInPin, INPUT);
	analogIn = 0;
	chargeStatus = false;
	delay(500);
}


void loop() {

	int stat = analogRead(statPin);
	delay(50);
	
        if (stat < 200){ chargeStatus = true; }
	else{ chargeStatus = false; }

	if (!chargeStatus){
		analogIn = analogRead(analogInPin);
		delay(5);
		DisplayLevel();
		delay(8000);
		sleep();
	}
        else{	ShowCharging(500); }
}

void DisplayLevel(){


	if (analogIn > NORMAL_LEVEL){
		digitalWrite(ledNormal, HIGH);
		digitalWrite(ledHalf, HIGH);
		digitalWrite(ledQuater, HIGH);
	}
	if (analogIn <  NORMAL_LEVEL){ digitalWrite(ledNormal, LOW); }

	if (analogIn > HALF_LEVEL){ digitalWrite(ledHalf, HIGH); digitalWrite(ledQuater, HIGH); }
	if (analogIn <  HALF_LEVEL){ digitalWrite(ledHalf, LOW); }

	if (analogIn > QUARTER_LEVEL){ digitalWrite(ledQuater, HIGH); }
	else{
		// Flash quick sequence so we know setup has started
		for (int k = 0; k < 100; k = k + 1) {
			if (k % 2 == 0) { digitalWrite(ledQuater, HIGH); }
			else { digitalWrite(ledQuater, LOW); }
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
	loop();
}

