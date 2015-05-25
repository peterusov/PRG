//beta HTDX-07 V:15.1.23.1

#include <RFduinoBLE.h>
#include <TimedAction.h>
#include <Ultrasonic.h>

//#define _GPIO			0
#define BLUE_LED_GPIO	 	1
#define POWER_SWITCH_GPIO	2
#define XRAY_READER_GPIO        3
//#define           _GPIO	4
#define AXIAL_US_TRIG_GPIO	5
#define AXIAL_US_ECHO_GPIO	6

#define ARRAY_DEPTH 500

typedef union  { byte b[4]; float f; char c[4]; } rigisterCell;

rigisterCell vXrayFlag;
rigisterCell vAxialDistance;
rigisterCell vXrayValue;
rigisterCell vSecs;

char ble_send_buf[16];  // 4 byte for each float value. BLE packet buffer is 20 bytes MAX.

int signalLevel = 0;
bool isPause = true;
bool isNeedAxialDistance = false;
bool isNeedXray = false;

int pointer;
float monValArray[ARRAY_DEPTH];
float THRESHOLD;

bool startFlag;
int start;
int endT;


Ultrasonic usAxial(AXIAL_US_TRIG_GPIO, AXIAL_US_ECHO_GPIO);   // (Trig PIN,Echo PIN)

TimedAction timedActionAxialDistance = TimedAction(100, readSagtSonic);
TimedAction timedActionBLESend = TimedAction(500, thredBLESend);
//TimedAction timedActionXrayRead = TimedAction(100, thredXRayRead);

void setup()
{
	pinMode(POWER_SWITCH_GPIO, OUTPUT);
	pinMode(BLUE_LED_GPIO, OUTPUT);
	pinMode(AXIAL_US_TRIG_GPIO, OUTPUT);
	pinMode(AXIAL_US_ECHO_GPIO, INPUT);

	override_uart_limit = true;

	startFlag = false;
	THRESHOLD = 60;
	pointer = 0;
	vXrayFlag.f = 0;
	vSecs.f = 0;
	
	vAxialDistance.f = 0;
	vXrayValue.f = 0;
	/////
	//Serial.begin(9600);
	/////
	RFduinoBLE.deviceName = "HTDX-07";
	RFduinoBLE.advertisementData = "htdxdata";
	RFduinoBLE.advertisementInterval = MILLISECONDS(300);
	//RFduinoBLE.txPowerLevel =  +4;  // (-20dbM to +4 dBm)
	RFduinoBLE.begin();

	delay(50);
	digitalWrite(POWER_SWITCH_GPIO, LOW);
	delay(50);
	digitalWrite(BLUE_LED_GPIO, LOW);

}


void loop()
{
	while (RFduinoBLE.radioActive);

	if (!isPause)
	{
		if (isNeedAxialDistance){
			timedActionAxialDistance.check();
			timedActionBLESend.check();
		}

		if (isNeedXray)
		{
			vXrayValue.f = analogRead(XRAY_READER_GPIO);
			delay(1);

				if (vXrayValue.f >= THRESHOLD || startFlag)
				{
					if (!startFlag){ start = millis(); startFlag = true; }

						if (pointer < ARRAY_DEPTH){
							monValArray[pointer] = vXrayValue.f;
							pointer++;
							endT = millis();
							// Serial.print("Val: "); Serial.print(mon_val[pointer]); 
						}
						else{
							vSecs.f = (endT - start) / 1000.0;
							start = 0;
							endT = 0;
							vXrayFlag;	
							//TO DO: Send Array
						}
				}
		}

	}

}



void readSagtSonic(){
	vAxialDistance.f = usAxial.Ranging(CM);
	delay(50);
	////
	//Serial.print("Front:"); Serial.print(vFrontDistance.f); Serial.println("Cm");	
	////
}

void thredBLESend(){

	RFduino_ULPDelay(200);
	int i;
	for (i = 0; i < 4; i++){ ble_send_buf[i] = vAxialDistance.c[i]; }
	for (i = 4; i < 8; i++){ ble_send_buf[i] = vXrayFlag.c[i - 4]; }
	for (i = 8; i < 12; i++){ ble_send_buf[i] = vSecs.c[i - 8]; }


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

	//S - char byte "StandBy Mode"
	if (commandByte == 83){
		RFduinoBLE.end();
		delay(500);
		digitalWrite(BLUE_LED_GPIO, LOW);
		delay(2);
		RFduino_systemOff();
	};

	//A - Axial Distance Mode
	if (commandByte == 76){
		digitalWrite(POWER_SWITCH_GPIO, HIGH); delay(50);
		isNeedAxialDistance = true;
		vAxialDistance.f = 0;		
		startFlag = false;
		pointer = 0;
		vXrayFlag.f = 0;
		isNeedXray = false;
		isPause = false;
	};

	//X - Listen XRay
	if (commandByte == 88){
		cleanMonValArray();
		startFlag = false;		
		pointer = 0;
		vXrayFlag.f = 0;

		digitalWrite(POWER_SWITCH_GPIO, LOW);
		delay(50);
		isNeedXray = true;
		delay(50);
		isNeedAxialDistance = false;
		isPause = false;
	};

	//P - Pause Mode
	if (commandByte == 80){
		isPause = true;
		isNeedAxialDistance = false;
		delay(100);
		cleanMonValArray();
		startFlag = false;
		pointer = 0;
		vXrayFlag.f = 0;
		digitalWrite(POWER_SWITCH_GPIO, LOW); delay(5);
	};

}


void cleanMonValArray(){
	for (int i = 0; i < ARRAY_DEPTH; i++){	
		monValArray[i] = 0;
	}

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
