#include <TimedAction.h>
#include <RFduinoBLE.h>
#include <Ultrasonic.h>

//#define _GPIO           0
#define POWER_SWITCH_GPIO 1
#define READ_XRAY_GPIO    2
#define FRONT_TRIG_GPIO   3
#define FRONT_ECHO_GPIO   4
#define BACK_TRIG_GPIO    5
#define BACK_ECHO_GPIO    6

typedef union  { byte b[4]; float f; char c[4]; } rigisterCell;

rigisterCell vFrontDistance;
rigisterCell vBackDistance;
rigisterCell vXRay;

char ble_send_buf[12];  // 4 byte for each float value. BLE packet buffer is 20 bytes MAX.
int signalLevel = 0;

Ultrasonic ultraFront(FRONT_TRIG_GPIO,FRONT_ECHO_GPIO);   // (Trig PIN,Echo PIN)
Ultrasonic ultraBack(BACK_TRIG_GPIO,BACK_ECHO_GPIO);   // (Trig PIN,Echo PIN)

TimedAction timedActionReadXRay = TimedAction(100, readXRay);
TimedAction timedActionReadUSsonic = TimedAction(100, readUSsonic);
TimedAction timedActionBLESend = TimedAction(500, thredBLESend);

bool isPause = true;

void setup() {
  
  //Serial.begin(9600);  
  vFrontDistance.f = 0; vBackDistance.f = 0; vXRay.f = 0;
  
  pinMode(POWER_SWITCH_GPIO, OUTPUT);
  delay(5); 
  digitalWrite(POWER_SWITCH_GPIO, LOW);
  
  RFduinoBLE.deviceName = "HUS-01";
  RFduinoBLE.advertisementData = "husdata";
  RFduinoBLE.advertisementInterval = MILLISECONDS(300);
  //RFduinoBLE.txPowerLevel =  +4;  // (-20dbM to +4 dBm)
  RFduinoBLE.begin();   
  
}

void loop() {
  
  while (RFduinoBLE.radioActive);

  if(!isPause){
    timedActionReadXRay.check();	
    timedActionReadUSsonic.check();
    timedActionBLESend.check();			  
  }
}

void readUSsonic(){
  vFrontDistance.f = ultraFront.Ranging(CM);
  delay(50);
  vBackDistance.f = ultraBack.Ranging(CM);
  delay(50);   
}

void readXRay(){
	vXRay.f = analogRead(READ_XRAY_GPIO);
	delay(2);
}
void thredBLESend(){

	RFduino_ULPDelay(200);
	int i;
	for (i = 0; i < 4; i++){ ble_send_buf[i] = vFrontDistance.c[i]; }
	for (i = 4; i < 8; i++){ ble_send_buf[i] = vBackDistance.c[i - 4]; }
	for (i = 8; i < 12; i++){ ble_send_buf[i] = vXRay.c[i - 8]; }
	//for (i = 12; i < 16; i++){ ble_send_buf[i] = vXRay.c[i - 12]; }

	while(! RFduinoBLE.send(ble_send_buf, 12));	
}


void RFduinoBLE_onAdvertisement(bool start)
{
	if (start)
		//digitalWrite(BLUE_LED_GPIO, HIGH)
          ;
	else
		//digitalWrite(BLUE_LED_GPIO, LOW)
          ;
}

void RFduinoBLE_onConnect()
{
	//digitalWrite(BLUE_LED_GPIO, HIGH);
}

void RFduinoBLE_onDisconnect()
{
	//digitalWrite(BLUE_LED_GPIO, LOW);
}

void RFduinoBLE_onReceive(char *data, int len){
  
        uint8_t commandByte = data[0]; 
  
        //S - char byte "StandBy Mode"
	if (commandByte == 83){
                isPause = true;				
		digitalWrite(POWER_SWITCH_GPIO, LOW);
		delay(2);
                RFduinoBLE.end();  		
		delay(10);
		RFduino_systemOff();
	};

	//U - Start Ultrasonic
	if (commandByte == 85){
		digitalWrite(POWER_SWITCH_GPIO, HIGH);				
		delay(10);
		isPause = false;		
	};

	//P - Pause Mode
	if (commandByte == 84){		
                isPause = true;
		delay(100);
		digitalWrite(POWER_SWITCH_GPIO, LOW);				
	};
	

}

// returns the dBm signal strength indicated by the receiver after connection (-0dBm to -127dBm)
void RFduinoBLE_onRSSI(int rssi){
	signalLevel = rssi;
}
