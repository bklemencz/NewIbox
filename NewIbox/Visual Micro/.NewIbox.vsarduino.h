/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Due (Programming Port), Platform=sam, Package=arduino
*/

#define __SAM3X8E__
#define USB_VID 0x2341
#define USB_PID 0x003e
#define USBCON
#define USB_MANUFACTURER "\"Unknown\""
#define USB_PRODUCT "\"Arduino Due\""
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 84000000L
#define printf iprintf
#define __SAM__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
//
Bool InitCanBus ();
Bool SetAllCanFilters();
Bool SetCANExtendedDiagMode();
Bool SendGetByAddressReq(int32_t MemAddress,byte Size);
int32_t GetRawResponseByAddress();
double GetValueFromRawResponse(int32_t RawResponse, int PageVarNr);
int GetTimeStamp ();
Bool InitVariables();
Bool InitSerial();
Bool GetSerialLine();
bool SerialIncomingLineSplit(String IncomingLine);
void SerialClearItems();
void SerialClearLine();
bool SerialSendVersion();
bool SerialSendOK();
void SerialSendError(String ErrorMessage);
bool SerialSendStatus();
void SerialSendPageStatus();
bool SerialParseCommand();

#include "c:\Program Files (x86)\Arduino\hardware\arduino\sam\variants\arduino_due_x\pins_arduino.h" 
#include "c:\Program Files (x86)\Arduino\hardware\arduino\sam\variants\arduino_due_x\variant.h" 
#include "c:\Program Files (x86)\Arduino\hardware\arduino\sam\cores\arduino\arduino.h"
#include "C:\Users\Balazs\Documents\Atmel Studio\6.1\NewIbox\NewIbox\NewIbox.ino"
#include "C:\Users\Balazs\Documents\Atmel Studio\6.1\NewIbox\NewIbox\CanComm.ino"
#include "C:\Users\Balazs\Documents\Atmel Studio\6.1\NewIbox\NewIbox\Globals.h"
#include "C:\Users\Balazs\Documents\Atmel Studio\6.1\NewIbox\NewIbox\Globals.ino"
#include "C:\Users\Balazs\Documents\Atmel Studio\6.1\NewIbox\NewIbox\SerialCom.ino"
