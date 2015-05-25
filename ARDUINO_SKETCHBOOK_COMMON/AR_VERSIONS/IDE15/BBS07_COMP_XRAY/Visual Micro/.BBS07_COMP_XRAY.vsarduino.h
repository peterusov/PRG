/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: RFduino, Platform=RFduino, Package=arduino
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define __RFduino__
#define ARDUINO 158
#define ARDUINO_MAIN
#define printf iprintf
#define __RFDUINO__
#define __RFduino__
#define F_CPU 16000000
#define __cplusplus


//
//
void readSagtSonic();
void thredBLESend();
void RFduinoBLE_onReceive(char *data, int len);
void cleanMonValArray();
void RFduinoBLE_onRSSI(int rssi);
void RFduinoBLE_onAdvertisement(bool start);
void RFduinoBLE_onConnect();
void RFduinoBLE_onDisconnect();

#include "W:\PROGRAMMING\ArduinoIDE15\hardware\arduino\RFduino\cores\arduino\arduino.h"
#include "W:\PROGRAMMING\ArduinoIDE15\hardware\arduino\RFduino\variants\RFduino\libRFduino.h" 
#include "W:\PROGRAMMING\ArduinoIDE15\hardware\arduino\RFduino\variants\RFduino\libRFduinoBLE.h" 
#include "W:\PROGRAMMING\ArduinoIDE15\hardware\arduino\RFduino\variants\RFduino\libRFduinoGZLL.h" 
#include "W:\PROGRAMMING\ArduinoIDE15\hardware\arduino\RFduino\variants\RFduino\pins_arduino.h" 
#include "W:\PROGRAMMING\ArduinoIDE15\hardware\arduino\RFduino\variants\RFduino\variant.h" 
#include "W:\PROGRAMMING\ArduinoIDE15\Sketchbook\BBS07_COMP_XRAY\BBS07_COMP_XRAY_V1.3.ino"
#endif
