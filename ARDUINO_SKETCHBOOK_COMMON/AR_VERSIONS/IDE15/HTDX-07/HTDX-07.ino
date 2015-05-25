/*
HTDX-07: HEADGEAR TRANSVERSE DISTANCE AND X-RAY METERING (Version 7)
====================================================================
Schematics: SGL-FM-07 Version 2015.4.8.Rev 2.0
--------------------------------------------------------------------

RC: HTDX-07 V:15.4.26.3 - added reset value on "PAUSE" mode;
====================================================================
 */

#include <RFduinoBLE.h>
#include <TimedAction.h>
#include <Ultrasonic.h>

//#define _GPIO			0
#define BLUE_LED_GPIO	 	1
#define USND_POWER_SWITCH_GPIO	2
#define XRAY_READER_GPIO        3
//#define           _GPIO	4
#define AXIAL_US_TRIG_GPIO	5
#define AXIAL_US_ECHO_GPIO	6


char ble_send_buf[16];  // 4 byte for each float value. BLE packet buffer is 20 bytes MAX.

typedef union  { byte b[4]; float f; char c[4]; } rigisterCell;
rigisterCell vXrayFlag;
rigisterCell vAxialDistance;
rigisterCell vXrayValue;

int signalLevel = 0;
bool isPause = true;
bool isNeedAxialDistance = false;
bool isNeedXray = false;

Ultrasonic usAxial(AXIAL_US_TRIG_GPIO, AXIAL_US_ECHO_GPIO);   // (Trig PIN,Echo PIN)

TimedAction timedActionAxialDistance = TimedAction(100, readSagtSonic);
TimedAction timedActionBLESend = TimedAction(500, thredBLESend);
TimedAction timedActionXrayRead = TimedAction(100, thredXRayRead);

void setup()
{
	pinMode(USND_POWER_SWITCH_GPIO, OUTPUT);
	pinMode(BLUE_LED_GPIO, OUTPUT);
	pinMode(AXIAL_US_TRIG_GPIO, OUTPUT);
	pinMode(AXIAL_US_ECHO_GPIO, INPUT);

	override_uart_limit = true;
	vXrayFlag.f = 0;  vAxialDistance.f = 0; vXrayValue.f = 0;
	/////
	//Serial.begin(9600);
	/////
	RFduinoBLE.deviceName = "HTDX-7.2";
	RFduinoBLE.advertisementData = "htdxdata";
	RFduinoBLE.advertisementInterval = MILLISECONDS(300);
	//RFduinoBLE.txPowerLevel =  +4;  // (-20dbM to +4 dBm)
	RFduinoBLE.begin();

	delay(50);
	digitalWrite(USND_POWER_SWITCH_GPIO, LOW);
        delay(50);
	digitalWrite(BLUE_LED_GPIO, LOW);
	
}


void loop()
{
	while (RFduinoBLE.radioActive);

	if (!isPause){		
		if (isNeedAxialDistance){ 
                    timedActionAxialDistance.check();
                    timedActionBLESend.check();
                  }
		if (isNeedXray){ 
                    timedActionXrayRead.check();
                    timedActionBLESend.check(); 
                  }
		//else{ timedActionBLESend.check(); }			
	}
}

void thredXRayRead(){
	vXrayValue.f = analogRead(XRAY_READER_GPIO);
        delay(5);
}


void readSagtSonic(){
	vAxialDistance.f = usAxial.Ranging(CM);
	delay(50);
	////
	//Serial.print("Front:"); Serial.print(vAxialDistance.f); Serial.println("Cm");	
	////
}

void thredBLESend(){

	RFduino_ULPDelay(200);
	int i;
	for (i = 0; i < 4; i++){ ble_send_buf[i] = vAxialDistance.c[i]; }
	for (i = 4; i < 8; i++){ ble_send_buf[i] = vXrayValue.c[i - 4]; }

	/*for (i = 8; i < 12; i++){ ble_send_buf[i] = vRoll.c[i - 8]; }
	for (i = 12; i < 16; i++){ ble_send_buf[i] = vSagitalDistance.c[i - 12]; }*/	

	while (!RFduinoBLE.send(ble_send_buf, 16));

	digitalWrite(BLUE_LED_GPIO, HIGH);
	delay(200);
	digitalWrite(BLUE_LED_GPIO, LOW);

}



void RFduinoBLE_onReceive(char *data, int len)
{
	uint8_t commandByte = data[0];

	//S  -  (S)tandBy Mode
	if (commandByte == 83){
		RFduinoBLE.end();
		delay(500);
		digitalWrite(BLUE_LED_GPIO, LOW);
		delay(2);
		RFduino_systemOff();
	};

	//L  -  Axia(L) Distance Mode
	if (commandByte == 76){
		digitalWrite(USND_POWER_SWITCH_GPIO, HIGH); delay(50);
		isNeedAxialDistance = true;		
		delay(50);
		isNeedXray = false;
		isPause = false;
	};
	
	//X  -  Listen (X)Ray
	if (commandByte == 88){
		digitalWrite(USND_POWER_SWITCH_GPIO, LOW);
                delay(50);
		isNeedXray = true;		
		delay(50);
		isNeedAxialDistance = false;		
                vAxialDistance.f = 0; vXrayValue.f = 0;
		isPause = false;
                vAxialDistance.f=0;
	};
	
	//P  -  (P)ause Mode
	if (commandByte == 80){
		isPause = true;	
		isNeedAxialDistance = false;
		delay(100);		
		digitalWrite(USND_POWER_SWITCH_GPIO, LOW); delay(50);
                vXrayFlag.f = 0;  vAxialDistance.f = 0; vXrayValue.f = 0;   
                delay(50);               

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

