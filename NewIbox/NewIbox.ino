#include <sn65hvd234.h>
#include <due_can.h>
#include "Globals.h"
#include "WString.h"

/*
 * NewIbox.ino
 *
 * Created: 5/14/2014 11:29:28 AM
 * Author: T9569BK
 */ 


void setup()
{
	  InitVariables();
	  InitSerial();
	  CanReqestID = 0x18DAF110;
	  CanResopnseID = 0x18DA10F1;
	  IsCANExtFrame = true;
	  

}

void loop()
{
	
	
#pragma region Serial Streaming to begin turn on the CAN

	if (SerialStreamingOn && !IsCanInit)										//if the streaming command came but CAN not init yet
	{
		if (InitCanBus())														// Init Can bus at 500K
		{
			SetAllCanFilters();													//Set filters on CAN
								
			if (SetCANExtendedDiagMode())										// Start extended diagnostic session
			{
				IsCanInit = true;
				ActualVariable = 0;	
			} else
				SerialSendError("Unable to enter Extended Diagnostic Mode: "); //Send error message on the serial bus
		} else
			SerialSendError("Unable to open CAN Bus: ");						//Send error message to the serial bus
	} else
#pragma endregion Serial Streaming to begin turn on the CAN
//Else
#pragma region Serial Streaming On 
if (SerialStreamingOn && IsCanInit)											//if we already init everything and ready to stream
	{
		if (!WaitingForResponse)											// if there is no Pending request out
		{
			SendGetByAddressReq(Pageitems[ActualVariable].Address,Pageitems[ActualVariable].Length);
			TurnDone = false;
		} else
		if ((LastRawRes = GetRawResponseByAddress()) != -1)					//if there is a pending request check if we have the response
		{
			LastReadValues[ActualVariable] = GetValueFromRawResponse(LastRawRes,ActualVariable);
			if (ActualVariable < (PageItemsCount-1))
			{
				ActualVariable++;
			}
			else
			{
				ActualVariable = 0;
				TurnDone = true;
			}
		}
		if (TurnDone)														//if all variables polled already send out
		{
			Serial.print(GetTimeStamp());
			for (int i=0; i<PageItemsCount; i++)
			{
				Serial.print(",");
				Serial.print(LastReadValues[i]);
			}
			Serial.println("");
			TurnDone = false;
		}
				
		
	} else
#pragma endregion Serial Streaming On 
//Else
#pragma region Serial Streaming To End Turn Off the CAN
	if (!SerialStreamingOn && IsCanInit)										//if serial streaming was shut down we shut down the can line too.
	{
		CAN.disable();
#pragma endregion Serial Streaming To End Turn Off the CAN
	}
	
#pragma region Every 50ms poll the serial line for new commands
if ((millis()%50) == 0)
	{
		GetSerialLine();	
		if (SerialLastLine.Ready)
		{
			SerialIncomingLineSplit(SerialLastLine.Line);
			SerialParseCommand();
		}
	} 
#pragma endregion Every 50ms poll the serial line for new commands	
}
