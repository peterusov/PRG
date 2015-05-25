/*
HSDC-07: HEADGEAR SGITTAL DISTANCE AND COMPASS METERING (Version 7)
====================================================================
Schematics: SGL-FM-07 Version 2015.4.8.Rev 2.0
--------------------------------------------------------------------

RC: HSDC-07 V:15.4.25.3 - added reset value on "PAUSE" mode;
====================================================================
 */

#include <RFduinoBLE.h>
#include <SFE_HMC6343.h>
#include <Wire.h>
#include <TimedAction.h>
#include <Ultrasonic.h>

//#define _GPIO			0
#define BLUE_LED_GPIO	 	1 // <- DEVELOPMENT BOARD ONLY
#define POWER_SWITCH_GPIO	2
#define	SAGITAL_US_ECHO_GPIO	3
#define SAGITAL_US_TRIG_GPIO	4
#define SCL_GPIO		5
#define SDA_GPI			6

typedef union  { byte b[4]; float f; char c[4]; } rigisterCell;

rigisterCell vHead;
rigisterCell vPitch;
rigisterCell vRoll;
rigisterCell vSagitalDistance;

char ble_send_buf[16];  // 4 byte for each float value. BLE packet buffer is 20 bytes MAX.
SFE_HMC6343 compass;    // Declare the sensor object

int signalLevel = 0;
bool isPause = true;
bool isNeedSagitalDistance = false;
bool isNeedCompass = false;

Ultrasonic usSagital(SAGITAL_US_TRIG_GPIO, SAGITAL_US_ECHO_GPIO);   // (Trig PIN,Echo PIN)

TimedAction timedActionSagitalDistance = TimedAction(100, readSagtSonic);
TimedAction timedActionTiltData = TimedAction(500, thredTiltData);
TimedAction timedActionBLESend = TimedAction(500, thredBLESend);

void setup()
{
	pinMode(POWER_SWITCH_GPIO, OUTPUT);
	pinMode(SAGITAL_US_TRIG_GPIO, OUTPUT);
        pinMode(BLUE_LED_GPIO, OUTPUT);
	pinMode(SAGITAL_US_ECHO_GPIO, INPUT);

	override_uart_limit = true;	
	vHead.f = 0;  vPitch.f = 0;  vRoll.f = 0;  vSagitalDistance.f = 0; 
	/////
	//Serial.begin(9600);
	/////
	RFduinoBLE.deviceName = "HSDC-7.2";
	RFduinoBLE.advertisementData = "hsdcdata";
	RFduinoBLE.advertisementInterval = MILLISECONDS(300);
	//RFduinoBLE.txPowerLevel =  +4;  // (-20dbM to +4 dBm)
	RFduinoBLE.begin();

	delay(500);   // Give the HMC6343 a half second to wake up
		
	if (!compass.init()){
		//Serial.println("Sensor Initialization Failed\n\r"); // Report failure, is the sensor wiring correct?
	}

	delay(500);
	digitalWrite(POWER_SWITCH_GPIO, LOW);
	compass.enterSleep();
	delay(50);
        digitalWrite(BLUE_LED_GPIO, LOW);
}



void loop()
{
	while (RFduinoBLE.radioActive);

	if (!isPause){
		if (isNeedCompass){ timedActionTiltData.check(); }
		if (isNeedSagitalDistance){ timedActionSagitalDistance.check(); }
		timedActionBLESend.check();
	}
}


void readSagtSonic(){
	vSagitalDistance.f = usSagital.Ranging(CM);
	delay(50);
	////
	//Serial.print("Front:"); Serial.print(vFrontDistance.f); Serial.println("Cm");	
	////
}

void thredBLESend(){

	RFduino_ULPDelay(200);
	int i;
	for (i = 0; i < 4; i++){ ble_send_buf[i] = vHead.c[i]; }
	for (i = 4; i < 8; i++){ ble_send_buf[i] = vPitch.c[i - 4]; }
	for (i = 8; i < 12; i++){ ble_send_buf[i] = vRoll.c[i - 8]; }
	for (i = 12; i < 16; i++){ ble_send_buf[i] = vSagitalDistance.c[i - 12]; }	

	while(! RFduinoBLE.send(ble_send_buf, 16));	

    digitalWrite(BLUE_LED_GPIO, HIGH);
    delay(200);
    digitalWrite(BLUE_LED_GPIO, LOW);
        
}

void thredTiltData(){
  
	compass.readHeading();	
	delay(200); //Minimum delay of 200ms (HMC6343 has 5Hz sensor reads/calculations)
  
        vHead.f = (float)compass.heading / 10.0;
	vPitch.f = (float)compass.pitch / 10.0;
	vRoll.f = (float)compass.roll / 10.0;	
        ////
	///prinvHeadingData();  
	////
}


void RFduinoBLE_onReceive(char *data, int len)
{
	uint8_t commandByte = data[0]; 	
	
	//S - (S)tandBy Mode
	if (commandByte == 83){			
		RFduinoBLE.end();
		delay(500);
		compass.enterSleep();
		delay(10);
		RFduino_systemOff();
	};

	//L - Sagita(L) Distance Mode
	if (commandByte == 76){
		digitalWrite(POWER_SWITCH_GPIO, HIGH); delay(50);
		isNeedSagitalDistance = true;
		compass.enterSleep();
		delay(50);		
		isNeedCompass = false;
		isPause = false;
	};
	

	//C - (C)ompass Mode
	if (commandByte == 67){
		isNeedCompass = true;		
		isNeedSagitalDistance = false;		
		digitalWrite(POWER_SWITCH_GPIO, LOW); delay(50);
		compass.exitSleep(); delay(200);		
		isPause = false;		
	};

	//P - (P)ause Mode
	if (commandByte == 80){		
		isPause = true;
		isNeedCompass = false;
		isNeedSagitalDistance = false;
		delay(100);
		compass.enterSleep(); delay(50);
		digitalWrite(POWER_SWITCH_GPIO, LOW); delay(5);
	};	

}

// returns the dBm signal strength indicated by the receiver after connection (-0dBm to -127dBm)
void RFduinoBLE_onRSSI(int rssi){
	signalLevel = rssi;
}

void RFduinoBLE_onAdvertisement(bool start)
{
  if (start)
    digitalWrite(BLUE_LED_GPIO, HIGH);
  else
     digitalWrite(BLUE_LED_GPIO, LOW);
}

void RFduinoBLE_onConnect(){
  digitalWrite(BLUE_LED_GPIO, HIGH);
}

void RFduinoBLE_onDisconnect(){
  digitalWrite(BLUE_LED_GPIO, LOW);
}


//  void prinvHeadingData()
//  {
//  	Serial.print("Heading: ");
//  	Serial.print((float)compass.heading / 10.0); Serial.write(176);  // Print heading in degrees
//  	Serial.print(" | Pitch: ");
//  	Serial.print((float)compass.pitch / 10.0); Serial.write(176); 
//  	Serial.print(" | Roll: ");
//  	Serial.print((float)compass.roll / 10.0); Serial.write(176); 
//  	Serial.println();
//  }


