#include "stdafx.h"
#include "Advantech.h"

static char CR=0x0D;
static char LF=0x0A;
static char szCRLF[]={CR, LF, 0};

// CAdvantechTranceiver class implementation

static unsigned char StatusRequestDataBytes[] = { 0xAA, 0xAA, 0xAA, 0xAA };

CAdvantechTranceiver::CAdvantechTranceiver()
{
	m_Frequency = 0;
	m_MCReplyLength = 0;
}

// virtual
CAdvantechTranceiver::~CAdvantechTranceiver()
{
}

//virtual
MC_ErrorCode CAdvantechTranceiver::Identify(BOOL &bIdentified)
{
	bIdentified = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	bIdentified = TRUE;
	return MC_OK;
}

void CAdvantechTranceiver::FillCommandBuffer(unsigned char CommandCode, unsigned char *pcDataBytes)
{
	memset(m_pszCommand, 0, sizeof(m_pszCommand));
	m_pszCommand[0] = 0x01;	// address
	m_pszCommand[1] = CommandCode;	// command code
	memcpy(m_pszCommand+2, pcDataBytes, 4);
	m_pszCommand[6] = CheckSum(0, 5);
}

unsigned char CAdvantechTranceiver::CheckSum(int Start, int Stop)
{
	unsigned char sum = m_pszCommand[Start];
	for (int i=Start+1; i<=Stop; i++)
		sum += m_pszCommand[i];
	return sum;
}

short CAdvantechTranceiver::RawValueToShort(unsigned char *pszRawValue)
{
	short ret = pszRawValue[0];
	ret <<= 8;
	ret += pszRawValue[1];
	return ret;
}

MC_ErrorCode CAdvantechTranceiver::Command()
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	GetTerminal()->ClearAllBuffers();
	GetTerminal()->Write((char *)m_pszCommand, 7);

	MC_ErrorCode EC = ReadReplyUntilPrompt();

	return EC;
}

//virtual
MC_ErrorCode CAdvantechTranceiver::ReadReplyUntilPrompt()
{
	memset(m_pszReply, 0, sizeof(m_pszReply));
	m_MCReplyLength = 0;

	if (!GetTerminal()->Read((char *)m_pszReply, 7))
		return MC_DEVICE_NOT_RESPONDING;
	m_MCReplyLength = 7;
	return MC_OK;
}

//virtual
MC_ErrorCode CAdvantechTranceiver::GetDeviceVersionString(char *&pszString)
{
	static char szVersion[64];
	FillCommandBuffer(REQ, StatusRequestDataBytes);
	MC_ErrorCode EC = Command();
	if (EC == MC_OK)
	{
		unsigned char cStatusByte2 = m_pszReply[5];
		unsigned char version = cStatusByte2 & 0xF0;
		version >>= 4;
		unsigned char cStatusByte1 = m_pszReply[4];
		unsigned char cPowerClass = cStatusByte1 & 0xF0;
		cPowerClass >>= 4;
		int PowerClass = 0;
		switch (cPowerClass)
		{
		case 0x01: PowerClass = 2; break;
		case 0x02: PowerClass = 4; break;
		case 0x03: PowerClass = 5; break;
		case 0x04: PowerClass = 8; break;
		case 0x05: PowerClass = 10; break;
		case 0x06: PowerClass = 16; break;
		case 0x07: PowerClass = 20; break;
		case 0x08: PowerClass = 25; break;
		case 0x09: PowerClass = 40; break;
		case 0x0A: PowerClass = 60; break;
		default: PowerClass = 0;
		}
		sprintf(szVersion, "Power Class : %d Watt%sSoftware version 0x%02X", PowerClass, szCRLF, version);
		pszString = szVersion;
	}
	else
	{
		pszString = "M&C error!";
	}
	return MC_OK;
}

//virtual
MC_ErrorCode CAdvantechTranceiver::IsTemperatureAlarm(BOOL &bAlarm)
{
	bAlarm = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	FillCommandBuffer(REQ, StatusRequestDataBytes);
	Command();
	if (m_pszReply[4] & 1)
		bAlarm = TRUE;

	return MC_OK;
}

//virtual
MC_ErrorCode CAdvantechTranceiver::GetTemperature(double &T)
{
	T = -273.15;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	FillCommandBuffer(REQ, StatusRequestDataBytes);
	Command();
	T = m_pszReply[3];

	return MC_OK;
}

//virtual
MC_ErrorCode CAdvantechTranceiver::TurnOutputOn(BOOL &bOn)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	unsigned char DataBytes[] = { 0x00, 0xAA, 0xAA, 0xAA };
	if (bOn)
		DataBytes[0] = 0x01;
	FillCommandBuffer(ON_OFF, DataBytes);
	MC_ErrorCode EC = Command();
	EC = IsOutputOn(bOn);
	return EC;
}

//virtual
MC_ErrorCode CAdvantechTranceiver::IsOutputOn(BOOL &bOn)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	FillCommandBuffer(REQ, StatusRequestDataBytes);
	MC_ErrorCode EC = Command();

	bOn = FALSE;
	if (m_pszReply[4] & 1<<3)
		bOn = TRUE;
	
	return EC;
}

//virtual
MC_ErrorCode CAdvantechTranceiver::GetPower(double &Power)
{
	Power = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	FillCommandBuffer(REQ, StatusRequestDataBytes);
	MC_ErrorCode EC = Command();

	Power = m_pszReply[1]<<8;
	Power += m_pszReply[2];
	Power /= 100.;  // Advantech operates with 1/100 dBm
	return EC;
}
// Frequency

//virtual
MC_ErrorCode CAdvantechTranceiver::GetFrequency(unsigned int &Frequency)
{
	Frequency = m_Frequency;
	return MC_OK;
}

//virtual
MC_ErrorCode CAdvantechTranceiver::SetFrequency(unsigned int &Frequency)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	m_Frequency = Frequency;
	unsigned char DataBytes[] = { 0x00, 0x00, 0xAA, 0xAA };
	DataBytes[0] = HIBYTE(LOWORD(Frequency));
	DataBytes[1] = LOBYTE(LOWORD(Frequency));
	FillCommandBuffer(FREQ, DataBytes);
	MC_ErrorCode EC = Command();
	return EC;
}


// CAdvantechWty class implementation

char *CAdvantechWty1::m_pszName = "Advantech WTY-1 tranceiver";

CAdvantechWty1::CAdvantechWty1()
{
}

// virtual
CAdvantechWty1::~CAdvantechWty1()
{
}
