#include <RFduinoBLE.h>
#include <Wire.h>
#include <SFE_HMC6343.h>

#define RED_LED_PIN   2
#define GREEN_LED_PIN 3
#define BLUE_LED_PIN  4

typedef union  {
	byte b[4];
	float f;
	char  c[4];
} registerData;

char ble_send_buf[16];  // BLE packet buffer is 20 bytes MAX.
SFE_HMC6343 compass;    // Declare the sensor object


registerData vXray;

void setup()
{

	pinMode(RED_LED_PIN, OUTPUT);
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(BLUE_LED_PIN, OUTPUT);

	Serial.begin(9600);

	RFduinoBLE.deviceName = "BSS-07";
	RFduinoBLE.advertisementData = "data";
	RFduinoBLE.advertisementInterval = MILLISECONDS(300);
	//RFduinoBLE.txPowerLevel =  +4;  // (-20dbM to +4 dBm)
	RFduinoBLE.begin();

	delay(500);   // Give the HMC6343 a half second to wake up

	// Initialize the HMC6343 and verify its physical presence
	if (!compass.init()){
		Serial.println("Sensor Initialization Failed\n\r"); // Report failure, is the sensor wiring correct?
	}
}

void loop()
{

	compass.readHeading();    // Read, calculate, and print the heading, pitch, and roll from the sensor
	printHeadingData();

	sendBLEData();

	vXray.f=analogRead(2);

	digitalWrite(BLUE_LED_PIN, LOW);
	delay(50);
	digitalWrite(GREEN_LED_PIN, HIGH);
	delay(50);
	digitalWrite(GREEN_LED_PIN, LOW);

	delay(100); //Minimum delay of 200ms (HMC6343 has 5Hz sensor reads/calculations)

}

void sendBLEData()
{
	registerData vHead;
	registerData vPitch;
	registerData vRoll;

	vHead.f = (float)compass.heading / 10.0;
	vPitch.f = (float)compass.pitch / 10.0;
	vRoll.f = (float)compass.roll / 10.0;
	//  tHead.f =  (float) 123.51; tPitch.f = (float) -181.2; tRoll.f =  (float) -0.49;

	int i;
	for (i = 0; i<4; i++){ ble_send_buf[i] = vHead.c[i]; }
	for (i = 4; i<8; i++){ ble_send_buf[i] = vPitch.c[i - 4]; }
	for (i = 8; i<12; i++){ ble_send_buf[i] = vRoll.c[i - 8]; }
	for (i = 12; i<16; i++){ ble_send_buf[i] = vXray.c[i - 12]; }

	RFduinoBLE.send(ble_send_buf, 16);

}

void printHeadingData()
{
	Serial.print("Heading: ");
	Serial.print((float)compass.heading / 10.0); Serial.write(176); Serial.println(); // Print heading in degrees
	Serial.print("Pitch: ");
	Serial.print((float)compass.pitch / 10.0); Serial.write(176); Serial.println();
	Serial.print("Roll: ");
	Serial.print((float)compass.roll / 10.0); Serial.write(176); Serial.println();
	Serial.println();
}


void RFduinoBLE_onAdvertisement(bool start)
{
	if (start)
		digitalWrite(BLUE_LED_PIN, HIGH);
	else
		digitalWrite(BLUE_LED_PIN, LOW);
}

void RFduinoBLE_onConnect()
{
	digitalWrite(BLUE_LED_PIN, HIGH);
}

void RFduinoBLE_onDisconnect()
{
	digitalWrite(BLUE_LED_PIN, LOW);
}

void RFduinoBLE_onReceive(char *data, int len)
{
	uint8_t myByte = data[0]; // store first char in array to myByte
	Serial.println(myByte); // print myByte via serial


	digitalWrite(BLUE_LED_PIN, LOW);	
	digitalWrite(RED_LED_PIN, HIGH);
	//digitalWrite(green_led, HIGH);
	delay(300);
	digitalWrite(RED_LED_PIN, LOW);
	//digitalWrite(green_led, LOW);


}



