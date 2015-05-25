#include <RFduinoBLE.h>

#define RX_GPIO       	2
#define TX_GPIO         3

char ble_send_buf[12];
int i;

void setup() {
   i =0 ;
  Serial.begin(9600, RX_GPIO, TX_GPIO); 
  
  RFduinoBLE.deviceName = "BSS-AT7";
  RFduinoBLE.advertisementData = "bssatdata";
  RFduinoBLE.advertisementInterval = MILLISECONDS(300);
  //RFduinoBLE.txPowerLevel =  +4;  // (-20dbM to +4 dBm)
  RFduinoBLE.begin(); 
 
}


void loop()  {
  
 if (Serial.available()>0){
   ble_send_buf[i]= Serial.read();
   if (int(ble_send_buf[i])==13 || int(ble_send_buf[i])==10 ){ //If Carriage return has been reached
       BLESend();
     for(int x=0;x<=10;x++){ ble_send_buf[x]=' ';}
     i=0;
   }
   i++; 
  } //If rfDuinoSerial.available
  
 }
 
 void BLESend(){
   while(! RFduinoBLE.send(ble_send_buf, 12));	
 }



