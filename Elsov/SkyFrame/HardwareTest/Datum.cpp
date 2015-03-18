#include "stdafx.h"
#include "Datum.h"

static char CR=0x0D;
static char LF=0x0A;
static char szCRLF[]={CR, LF};

// CDatumModem implementation

CDatumModem::CDatumModem()
{
	m_bParamSavingInNvRamRightAfterSettingEnabled = FALSE;
	m_ModemAddress = 12;
	m_ControllerAddress = 0x01;
	m_cStatusByte = 0;
	m_cErrorByte = 0;
	m_cDataBytes = 0;
	m_ReturnedDataLength = 0;
}

CDatumModem::~CDatumModem()
{
}

void CDatumModem::WriteLog(unsigned char *pszString, int length)
{
	if (!IsLogFileEnabled()) return;
}

//virtual
CTerminal *CDatumModem::GetTerminal()
{
	switch (m_ControlType)
	{
	case MC_UNKNOWN_TYPE:
	case MC_SERIAL_PORT:
		return &m_SerialPort;
	case MC_TELNET:
		return &m_TcpIpSocket;
	case MC_SNMP:
		return NULL;
	}
	return NULL;
}

//virtual
void  CDatumModem::OnMcConnected()
{
}

MC_ErrorCode CDatumModem::Command(int CommandLength, BOOL bWaitForReply/* = TRUE */)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	GetTerminal()->ClearAllBuffers();
	GetTerminal()->Write((char *)m_pszCommand, CommandLength);
	if (!bWaitForReply)
		return MC_OK;

	MC_ErrorCode EC = ReadReplyUntilPrompt();
	GetTerminal()->ClearAllBuffers();

	return EC;
}

void CDatumModem::SetMCBusAddress(unsigned int Address)
{
	m_ModemAddress = LOBYTE(LOWORD(Address));
}

//virtual
BOOL CDatumModem::CheckMCBusAddress(unsigned int Address)
{
	if (Address < GetMinMCBusAddressScanning() ||
		Address > GetMaxMCBusAddressScanning())
		return FALSE;
	if (!IsControllable())
		return FALSE;

	BOOL bMyAddress = FALSE;
	unsigned int freq = 0;
	unsigned int StoredAddress = GetMCBusAddress();
	SetMCBusAddress(Address);
	MC_ErrorCode EC = GetRFrequency(freq, 1);
	if (EC == MC_OK)
		bMyAddress = TRUE;
	SetMCBusAddress(StoredAddress);
	return bMyAddress;
}

unsigned int CDatumModem::RawDataToInt(unsigned char *pszRawData)
{
	// Intel byte order!!! I do not know what byte order is on Motorola processor. Faik, please care about it! Zhenya
	unsigned int IntValue = *((unsigned int *)pszRawData);
	return IntValue;
}

short CDatumModem::RawDataToSignedShort(unsigned char *pszRawData)
{
	// Intel byte order!!! I do not know what byte order is on Motorola processor. Faik, please care about it! Zhenya
	short ShortValue = *((short *)pszRawData);
	return ShortValue;
}

void CDatumModem::IntToRawData(unsigned int IntValue, unsigned char *pszRawData)
{
	// Intel byte order!!! I do not know what byte order is on Motorola processor. Faik, please care about it! Zhenya
	pszRawData[0] = (unsigned char)((IntValue & 0x000000FF) >> 0);
	pszRawData[1] = (unsigned char)((IntValue & 0x0000FF00) >> 8);
	pszRawData[2] = (unsigned char)((IntValue & 0x00FF0000) >> 16);
	pszRawData[3] = (unsigned char)((IntValue & 0xFF000000) >> 24);
}

void CDatumModem::SignedShortToRawData(short ShortValue, unsigned char *pszRawData)
{
	// Intel byte order!!! I do not know what byte order is on Motorola processor. Faik, please care about it! Zhenya
	memcpy(pszRawData, &ShortValue, sizeof(ShortValue));
}

// Common device commands
// 10 MHz reference type
// Frequency commands
// Modulation Type commands
// Output
// Fec Type commands
// Data clock source
// Data commands

// BER

double CDatumModem::ParseBER()
{
	unsigned short ber = *((unsigned short *)(m_pDataBytes+16));
	int mantissa = ber & 0x0FFF;			// bits 0...11
	int NegativeExponent = ber & 0xF000;	// bits 12...15
	NegativeExponent >>= 12;
	double BER = (double)mantissa;
	for (int i = 0; i < NegativeExponent; i++)
		BER /= 10.;

	return BER;
}

double CDatumModem::ParseSER()
{
	unsigned short ser = *((unsigned short *)(m_pDataBytes+18));
	int mantissa = ser & 0x0FFF;			// bits 0...11
	int NegativeExponent = ser & 0xF000;	// bits 12...15
	NegativeExponent >>= 12;
	double SER = (double)mantissa;
	for (int i = 0; i < NegativeExponent; i++)
		SER /= 10.;

	return SER;
}

// Differential encoding/decoding commands
// Scrambling/Descrambling
// Spectral inversion
// Data inversion



