#include "WString.h"

Bool InitVariables()
{
	SerialStreamingOn = false;
	KeepCANPolling = false;
	IsCanInit = false;
	WaitingForResponse = false;
	SerialLastLine.Line = "";
	SerialLastLine.Ready =false;
	PageComing =false;
	TryCount = 0;
	TCPCommandPause = false;
}

/**
 * Initialize Serial Port 
 * 
 * 
 * \return true / may implement some error handling later
 */
Bool InitSerial()
{
	Serial.begin(115200);
	return true;
}

/**
 * \brief 
 *	Gets a line from serial port and puts it in the SerialLastLine struct. Sets the ready flag and exits if new line found.
 * 
 * \return Bool
 */
Bool GetSerialLine()
{
	char LastChar;
	while ((Serial.available()>0) && (SerialLastLine.Ready == false))
	{
		LastChar = Serial.read();
		if (LastChar != '\n')
		{
			SerialLastLine.Line += LastChar;
			SerialLastLine.Ready = false;
		} else
		if (LastChar == '\n')
		{
			SerialLastLine.Ready = true;
			return true;
		}		
	}
	if (SerialLastLine.Line =="")
		SerialLastLine.Ready = false;
	return SerialLastLine.Ready;
}

/**
 * \brief 
 *
 *  Splits the IncomingLine String into the SerialLastLineItems string array by the '/' delimiter
 *  Also checks if the string is starting with the delimiter 
 *
 * \param IncomingLine
 *	A line string containing '/' delimited items
 * 
 * \return Bool
 *	True	if the string starts with '/'
 *	False	if the string is not starting with the '/' delimiter
 */
bool SerialIncomingLineSplit(String IncomingLine)
{
	SerialClearItems();
	
	if (IncomingLine.startsWith("/"))
	{
		IncomingLine.remove(0,1);
			
	} else
	{
		Serial.print("/ERROR/");
		Serial.println(IncomingLine);
		LastError = "Not A Valid Incoming Command!";
		SerialClearItems();
		SerialClearLine();
		return false;
	}
	
	for (int i=0;i<IncomingLine.length();i++)
	{
		if (IncomingLine[i]!='/')
		{
			SerialLastLineItems[SerialLastLineItemsCount] += IncomingLine[i];		
		} else
		{
			SerialLastLineItemsCount++;
		}
	}
	SerialClearLine();
	return true;
	
}

void SerialClearItems()
{
	SerialLastLineItemsCount = 0;
	
	for (int i=0; i<20; i++)
	{
		SerialLastLineItems[i]="";
	}
}

void SerialClearLine()
{
	SerialLastLine.Ready = false;
	SerialLastLine.Line = "";
}

bool SerialSendVersion()
{
	String OutLine;
	OutLine = "/VER/BKBOX/";
	OutLine += APPL_VER;
	Serial.println(OutLine);
	PageComing = false;
}

bool SerialSendOK()
{
	Serial.println("/OK");
}

void SerialSendError(String ErrorMessage)
{
	Serial.print("/ERROR/");
	Serial.print(ErrorMessage);
	Serial.println(LastError);
	ErrorMessage = "";
	LastError = "";
}

bool SerialSendStatus()
{
	// to be implemented to send error messages
}

void SerialSendPageStatus()
{
	Serial.print("/PAGES/");
	Serial.println(PageItemsCount);
	for (int i = 0; i< PageItemsCount; i++)
	{
		Serial.print("/");
		Serial.print(Pageitems[i].Name);
		Serial.print("/");
		Serial.print(Pageitems[i].Address);
		Serial.print("/");
		Serial.print(Pageitems[i].Length);
		Serial.print("/");
		Serial.print(Pageitems[i].Mult,5);
		Serial.print("/");
		Serial.print(Pageitems[i].Offset);
		Serial.print("/");
		Serial.println(Pageitems[i].RateMs);
	}
}

bool SerialParseCommand()
{
	if (SerialLastLineItems[0] == "VER")
	{
		SerialSendVersion();
		
	} else
	if (SerialLastLineItems[0] == "PAGE")
	{
		PageItemsCount = atoi(SerialLastLineItems[1].c_str());
		PageComing = true;
		PageComingNr = 0;
		SerialSendOK();
	} else
	if (SerialLastLineItems[0] == "STRTSTREAM")
	{
		SerialStreamingOn = true;
		KeepCANPolling = true;
		PageComing = false;
		SerialSendOK();
	} else
	if (SerialLastLineItems[0] == "STATUS")
	{
		PageComing = false;
		SerialSendStatus();
	} else
	if (SerialLastLineItems[0] == "PAGESTATUS")
	{
		PageComing = false;
		SerialSendPageStatus();
	} else
	if (SerialLastLineItems[0] == "DISCONNECT")
	{
		SerialStreamingOn = false;
		KeepCANPolling = false;
		PageComing = false;
		InitVariables();
		SerialSendOK();
	}
	if (PageComing && SerialLastLineItems[0]=="VAR")
	{
		Pageitems[PageComingNr].Name = SerialLastLineItems[1];
		Pageitems[PageComingNr].Address = atoi(SerialLastLineItems[2].c_str());
		Pageitems[PageComingNr].Length = atoi(SerialLastLineItems[3].c_str());
		Pageitems[PageComingNr].Mult = atof(SerialLastLineItems[4].c_str());
		Pageitems[PageComingNr].Offset = atoi(SerialLastLineItems[5].c_str());
		Pageitems[PageComingNr].RateMs = atoi(SerialLastLineItems[6].c_str());
		SerialSendOK();
		PageComingNr++;
		if (PageComingNr > PageItemsCount) 
			PageComing = false;
	}
}
