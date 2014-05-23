Bool GetTCPLine()
{
	char LastChar;
	while ((client.available()>0) && (TCPLastLine.Ready == false))
	{
		LastChar = client.read();
		if (LastChar != '\n')
		{
			TCPLastLine.Line += LastChar;
			TCPLastLine.Ready = false;
		} else
		if (LastChar == '\n')
		{
			TCPLastLine.Ready = true;
			return true;
		}
	}
	if (TCPLastLine.Line =="")
	TCPLastLine.Ready = false;
	return TCPLastLine.Ready;
}

void TCPClearItems()
{
	TCPLastLineItemsCount = 0;
	
	for (int i=0; i<20; i++)
	{
		TCPLastLineItems[i]="";
	}
}

void TCPClearLine()
{
	TCPLastLine.Ready = false;
	TCPLastLine.Line = "";
}

bool TCPSendVersion()
{
	String OutLine;
	OutLine = "/VER/BKBOX/";
	OutLine += APPL_VER;
	OutLine += TCPNewLine;
	client.print(OutLine);
	PageComing = false;
}

bool TCPSendOK()
{
	delay(1);
	String outline;
	outline = "/OK" + TCPNewLine;
	client.print(outline);
}

void TCPSendError(String ErrorMessage)
{
	delay(1);
	String outline;
	outline = "/ERROR/";
	outline = outline + ErrorMessage + LastError + TCPNewLine;
	client.print(outline);
	ErrorMessage = "";
	LastError = "";
}

void TCPSendPageStatus()
{
	client.print("/PAGES/");
	client.println(PageItemsCount);
	for (int i = 0; i< PageItemsCount; i++)
	{
		client.print("/");
		client.print(Pageitems[i].Name);
		client.print("/");
		client.print(Pageitems[i].Address);
		client.print("/");
		client.print(Pageitems[i].Length);
		client.print("/");
		client.print(Pageitems[i].Mult,5);
		client.print("/");
		client.print(Pageitems[i].Offset);
		client.print("/");
		client.println(Pageitems[i].RateMs);
	}
}

bool TCPSendStatus()
{
	// to be implemented to send error messages
}

bool TCPParseCommand()
{
	if (TCPLastLineItems[0] == "VER")
	{
		TCPSendVersion();
		TCPCommandPause = false;
		
	} else
	if (TCPLastLineItems[0] == "PAGE")
	{
		PageItemsCount = atoi(TCPLastLineItems[1].c_str());
		PageComing = true;
		TCPStreamingOn = false;
		TCPCommandPause = false;
		PageComingNr = 0;
		TCPSendOK();
	} else
	if (TCPLastLineItems[0] == "STRTSTREAM")
	{
		TCPStreamingOn = true;
		KeepCANPolling = true;
		PageComing = false;
		TCPSendOK();
		TCPCommandPause = false;
	} else
	if (TCPLastLineItems[0] == "STATUS")
	{
		PageComing = false;
		TCPSendStatus();
		TCPCommandPause = false;
	} else
	if (TCPLastLineItems[0] == "PAGESTATUS")
	{
		PageComing = false;
		TCPSendPageStatus();
		TCPCommandPause = false;
	} else
	if (TCPLastLineItems[0] == "DISCONNECT")
	{
		TCPStreamingOn = false;
		TCPCommandPause = false;
		KeepCANPolling = false;
		PageComing = false;
		InitVariables();
		TCPSendOK();
	}
	if (PageComing && TCPLastLineItems[0]=="VAR")
	{
		Pageitems[PageComingNr].Name = TCPLastLineItems[1];
		Pageitems[PageComingNr].Address = atoi(TCPLastLineItems[2].c_str());
		Pageitems[PageComingNr].Length = atoi(TCPLastLineItems[3].c_str());
		Pageitems[PageComingNr].Mult = atof(TCPLastLineItems[4].c_str());
		Pageitems[PageComingNr].Offset = atoi(TCPLastLineItems[5].c_str());
		Pageitems[PageComingNr].RateMs = atoi(TCPLastLineItems[6].c_str());
		TCPSendOK();
		PageComingNr++;
		if (PageComingNr > PageItemsCount)
		{
			TCPCommandPause = false;
			PageComing = false;
		}
		
	}
}

bool TCPIncomingLineSplit(String IncomingLine)
{
	TCPClearItems();
	
	if (IncomingLine.startsWith("/"))
	{
		IncomingLine.remove(0,1);
		IncomingLine.replace("\r","");
		IncomingLine.replace("\n","");
		
	} else
	{
		client.print("/ERROR/");
		client.println(IncomingLine);
		LastError = "Not A Valid Incoming Command!";
		TCPClearItems();
		TCPClearLine();
		return false;
	}
	
	for (int i=0;i<IncomingLine.length();i++)
	{
		if (IncomingLine[i]!='/')
		{
			TCPLastLineItems[TCPLastLineItemsCount] += IncomingLine[i];
		} else
		{
			TCPLastLineItemsCount++;
		}
	}
	//client.println(TCPLastLineItems[0]);
	TCPClearLine();
	return true;
	
}