/* Vertex.cpp: Monitoring and Control of Vertex outdor tranceiver implementation.
Author: Evgeny A. Kozhevnikov Moscow, Russia  evgeny@kozhevnikov.org

  Revision history:
	25 Nov 2002 - initial creation
	
*/

#include "stdafx.h"
#include "Vertex.h"

const unsigned char STX	=	0x02;		// start transmission byte
const unsigned char ETX =	0x03;		// end transmission byte
const unsigned char BRDCST =0x00;	// broadcast address
const unsigned char ENDBODY = 0x2A;
const unsigned char GET_STATUS =	0x16;
const unsigned char SET_CONFIGURATION =	0x17;
const unsigned char CONTROL =	0x18;
const unsigned char UNIT_STATUS =	0x20;
const unsigned char INVALID_CMD = 0x3F;


// CVertexTranceiver class implementation

CVertexTranceiver::CVertexTranceiver()
{
	m_MCReplyLength = 0;
	m_BusAddress = 0x04;
}

// virtual
CVertexTranceiver::~CVertexTranceiver()
{
}

//virtual
MC_ErrorCode CVertexTranceiver::Identify(BOOL &bIdentified)
{
	bIdentified = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	bIdentified = TRUE;
	return MC_OK;
}

static unsigned char CheckSum(unsigned char *pcTarget, int Start, int Stop)
{
	unsigned char sum = pcTarget[Start];
	for (int i=Start+1; i<=Stop; i++)
		sum += pcTarget[i];
	return sum;
}

static int FillCommandBuffer(unsigned char *pcTarget, LOGICAL bAckFlag, unsigned char address, unsigned char CommandCode, unsigned char *pcDataBytes, unsigned char nDataBytes)
{
	int i=0;
	pcTarget[i++] = ETX; // closing pad byte
	pcTarget[i++] = ETX; // closing pad byte

	pcTarget[i++] = STX;	// opening pad byte
	pcTarget[i++] = 7+nDataBytes; // count
	pcTarget[i++] = address;	// dest/src byte - broadcast
	pcTarget[i] = CommandCode & 0x3F; // set 7th and 8th bits to 0
	if (bAckFlag)
		pcTarget[i] |= 0x40; // set 7th to 1
	++i;

	for (int j=0; j<nDataBytes; j++)
	{
		pcTarget[i++] = pcDataBytes[j]; // data bytes
	}
	pcTarget[i++] = ENDBODY;
	pcTarget[i++] = ::CheckSum(pcTarget, 4, i-1);
	pcTarget[i++] = ETX; // closing pad byte
	return i;
}

int CVertexTranceiver::FillCommandBuffer(unsigned char CommandCode, LOGICAL bAckFlag, unsigned char address, unsigned char *pcDataBytes, unsigned char nDataBytes)
{
	return ::FillCommandBuffer(m_pszCommand, bAckFlag, address, CommandCode, pcDataBytes, nDataBytes);
}

double CVertexTranceiver::RawValueToDouble(unsigned char *pszRawValue)
{
	double value = 0;
	double sign = 1;
	// 1. Skip symbols until <, >, +, -, or any digit
	while (*pszRawValue !=0 && !isdigit(*pszRawValue) &&
		*pszRawValue != '+' && *pszRawValue != '-')
	{
		++pszRawValue;
	}
	if (*pszRawValue == '-')
		sign = -1;
	if (!isdigit(*pszRawValue))
		++pszRawValue;
	int n = sscanf((char *)pszRawValue, "%lf", &value);

	return sign*value;
}

MC_ErrorCode CVertexTranceiver::Command(int CommandLength)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	GetTerminal()->ClearAllBuffers();
	GetTerminal()->Write((char *)m_pszCommand, CommandLength);

	MC_ErrorCode EC = ReadReplyUntilPrompt();
	// Analize error byte
	return EC;
}

static MC_ErrorCode ReadMCPacket(CTerminal *pTerminal, unsigned char *pcTarget, unsigned int &nBytesRead)
{
	unsigned char byte;
	nBytesRead = 0;

	// STX
	if (!pTerminal->Read((char *)&byte, 1))
		return MC_DEVICE_NOT_RESPONDING;
	++nBytesRead;
	if (byte != STX)
		return MC_GENERIC_ERROR;

	// cmd/len
	if (!pTerminal->Read((char *)&byte, 1))
		return MC_DEVICE_NOT_RESPONDING;
	++nBytesRead;
	int nBytesToRead = byte - nBytesRead - 4; // 3: ENDBODY, checksum end ETX

	// address
	if (!pTerminal->Read((char *)&byte, 1))
		return MC_DEVICE_NOT_RESPONDING;
	++nBytesRead;

	// data bytes
	if (!pTerminal->Read((char *)pcTarget, nBytesToRead))
		return MC_DEVICE_NOT_RESPONDING;
	nBytesRead += nBytesToRead;
	
	// end body
	if (!pTerminal->Read((char *)&byte, 1))
		return MC_DEVICE_NOT_RESPONDING;
	if (nBytesToRead == 0)
	{
		if (byte == INVALID_CMD)
			return MC_INVALID_COMMAND;
	}
	if (byte != ENDBODY)
	{
	}
	++nBytesRead;

	// crc
	if (!pTerminal->Read((char *)&byte, 1))
		return MC_DEVICE_NOT_RESPONDING;
	++nBytesRead;
	// TODO: check CRC!

	// ETX	
	if (!pTerminal->Read((char *)&byte, 1))
		return MC_DEVICE_NOT_RESPONDING;
	++nBytesRead;

	return MC_OK;
}

