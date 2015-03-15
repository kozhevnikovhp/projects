/* Wavesat.cpp: Monitoring and Control of WaveSat outdor tranceiver implementation.
Author: Evgeny A. Kozhevnikov Moscow, Russia  ek538@mail.ru

  Revision history:
	01 September 2002 - initial creation
	02 September 2003 - M&C substitutor
	
*/

#include "stdafx.h"
#include "WaveSat.h"

const unsigned char STX	=	0x7E;		// start transmission byte
const unsigned char ETX =	0x7F;		// end transmission byte
const unsigned char BRDCST =0xFF;	// broadcast address - dest=0xF and src=0xF
const unsigned char GET =	0x00;
const unsigned char SET =	0x01;
const unsigned char UPD =	0x08;
const unsigned char ACK =	0x0E;
const unsigned char NACK =	0x0F;


// CWaveSatTranceiver class implementation

CWaveSatTranceiver::CWaveSatTranceiver()
{
	m_MCReplyLength = 0;
}

// virtual
CWaveSatTranceiver::~CWaveSatTranceiver()
{
}

//virtual
MC_ErrorCode CWaveSatTranceiver::Identify(BOOL &bIdentified)
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
		sum ^= pcTarget[i];
	return ~sum;
}

static int FillCommandBuffer(unsigned char *pcTarget, unsigned char CommandCode, unsigned char *pcDataBytes, unsigned char nDataBytes)
{
	int i=0;
	pcTarget[i++] = STX;	// opening pad byte
	pcTarget[i++] = BRDCST;	// dest/src byte - broadcast
	pcTarget[i++] = CommandCode << 4 | nDataBytes;  // cmd occupies high nibble and len occupies low one 

	for (int j=0; j<nDataBytes; j++)
	{
		pcTarget[i++] = pcDataBytes[j]; // data bytes
	}
	pcTarget[i++] = ::CheckSum(pcTarget, 1, i-1);
	pcTarget[i++] = ETX; // closing pad byte
	return i;
}

int CWaveSatTranceiver::FillCommandBuffer(unsigned char CommandCode, unsigned char *pcDataBytes, unsigned char nDataBytes)
{
	return ::FillCommandBuffer(m_pszCommand, CommandCode, pcDataBytes, nDataBytes);
}

short CWaveSatTranceiver::RawValueToShort(unsigned char *pszRawValue)
{
	short ret = pszRawValue[0];
	ret <<= 8;
	ret += pszRawValue[1];
	return ret;
}

MC_ErrorCode CWaveSatTranceiver::Command(int CommandLength)
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

	// dest/src
	if (!pTerminal->Read((char *)&byte, 1))
		return MC_DEVICE_NOT_RESPONDING;
	++nBytesRead;

	// cmd/len
	if (!pTerminal->Read((char *)&byte, 1))
		return MC_DEVICE_NOT_RESPONDING;
	++nBytesRead;
	unsigned char len = byte & 0x0F;

	// data bytes
	if (!pTerminal->Read((char *)pcTarget, len))
		return MC_DEVICE_NOT_RESPONDING;
	nBytesRead += len;
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
MC_ErrorCode CWaveSatTranceiver::ReadReplyUntilPrompt()
{
	memset(m_pszReply, 0, sizeof(m_pszReply));
	return ::ReadMCPacket(GetTerminal(), m_pszReply, m_MCReplyLength);

}

//virtual
MC_ErrorCode CWaveSatTranceiver::GetDeviceVersionString(char *&pszString)
{
	pszString = "Wavesat tranceiver";
	return MC_OK;
}

//virtual
MC_ErrorCode CWaveSatTranceiver::IsTemperatureAlarm(BOOL &bAlarm)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	unsigned char DatabaseIdentifier[] = { 0x00, 0x02 };
	int CommandLength = FillCommandBuffer(GET, DatabaseIdentifier, 2);
	MC_ErrorCode EC = Command(CommandLength);
	bAlarm = FALSE;
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != 0x00 || m_pszReply[1] != 0x02)
		return MC_GENERIC_ERROR;  // invalid database element identifier
	if (m_pszReply[3])
		bAlarm = TRUE;
	return EC;
}

//virtual
MC_ErrorCode CWaveSatTranceiver::GetTemperature(double &T)
{
	T = -273.15;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	unsigned char DatabaseIdentifier[] = { 0x06, 0x06 };
	int CommandLength = FillCommandBuffer(GET, DatabaseIdentifier, 2);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != 0x06 || m_pszReply[1] != 0x06)
		return MC_GENERIC_ERROR;  // invalid database element identifier
	T = RawValueToShort(m_pszReply+2)-273;
	return MC_OK;
}

//virtual
MC_ErrorCode CWaveSatTranceiver::TurnOutputOn(BOOL &bOn)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	unsigned char Data[] = { 0x13, 0x01, 0x00, 0x01 };
	if (bOn)
		Data[3] = 0x00; // unmute
	int CommandLength = FillCommandBuffer(SET, Data, 4);
	MC_ErrorCode EC = Command(CommandLength);

	EC = IsOutputOn(bOn);
	return EC;
}

