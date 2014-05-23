#include <SPI.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <EthernetClient.h>
#include <Ethernet.h>
#include <Dns.h>
#include <Dhcp.h>
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
	  Ethernet.begin(mac, serverIP);
	  server.begin();
	  SPI.setClockDivider(10, 8);
	  CanReqestID = 0x18DA10F1;
	  CanResopnseID = 0x18DAF110;
	  IsCANExtFrame = true;
	  

}

void loop()
{
 client = server.available();
  
#pragma region TCP Routines If Connected
while (client.connected())																	// if a client is connected 
 {
	 //////////////////////////////////////////////////////////////////////////
	 /// TCP Streaming just turned on, need to init CAN 
	 //////////////////////////////////////////////////////////////////////////
	 

	 if(TCPStreamingOn && !IsCanInit && (TryCount<MAX_TRY_COUNT))
	 {
		 if (InitCanBus())														// Init Can bus at 500K
		 {
			 SetAllCanFilters();													//Set filters on CAN
			 
			 if (SetCANExtendedDiagMode())										// Start extended diagnostic session
			 {
				 IsCanInit = true;
				 ActualVariable = 0;
				 TryCount = 0;
			 } else
			 {
				 LastError = "Timeout!";
				 TCPSendError("Unable to enter Extended Diagnostic Mode: "); //Send error message on the serial bus
				 TryCount++;
				 if (TryCount == MAX_TRY_COUNT)
				 {
					 LastError = "Timeout!";
					 TCPSendError("Maximum number of retries reached. Please check vehicle connection, Disconnect and Reconnect!");
					 TCPSendError("");
				 }
			 }
		 } else
		 {
			 LastError = "CAN SYNC Error!";
			 TCPSendError("Unable to open CAN Bus: ");						//Send error message to the serial bus
			 TryCount++;
		 }	 
	 } else

	 //////////////////////////////////////////////////////////////////////////
	 /// TCP Streaming ON
	 //////////////////////////////////////////////////////////////////////////
	 

	 if (TCPStreamingOn && IsCanInit && !TCPCommandPause)
	 {
		 
	 } else
	 

	 //////////////////////////////////////////////////////////////////////////
	 /// TO STOP TCP STREAMING and Turn Off CAN bus
	 //////////////////////////////////////////////////////////////////////////
	 
	 if (!TCPStreamingOn && IsCanInit)
	 {
		 CAN.disable();
		 IsCanInit = false;
	 }
	 
	 //////////////////////////////////////////////////////////////////////////
	 /// EVERY 50ms Check For Incoming TCP command
	 //////////////////////////////////////////////////////////////////////////
	
	if (client.available())
	 {
		 if (TCPLastLine.Line == "")
		 {
			 char LastRead = client.read();
			 TCPLastLine.Line += LastRead;
			 TCPLastLine.Ready = false;
			 if (LastRead == '/')
			 {
				 TCPCommandPause = true;
			 }
		 }
		 
		 GetTCPLine();
		 if (TCPLastLine.Ready)
		 {
			 TCPIncomingLineSplit(TCPLastLine.Line);
			 TCPParseCommand();
		 }
	 }					// End of TCP incoming command check
 }						// End of TCP Client connected
#pragma endregion TCP Routines If Connected
	
#pragma region Serial Routines
#pragma region Serial Streaming to begin turn on the CAN

	if (SerialStreamingOn && !IsCanInit && (TryCount<MAX_TRY_COUNT))			//if the streaming command came but CAN not init yet
	{
		if (InitCanBus())														// Init Can bus at 500K
		{
			SetAllCanFilters();													//Set filters on CAN
								
			if (SetCANExtendedDiagMode())										// Start extended diagnostic session
			{
				IsCanInit = true;
				ActualVariable = 0;	
				TryCount = 0;
			} else
			{
				SerialSendError("Unable to enter Extended Diagnostic Mode: "); //Send error message on the serial bus
				TryCount++;
				if (TryCount == MAX_TRY_COUNT)
				{
					SerialSendError("Maximum number of retries reached. Please check vehicle connection, Disconnect and Reconnect!");
				}
			}
		} else
		{
			SerialSendError("Unable to open CAN Bus: ");						//Send error message to the serial bus
			TryCount++;
		}
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
			WaitingForResponse = false;
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
		IsCanInit = false;
#pragma endregion Serial Streaming To End Turn Off the CAN
	}
	
#pragma region Every 50ms poll the serial line for new commands
if ((millis()%50) == 0)
	{
		GetSerialLine();	
		if (SerialLastLine.Ready) // && (SerialLastLine.Line.startsWith("/")))
		{
			SerialIncomingLineSplit(SerialLastLine.Line);
			SerialParseCommand();
		}
	} 
#pragma endregion Every 50ms poll the serial line for new commands	
#pragma endregion Serial Routines
}
