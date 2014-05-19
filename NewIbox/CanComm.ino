/**
 * \brief 
 * 
 * 
 * \return 
 * \ false if CAN Init failed, string in LastError
 * \ true if CAN init success
 */

Bool InitCanBus ()
{
	if (CAN.init(CAN_BPS_500K)) 
	{
		return true;
	}
	else 
	{
		return false;
		LastError = "CAN PORT INIT ERROR!";
	}
}

Bool SetAllCanFilters()
{
	if (IsCANExtFrame)
	{
		CAN.setRXFilter(0, CanResopnseID, 0x1FFFFFFF, true);
	} else
	{
		CAN.setRXFilter(0, CanResopnseID, 0x7FF, false);
	}
	return true;	
}

Bool SetCANExtendedDiagMode()
{
	CAN_FRAME Frame1,incoming;
	int SentOutMilis;
	Frame1.id = CanReqestID;
	Frame1.extended = IsCANExtFrame;
	Frame1.length = 8;
	Frame1.data.bytes[0]=0x02;
	Frame1.data.bytes[1]=0x10;
	Frame1.data.bytes[2]=0x92;
	CAN.sendFrame(Frame1);
	
	SentOutMilis = millis();
	while ((!CAN.rx_avail()) && ((millis()-SentOutMilis)<CAN_TIMEOUT_MS) );
	
	if (CAN.rx_avail())
	{
		CAN.get_rx_buff(incoming);
		if ((incoming.data.bytes[1]==0x50) && (incoming.data.bytes[2]==0x92))
		{
			return true;
		} else
		{
			LastError = "No Positive Response for Extended Request";
			return false;
		}
	} else
	{
		LastError = "CAN Timeout";
		return false;
	}	
}

Bool SendGetByAddressReq(int32_t MemAddress,byte Size)
{
	MemAddress_t TempAddress;
	TempAddress.Address = MemAddress;
	CAN_FRAME OutGoingFr;
	OutGoingFr.extended = IsCANExtFrame;
	OutGoingFr.id = CanReqestID;
	OutGoingFr.length = 8;
	OutGoingFr.data.bytes[0] = 0x05;
	OutGoingFr.data.bytes[1] = 0x23;
	OutGoingFr.data.bytes[2] = TempAddress.Bytes[2];
	OutGoingFr.data.bytes[3] = TempAddress.Bytes[1];
	OutGoingFr.data.bytes[4] = TempAddress.Bytes[0];
	OutGoingFr.data.bytes[5] = Size;
	CAN.sendFrame(OutGoingFr);
	WaitingForResponse = true;
	return true;
	
}

int32_t GetRawResponseByAddress()
{
	CAN_FRAME In_Frame;
	if(CAN.rx_avail())
	{
		CAN.get_rx_buff(In_Frame);
		// Response to the ReadByMemoryAddress function
		if (In_Frame.data.bytes[1] == 0x63)
		{
			// One byte response
			if (In_Frame.data.bytes[0] == 0x02)
			{
				return In_Frame.data.bytes[2];
			} else
			// Two Byte response
			if (In_Frame.data.bytes[0] == 0x03)
			{
				return (In_Frame.data.bytes[2]*256)+In_Frame.data.bytes[3];
			} else
			// Three byte response
			if (In_Frame.data.bytes[0] == 0x04)
			{
				return (In_Frame.data.bytes[2] * 65536) + (In_Frame.data.bytes[3] * 256) + In_Frame.data.bytes[4];
			} else
			// Four byte response
			if (In_Frame.data.bytes[0] == 0x05)
			{
				return (In_Frame.data.bytes[2] * 16777216) + (In_Frame.data.bytes[3] * 65536) + (In_Frame.data.bytes[4] * 256 ) + In_Frame.data.bytes[5];
			}
			WaitingForResponse = false;
		}
	} else
		return -1;
}

/**
 * \brief Calculates the physical value from the Raw CAN value
 * 
 * \param RawResponse
 * \param PageVarNr
 * 
 * \return double
 */
double GetValueFromRawResponse(int32_t RawResponse, int PageVarNr)
{
	return (RawResponse * Pageitems[PageVarNr].Mult) + Pageitems[PageVarNr].Offset;
}

int GetTimeStamp ()
{
	return (millis() - SessionStartTime);
}