//virtual
MC_ErrorCode CWaveSatTranceiver::IsOutputOn(BOOL &bOn)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	unsigned char DatabaseIdentifier[] = { 0x06, 0x01 };
	int CommandLength = FillCommandBuffer(GET, DatabaseIdentifier, 2);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != 0x06 || m_pszReply[1] != 0x01)
		return MC_GENERIC_ERROR;  // invalid database element identifier
	bOn = TRUE;
	if (m_pszReply[3] != 0x00)
		bOn = FALSE;
	return MC_OK;
}

//virtual
MC_ErrorCode CWaveSatTranceiver::GetPower(double &Power)
{
	Power = 0.;
	unsigned char DatabaseIdentifier[] = { 0x17, 0xFF };
	int CommandLength = FillCommandBuffer(GET, DatabaseIdentifier, 2);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != 0x17 || m_pszReply[1] != 0xFF)
		return MC_GENERIC_ERROR;  // invalid database element identifier
	Power = RawValueToShort(m_pszReply+2)/10.;
	return MC_OK;
}

// Frequency

//virtual
MC_ErrorCode CWaveSatTranceiver::GetFrequency(unsigned int &Frequency)
{
	Frequency = 0;
	unsigned char DatabaseIdentifier[] = { 0x16, 0xFF };
	int CommandLength = FillCommandBuffer(GET, DatabaseIdentifier, 2);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != 0x16 || m_pszReply[1] != 0xFF)
		return MC_GENERIC_ERROR;  // invalid database element identifier
	Frequency = RawValueToShort(m_pszReply+2);
	return MC_OK;
}

//virtual
MC_ErrorCode CWaveSatTranceiver::SetFrequency(unsigned int &Frequency)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	unsigned char Data[] = { 0x16, 0xFF, 0x00, 0x00 };
	unsigned short sFreq = LOWORD(Frequency);
	Data[2] = HIBYTE(sFreq);
	Data[3] = LOBYTE(sFreq);
	int CommandLength = FillCommandBuffer(SET, Data, 4);
	MC_ErrorCode EC = Command(CommandLength);

//	EC = GetFrequency(Frequency);
	return EC;
}

// Gain
//virtual
MC_ErrorCode CWaveSatTranceiver::GetGain(double &Gain)
{
	Gain = 0.;
	unsigned char DatabaseIdentifier[] = { 0x18, 0xFF };
	int CommandLength = FillCommandBuffer(GET, DatabaseIdentifier, 2);
	MC_ErrorCode EC = Command(CommandLength);
	if (EC != MC_OK)
		return EC;
	if (m_pszReply[0] != 0x18 || m_pszReply[1] != 0xFF)
		return MC_GENERIC_ERROR;  // invalid database element identifier
	Gain = RawValueToShort(m_pszReply+2)/10.;
	return MC_OK;
}


// CWaveSatWtr70 class implementation

char *CWaveSatWtr70::m_pszName = "WaveSat WTR-70 tranceiver";

CWaveSatWtr70::CWaveSatWtr70()
{
}

// virtual
CWaveSatWtr70::~CWaveSatWtr70()
{
}


// CWaveSatSubstitutor class implementation

char *CWaveSatSubstitutor::m_pszName = "WaveSat substitutor";

CWaveSatSubstitutor::CWaveSatSubstitutor()
{
	m_bAlarm = FALSE;
}

//virtual
CWaveSatSubstitutor::~CWaveSatSubstitutor()
{
}

//virtual
void CWaveSatSubstitutor::OnThreadStarted()
{
	TRACE("CWaveSatSubstitutor::ThreadProcedure started\n");
}

//virtual
LOGICAL CWaveSatSubstitutor::ThreadBody()
{
	ReadRequestSendReply();
	portableSwitchContext();
	return LOGICAL_TRUE;
}

//virtual
void CWaveSatSubstitutor::OnThreadEnded()
{
	TRACE("CWaveSatSubstitutor::thread procedure ended\n");
}

BOOL CWaveSatSubstitutor::ReadRequestSendReply()
{
	memset(m_pcRequest, 0, sizeof(m_pcRequest));
	memset(m_pcReply, 0, sizeof(m_pcReply));
	static unsigned char Data[4];
	memset(Data, 0, sizeof(Data));
	int CommandLength = 0;
	unsigned int nBytesInRequest;
	if (::ReadMCPacket(GetTerminal(), m_pcRequest, nBytesInRequest))
		return FALSE;
	if (m_pcRequest[0] == 0x06 && m_pcRequest[1] == 0x06)
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
	}
	if (CommandLength)
		GetTerminal()->Write((char *)m_pcReply, CommandLength);

	return TRUE;
}

//virtual
MC_ErrorCode CWaveSatSubstitutor::GetDeviceVersionString(char *&pszString)
{
	pszString = "Software device emulator for WaveSat outdoor equipment substitution";
	return MC_OK;
}

//virtual
BOOL CWaveSatSubstitutor::Start()
{
	return Run();
}

//virtual
BOOL CWaveSatSubstitutor::Stop()
{
	return CThreadObject::Stop();
}
