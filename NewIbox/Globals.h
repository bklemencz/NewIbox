/*
 * Globals.h
 *
 * Created: 5/14/2014 1:26:51 PM
 *  Author: T9569BK
 */ 
#include <SPI.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <EthernetClient.h>
#include <Ethernet.h>
#include <Dns.h>
#include <Dhcp.h>

#ifndef GLOBALS_H_

//////////////////////////////////////////////////////////////////////////
//// CONSTANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////

const int CAN_TIMEOUT_MS = 500;
const String APPL_VER = "V0.2";
const int MAX_TRY_COUNT = 5;

//////////////////////////////////////////////////////////////////////////
/// STUCTURE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct PageItems_t
{
	String Name;
	int32_t Address;
	int Length;
	double Mult;
	int Offset;
	int RateMs;
};

struct SerialReadLine_t
{
	String Line;
	Bool Ready;
};


union MemAddress_t
{
	int Address;
	byte Bytes[4];
	
};

//////////////////////////////////////////////////////////////////////////
//// MAIN GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////
PageItems_t Pageitems[24];
int PageItemsCount;
String LastError;

//////////////////////////////////////////////////////////////////////////
////// ETHERNET VARIABLES
//////////////////////////////////////////////////////////////////////////
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress serverIP(192,168,0,50);
int serverPort=1000;
String TCPNewLine = "\r\n";

EthernetServer server(serverPort);
EthernetClient client;
SerialReadLine_t TCPLastLine;
String TCPLastLineItems[20];
String TCPToSendLine;
int TCPLastLineItemsCount;
bool TCPStreamingOn,TCPCommandPause;


//////////////////////////////////////////////////////////////////////////
//// SERIAL COMMUNICATION GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////

SerialReadLine_t SerialLastLine;
String SerialLastLineItems[20];
String SerialToSendLine;
double LastReadValues[24];
int SerialLastLineItemsCount,PageComingNr;
bool SerialStreamingOn,PageComing;
int TimeStamp,SessionStartTime;

//////////////////////////////////////////////////////////////////////////
///// CAN COMMUNICATION GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////

int32_t CanReqestID, CanResopnseID;
int32_t LastRawRes;
Bool IsCANExtFrame;
Bool KeepCANPolling,IsCanInit;
Bool WaitingForResponse,TurnDone;
int ActualVariable;
int TryCount;



#define GLOBALS_H_





#endif /* GLOBALS_H_ */