//virtual
MC_ErrorCode CVertexTranceiver::ReadReplyUntilPrompt()
{
	memset(m_pszReply, 0, sizeof(m_pszReply));
	return ::ReadMCPacket(GetTerminal(), m_pszReply, m_MCReplyLength);
}

//virtual
void CVertexTranceiver::SetMCBusAddress(unsigned int Address)
{
	m_BusAddress = LOBYTE(LOWORD(Address));
}
//virtual
MC_ErrorCode CVertexTranceiver::GetDeviceVersionString(char *&pszString)
{
	pszString = "Vertex tranceiver";
	return MC_OK;
}

//virtual
MC_ErrorCode CVertexTranceiver::IsTemperatureAlarm(BOOL &bAlarm)
{
	return MC_OK;
}

//virtual
MC_ErrorCode CVertexTranceiver::GetTemperature(double &T)
{
	T = -273.15;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	unsigned char MessageData[] = { 0x51 };
	int CommandLength = FillCommandBuffer(GET_STATUS, LOGICAL_FALSE, m_BusAddress, MessageData, sizeof(MessageData));
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != GET_STATUS || m_pszReply[1] != MessageData[0])
		return MC_GENERIC_ERROR;  // invalid database element identifier
	T = RawValueToDouble(m_pszReply+2);
	return MC_OK;
}

//virtual
MC_ErrorCode CVertexTranceiver::TurnOutputOn(BOOL &bOn)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	unsigned char MessageData[] = { 0x50, 0x00 };
	if (bOn)
		MessageData[1] = 0x31;
	else
		MessageData[1] = 0x30;
	int CommandLength = FillCommandBuffer(CONTROL, LOGICAL_TRUE, m_BusAddress, MessageData, sizeof(MessageData));
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	return IsOutputOn(bOn);
}

//virtual
MC_ErrorCode CVertexTranceiver::IsOutputOn(BOOL &bOn)
{
	bOn = FALSE;
	unsigned char MessageData[] = { 0x50 };
	int CommandLength = FillCommandBuffer(CONTROL, LOGICAL_FALSE, m_BusAddress, MessageData, sizeof(MessageData));
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != CONTROL || m_pszReply[1] != MessageData[0])
		return MC_GENERIC_ERROR;  // invalid database element identifier
	if (m_pszReply[2] == 0x31)
		bOn = TRUE;
	else if (m_pszReply[2] == 0x30)
		bOn = FALSE;
	else
	{
		ASSERT(FALSE);
	}
	
	return MC_OK;
}

//virtual
MC_ErrorCode CVertexTranceiver::GetPower(double &Power)
{
	Power = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	unsigned char MessageData[] = { 0x50 };
	int CommandLength = FillCommandBuffer(GET_STATUS, LOGICAL_FALSE, m_BusAddress, MessageData, sizeof(MessageData));
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != GET_STATUS || m_pszReply[1] != MessageData[0])
		return MC_GENERIC_ERROR;  // invalid database element identifier
	Power = RawValueToDouble(m_pszReply+2);
	return MC_OK;
}

// Frequency


// Gain
//virtual
MC_ErrorCode CVertexTranceiver::GetGain(double &Gain)
{
	Gain = 0.;
	unsigned char MessageData[] = { 0x52 };
	int CommandLength = FillCommandBuffer(CONTROL, LOGICAL_FALSE, m_BusAddress, MessageData, sizeof(MessageData));
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != CONTROL || m_pszReply[1] != MessageData[0])
		return MC_GENERIC_ERROR;  // invalid database element identifier
	Gain = RawValueToDouble(m_pszReply+2);
	
	return MC_OK;
}

//virtual
MC_ErrorCode CVertexTranceiver::SetGain(double &Gain)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	unsigned char MessageData[] = { 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	char *pszValue = (char *)MessageData+1;
	sprintf(pszValue, "%+.1f", Gain);
	int CommandLength = FillCommandBuffer(CONTROL, LOGICAL_TRUE, m_BusAddress, MessageData, 1+strlen(pszValue));
	MC_ErrorCode EC = Command(CommandLength);
	GetGain(Gain);
	return EC;
}


// CVertex50W class implementation

char *CVertex50W::m_pszName = "Vertex 50W tranceiver";

CVertex50W::CVertex50W()
{
}

// virtual
CVertex50W::~CVertex50W()
{
}


// CVertexSubstitutor class implementation

char *CVertexSubstitutor::m_pszName = "Vertex substitutor";

CVertexSubstitutor::CVertexSubstitutor()
{
	m_bAlarm = FALSE;
}

//virtual
CVertexSubstitutor::~CVertexSubstitutor()
{
}

//virtual
void CVertexSubstitutor::OnThreadStarted()
{
	TRACE("CVertexSubstitutor::ThreadProcedure started\n");
}

//virtual
LOGICAL CVertexSubstitutor::ThreadBody()
{
	ReadRequestSendReply();
	portableSwitchContext();
	return LOGICAL_TRUE;
}

//virtual
void CVertexSubstitutor::OnThreadEnded()
{
	TRACE("CVertex50WSubstitutor::thread procedure ended\n");
}

BOOL CVertexSubstitutor::ReadRequestSendReply()
{
	memset(m_pcRequest, 0, sizeof(m_pcRequest));
	memset(m_pcReply, 0, sizeof(m_pcReply));

	static unsigned char Data[6] = { 0x02, 0x06, 0x04, 0x3F, 0x43, 0x03 };
//	memset(Data, 0, sizeof(Data));
	int CommandLength = 6;
	unsigned int nBytesInRequest;
	if (::ReadMCPacket(GetTerminal(), m_pcRequest, nBytesInRequest))
		return FALSE;
	TRACE("\n");
	for (unsigned int i = 0; i < nBytesInRequest; i++)
	{
		TRACE(" 0x%X", m_pcRequest[i]);
	}
	TRACE("\n");
/*	if (m_pcRequest[0] == 0x06 && m_pcRequest[1] == 0x06)
	{ // Temperature
		Data[0] = 0x06; Data[1] = 0x06; // DB element identifier
		Data[2] = 0x01;
		if (m_bAlarm)
			Data[3] = 0x43; // +50 Celsius
		else
			Data[3] = 0x02; // -15 Celsius
		CommandLength = FillCommandBuffer(m_pcReply, UPD, Data, 4);
	}
	else if (m_pcRequest[0] == 0x17 && m_pcRequest[1] == 0xFF)
	{ // Output power
		Data[0] = 0x17; Data[1] = 0xFF; // DB element identifier
		Data[2] = 0x01; Data[3] = 0x2C; // 30 dBm
		CommandLength = FillCommandBuffer(m_pcReply, UPD, Data, 4);
	}
	else if (m_pcRequest[0] == 0x18 && m_pcRequest[1] == 0xFF)
	{ // Gain variation
		Data[0] = 0x18; Data[1] = 0xFF; // DB element identifier
		Data[2] = 0x02; Data[3] = 0x08; // 52 dBm
		CommandLength = FillCommandBuffer(m_pcReply, UPD, Data, 4);
	}
	else if (m_pcRequest[0] == 0x16 && m_pcRequest[1] == 0xFF)
	{ // Frequency
		Data[0] = 0x16; Data[1] = 0xFF; // DB element identifier
		Data[2] = 0x03; Data[3] = 0xB6; // 950 MHz
		CommandLength = FillCommandBuffer(m_pcReply, UPD, Data, 4);
	}
	else if (m_pcRequest[0] == 0x00 && m_pcRequest[1] == 0x02)
	{ // Temperature alarm
		Data[0] = 0x00; Data[1] = 0x02; // DB element identifier
		Data[2] = 0x00;
		if (m_bAlarm)
			Data[3] = 0x01; // Alarm
		else
			Data[3] = 0x00; // No alarm
		m_bAlarm = !m_bAlarm;
		CommandLength = FillCommandBuffer(m_pcReply, UPD, Data, 4);
	}
	else if (m_pcRequest[0] == 0x06 && m_pcRequest[1] == 0x01)
	{ // Mute status
		Data[0] = 0x06; Data[1] = 0x01; // DB element identifier
		Data[2] = 0x00; Data[3] = 0x01; // Muted
		CommandLength = FillCommandBuffer(m_pcReply, UPD, Data, 4);
	}*/

	if (CommandLength)
		GetTerminal()->Write((char *)Data, CommandLength);

	return TRUE;
}

//virtual
MC_ErrorCode CVertexSubstitutor::GetDeviceVersionString(char *&pszString)
{
	pszString = "Software device emulator for Vertex outdoor equipment substitution";
	return MC_OK;
}

//virtual
BOOL CVertexSubstitutor::Start()
{
	return Run();
}

//virtual
BOOL CVertexSubstitutor::Stop()
{
	return CThreadObject::Stop();
}
