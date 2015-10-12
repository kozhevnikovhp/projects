#include "stdafx.h"
#include "Datum_M7.h"

static char CR = 0x0D;
static char LF = 0x0A;
static char szCRLF[] = {CR, LF};

//////////////////////////////////////////////////////////////////////
//
// 

static const char *DATUM_M7_MODULATION_TYPES[] = {
	"BPSK",
	"QPSK",
	"OQPSK",
	"8PSK",
	"8QAM",
	"Unknown",
	"16QAM"
};

static const char *DATUM_M7_FEC_MODES[] = {
	"Viterbi 1/2",
	"Viterbi 2/3",
	"Viterbi 3/4",
	"Viterbi 7/8",
	"Viterbi 14/17",
	"Viterbi 10/11",
	"Viterbi 16/17"
};

static const char *DATUM_M7_SCRAMBLER_MODES[] = {
	"Disabled",		// 0
	"Auto",			// 1
	"V.35",			// 2
	"Intelsat",		// 3
	"EFD",			// 4
	"Alt V.35",		// 5
	"Alt Intelsat",	// 6
	"",				// 7
	"FEC Sync"		// 8
};

static const char *DATUM_M7_10MHZ_MODES[] = { "Disabled", "Enabled" };

static const char *DATUM_M7_DIFF_CODING_MODES[] = { "Disabled", "Enabled" };

static const char *DATUM_M7_R_CLOCK_SOURCES[] = {"RCV_Clock", "Internal", "External", "Mod_Clock"};
static const char *DATUM_M7_T_CLOCK_SOURCES[] = {"Internal", "TT_Clock", "External", "RCV_Clock"};

static const char *DATUM_M7_R_SWEEP_MODES[] = { "Normal (fast)", "Search" };
static const char *DATUM_M7_R_POWER_SUPPLY_MODES[] = { "None", "13 Volts", "18 Volts" };
static const char *DATUM_M7_T_POWER_SUPPLY_MODES[] = { "None", "24 Volts" };

static const char *DATUM_M7_REED_SOLOMON_MODES[] = { "Disabled", "IESS-308", "IESS-309", "IESS-310", "Custom", "CT220,200" };

CDatum_M7::CDatum_M7()
{
}

//virtual
CDatum_M7::~CDatum_M7()
{
}

unsigned short CRC_CCITT_BYTE(unsigned char byte)
{
	unsigned short crc[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
	};

	return crc[byte];
}

unsigned short CRC_CCITT(unsigned char *pBuffer, size_t len)
{
	unsigned short crcValue=0xFFFF;
	for (int i = 0; i < len; i++)
	{
		unsigned short tmp = crcValue ^ pBuffer[i];
		crcValue = (crcValue >> 8) ^ CRC_CCITT_BYTE(tmp & 0xFF);
	}
	return crcValue;
}

//virtual
MC_ErrorCode CDatum_M7::ReadReplyUntilPrompt()
{
	unsigned char byte;
	bool bPad = false;
	m_ReturnedDataLength = 0;
	m_RawReplyLength = 0;

	while (!bPad)
	{
		byte = 0x00;
		if (!GetTerminal()->Read((char *)&byte, 1))
			return MC_DEVICE_NOT_RESPONDING;
		if (byte == 0x5A)
		{
			m_pRawReply[m_RawReplyLength++] = byte;
			bPad = true;
			break;
		}
	}
	if (!bPad)
		return MC_DEVICE_NOT_RESPONDING;

	// Addresses
	LOGICAL b = GetTerminal()->Read(m_pRawReply+m_RawReplyLength, 3);
	if (!b)
		return MC_DEVICE_NOT_RESPONDING;
	if (m_pRawReply[m_RawReplyLength] != m_ControllerAddress)
		return MC_GENERIC_ERROR;
	if (m_pRawReply[m_RawReplyLength+1] != m_ModemAddress)
		return MC_GENERIC_ERROR;
	if (m_pRawReply[m_RawReplyLength+2] != 0)
		return MC_GENERIC_ERROR;
	m_RawReplyLength += 3;

	// total byte count
	b = GetTerminal()->Read(m_pRawReply+m_RawReplyLength, 3);
	if (!b)
		return MC_DEVICE_NOT_RESPONDING;
	m_RawReplyLength += 3;

	// Slot
	b = GetTerminal()->Read(m_pRawReply+m_RawReplyLength, 1);
	if (!b)
		return MC_DEVICE_NOT_RESPONDING;
	m_RawReplyLength += 1;

	// Command number
	b = GetTerminal()->Read(m_pRawReply+m_RawReplyLength, 1);
	if (!b)
		return MC_DEVICE_NOT_RESPONDING;
	m_RawReplyLength += 1;

	// Error code
	b = GetTerminal()->Read(m_pRawReply+m_RawReplyLength, 1);
	if (!b)
		return MC_DEVICE_NOT_RESPONDING;
	m_RawReplyLength += 1;

	// Byte counter
	b = GetTerminal()->Read(m_pRawReply+m_RawReplyLength, 1);
	if (!b)
		return MC_DEVICE_NOT_RESPONDING;
	int nDataBytes = m_pRawReply[m_RawReplyLength];
	m_RawReplyLength += 1;

	// Data bytes
	b = GetTerminal()->Read(m_pRawReply+m_RawReplyLength, nDataBytes);
	if (!b)
		return MC_DEVICE_NOT_RESPONDING;
	m_RawReplyLength += nDataBytes;

	unsigned short CRC_received; 
	if (!GetTerminal()->Read(&CRC_received, 2))
		return MC_DEVICE_NOT_RESPONDING;

	unsigned short CRC_calculated = CRC_CCITT(m_pRawReply, m_RawReplyLength);
	if (CRC_calculated != CRC_received)
		return MC_BAD_CRC;

	return MC_OK;
}

//virtual
MC_ErrorCode CDatum_M7::GetDeviceVersionString(char *&pszString)
{
	pszString = m_szVersion;
	m_szVersion[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	std::string result;
	// Model
	result += "Model : ";
	std::string str;
	MC_ErrorCode EC = GetModelName(str);
	result += str;
	result += szCRLF;
	
	// Software version
	result += "Software version : ";
	EC = GetSoftwareVersion(str);
	result += str;
	result += szCRLF;


	// Serial number
	result += "SerialNo : ";
	unsigned int SN;
	EC = GetSerialNo(SN);
	char szSerial[16];
	sprintf(szSerial, "%d", SN);
	result += szSerial;
	result += szCRLF;

	result += szCRLF;
	// Slot1
	result += "Slot 1 : ";
	EC = getStringParam(1, 13, str);
	result += str;
	result += " ";
	EC = getStringParam(0, 10, str);
	result += str;
	result += szCRLF;

	// Slot2
	result += "Slot 2 : ";
	EC = getStringParam(2, 13, str);
	result += str;
	result += " ";
	EC = getStringParam(0, 11, str);
	result += str;
	result += szCRLF;

	// Slot3
	result += "Slot 3 : ";
	EC = getStringParam(3, 13, str);
	result += str;
	result += " ";
	EC = getStringParam(0, 12, str);
	result += str;
	result += szCRLF;

	// Slot4
	result += "Slot 4 : ";
	EC = getStringParam(4, 13, str);
	result += str;
	result += " ";
	EC = getStringParam(0, 13, str);
	result += str;
	result += szCRLF;

	strcpy(m_szVersion, result.c_str());
	return EC;
}

//	virtual MC_ErrorCode GetModulatorVersionString(char *&pszString, int Modulator) { return GetDeviceVersionString(pszString); }
//	virtual MC_ErrorCode GetDemodulatorVersionString(char *&pszString, int Demodulator) { return GetDeviceVersionString(pszString); }
//virtual
MC_ErrorCode CDatum_M7::GetUnitName(char *&pszString)
{
	pszString = m_szUnitName;
	m_szUnitName[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	std::string result;
	MC_ErrorCode EC = getStringParam(0, 4, result);
	strcpy(m_szUnitName, result.c_str());	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetUnitName(char *pszString)
{
	MC_ErrorCode EC = setStringParam(0, 4, pszString);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetModelName(std::string &strModel)
{
	MC_ErrorCode EC = getStringParam(0, 8, strModel);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetSerialNo(unsigned int &serialNo)
{
	MC_ErrorCode EC = getUnsignedInt32Param(0, 15, serialNo);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetSoftwareVersion(std::string &strVersion)
{
	MC_ErrorCode EC = getStringParam(0, 14, strVersion);
	return EC;
}


//virtual
MC_ErrorCode CDatum_M7::GetRFrequency(unsigned int &Frequency, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getUnsignedInt32Param(getDemodulatorSlotNumber(Demodulator), 17, Frequency);
	Frequency /= 1000;
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetRFrequency(unsigned int &Frequency, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFrequency(Frequency, Demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setUnsignedInt32Param(getDemodulatorSlotNumber(Demodulator), 17, Frequency*1000);
	if (EC != MC_OK)
		return EC;

	GetRFrequency(Frequency, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetTFrequency(unsigned int &Frequency, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getUnsignedInt32Param(getModulatorSlotNumber(Modulator), 17, Frequency);
	Frequency /= 1000;
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetTFrequency(unsigned int &Frequency, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFrequency(Frequency, Modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setUnsignedInt32Param(getModulatorSlotNumber(Modulator), 17, Frequency*1000);
	if (EC != MC_OK)
		return EC;

	GetTFrequency(Frequency, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetSearchRange(unsigned int &SearchRange, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getUnsignedInt32Param(getDemodulatorSlotNumber(Demodulator), 22, SearchRange);
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetSearchRange(unsigned int &SearchRange, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateSearchRange(SearchRange, Demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setUnsignedInt32Param(getDemodulatorSlotNumber(Demodulator), 22, SearchRange);
	if (EC != MC_OK)
		return EC;

	GetSearchRange(SearchRange, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetInputLevel(double &InputLevel, int Demodulator)
{
	InputLevel = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int iLevel = 0;
	MC_ErrorCode EC = getSignedInt16Param(getDemodulatorSlotNumber(Demodulator), 4, iLevel);
	if (EC != MC_OK)
		return EC;
	InputLevel = iLevel/100.;
	
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetEbNo(double &EbNo, int Demodulator)
{
	EbNo = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	unsigned int value = 0;
	MC_ErrorCode EC = getUnsignedInt32Param(getDemodulatorSlotNumber(Demodulator), 1, value);
	if (EC != MC_OK)
		return EC;
	EbNo = value/100.;
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::IsRCarrier(BOOL &bValid, int Demodulator)
{
	bValid = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	unsigned int value = 0;
	MC_ErrorCode EC = getUnsignedInt8Param(getDemodulatorSlotNumber(Demodulator), 0, value);
	bValid = (value != 0);
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetDemodulatorOffset(int &Offset, int Demodulator)
{
	Offset = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getSignedInt32Param(getDemodulatorSlotNumber(Demodulator), 3, Offset);
	
	return EC;
}

// Modulation type

//virtual
int CDatum_M7::GetRModulationTypeCount()
{
	return sizeof(DATUM_M7_MODULATION_TYPES)/sizeof(DATUM_M7_MODULATION_TYPES[0]);
}

//virtual
const char *CDatum_M7::GetRModulationTypeName(int Type)
{
	return DATUM_M7_MODULATION_TYPES[Type];
}

//virtual
MC_ErrorCode CDatum_M7::GetRModulationType(int &Type, int Demodulator)
{
	Type = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getSignedInt8Param(getDemodulatorSlotNumber(Demodulator), 25, Type);
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetRModulationType(int &Type, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRModulationType(Type, Demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setSignedInt8Param(getDemodulatorSlotNumber(Demodulator), 25, Type);
	if (EC != MC_OK)

	GetRModulationType(Type, Demodulator);
	
	return EC;
}

//virtual
int CDatum_M7::GetTModulationTypeCount()
{
	return sizeof(DATUM_M7_MODULATION_TYPES)/sizeof(DATUM_M7_MODULATION_TYPES[0]);
}

//virtual
const char *CDatum_M7::GetTModulationTypeName(int Type)
{
	return DATUM_M7_MODULATION_TYPES[Type];
}

//virtual
MC_ErrorCode CDatum_M7::GetTModulationType(int &Type, int Modulator)
{
	Type = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getSignedInt8Param(getModulatorSlotNumber(Modulator), 21, Type);

	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetTModulationType(int &Type, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTModulationType(Type, Modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setSignedInt8Param(getModulatorSlotNumber(Modulator), 21, Type);
	if (EC != MC_OK)
		return EC;
	GetTModulationType(Type, Modulator);
	
	return EC;
}

// Spectral inversion
//virtual
MC_ErrorCode CDatum_M7::IsRSpectralInvEnabled(BOOL &bEnabled, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getBooleanParam(getDemodulatorSlotNumber(Demodulator), 26, bEnabled);
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::EnableRSpectralInv(BOOL &bEnabled, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRSpectralInvEnabled(bEnabled, Demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setBooleanParam(getDemodulatorSlotNumber(Demodulator), 26, bEnabled);
	if (EC != MC_OK)
		return EC;
	IsRSpectralInvEnabled(bEnabled, Demodulator);
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::IsTSpectralInvEnabled(BOOL &bEnabled, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getBooleanParam(getModulatorSlotNumber(Modulator), 22, bEnabled);
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::EnableTSpectralInv(BOOL &bEnabled, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTSpectralInvEnabled(bEnabled, Modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setBooleanParam(getModulatorSlotNumber(Modulator), 22, bEnabled);
	if (EC != MC_OK)
		return EC;
	IsTSpectralInvEnabled(bEnabled, Modulator);
	
	return EC;
}

//virtual
int CDatum_M7::GetRPowerSupplyModesCount()
{
	return sizeof(DATUM_M7_R_POWER_SUPPLY_MODES)/sizeof(DATUM_M7_R_POWER_SUPPLY_MODES[0]);
}

//virtual
const char *CDatum_M7::GetRPowerSupplyModeName(int Mode)
{
	return DATUM_M7_R_POWER_SUPPLY_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetRPowerSupplyMode(int &Mode, int Demodulator)
{
	Mode = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getSignedInt8Param(getDemodulatorSlotNumber(Demodulator), 176, Mode); // 192?

	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetRPowerSupplyMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRPowerSupplyMode(Mode, Demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setSignedInt8Param(getDemodulatorSlotNumber(Demodulator), 176, Mode);

	GetRPowerSupplyMode(Mode, Demodulator);
	return EC;
}

//virtual
int CDatum_M7::GetTPowerSupplyModesCount()
{
	return sizeof(DATUM_M7_T_POWER_SUPPLY_MODES)/sizeof(DATUM_M7_T_POWER_SUPPLY_MODES[0]);
}

//virtual
const char *CDatum_M7::GetTPowerSupplyModeName(int Mode)
{
	return DATUM_M7_T_POWER_SUPPLY_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetTPowerSupplyMode(int &Mode, int Modulator)
{
	Mode = -1;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getSignedInt8Param(getModulatorSlotNumber(Modulator), 176, Mode); // 192?

	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetTPowerSupplyMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTPowerSupplyMode(Mode, Modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setSignedInt8Param(getModulatorSlotNumber(Modulator), 176, Mode);

	GetTPowerSupplyMode(Mode, Modulator);
	return EC;
}

// 10 MHz reference 

//virtual
int CDatum_M7::GetR10MHzModesCount()
{
	return sizeof(DATUM_M7_10MHZ_MODES)/sizeof(DATUM_M7_10MHZ_MODES[0]);
}

//virtual
const char *CDatum_M7::doGetR10MHzModeName(int mode)
{
	return DATUM_M7_10MHZ_MODES[mode];
}

//virtual
MC_ErrorCode CDatum_M7::doGetR10MHzMode(int &mode, int demodulator)
{
	MC_ErrorCode EC = getSignedInt8Param(getDemodulatorSlotNumber(demodulator), 179, mode);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::doSetR10MHzMode(int &mode, int demodulator)
{
	MC_ErrorCode EC = setSignedInt8Param(getDemodulatorSlotNumber(demodulator), 179, mode);
	return EC;
}

//virtual
int CDatum_M7::GetT10MHzModesCount()
{
	return sizeof(DATUM_M7_10MHZ_MODES)/sizeof(DATUM_M7_10MHZ_MODES[0]);
}

//virtual
const char *CDatum_M7::doGetT10MHzModeName(int mode)
{
	return DATUM_M7_10MHZ_MODES[mode];
}

//virtual
MC_ErrorCode CDatum_M7::doGetT10MHzMode(int &mode, int modulator)
{
	MC_ErrorCode EC = getSignedInt8Param(getModulatorSlotNumber(modulator), 179, mode);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::doSetT10MHzMode(int &mode, int modulator)
{
	MC_ErrorCode EC = setSignedInt8Param(getModulatorSlotNumber(modulator), 179, mode);
	return EC;
}

// Data rate

//virtual
MC_ErrorCode CDatum_M7::GetRDataRate(unsigned int &DataRate, int Demodulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getUnsignedInt32Param(getDemodulatorSlotNumber(Demodulator), 32, DataRate);
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetRDataRate(unsigned int &DataRate, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRDataRate(DataRate, Demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setUnsignedInt32Param(getDemodulatorSlotNumber(Demodulator), 32, DataRate);
	if (EC != MC_OK)
		return EC;

	GetRDataRate(DataRate, Demodulator);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetTDataRate(unsigned int &DataRate, int Modulator)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getUnsignedInt32Param(getModulatorSlotNumber(Modulator), 32, DataRate);
	
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetTDataRate(unsigned int &DataRate, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTDataRate(DataRate, Modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setUnsignedInt32Param(getModulatorSlotNumber(Modulator), 32, DataRate);
	if (EC != MC_OK)
		return EC;

	GetTDataRate(DataRate, Modulator);
	return EC;
}

// Output
//virtual
MC_ErrorCode CDatum_M7::IsOutputOn(BOOL &bOn, int Modulator)
{
	bOn = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = getBooleanParam(getModulatorSlotNumber(Modulator), 20, bOn);

	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::TurnOutputOn(BOOL &bOn, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateOutputOn(bOn, Modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = setBooleanParam(getModulatorSlotNumber(Modulator), 20, bOn);

	IsOutputOn(bOn, Modulator);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetOutputLevel(double &Level, int Modulator)
{
	Level = 0.;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	int iLevel = 0;
	MC_ErrorCode EC = getSignedInt16Param(getModulatorSlotNumber(Modulator), 19, iLevel);
	if (EC != MC_OK)
		return EC;

	Level = iLevel/100.;

	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetOutputLevel(double &Level, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateOutputLevel(Level, Modulator))
		return MC_OK; // already set

	int iLevel = (int)(Level*100);
	MC_ErrorCode EC = setSignedInt16Param(getModulatorSlotNumber(Modulator), 19, iLevel);

	GetOutputLevel(Level, Modulator);
	return EC;
}

// FEC
//virtual
int CDatum_M7::GetRFecModesCount()
{
	return (sizeof(DATUM_M7_FEC_MODES)/sizeof(DATUM_M7_FEC_MODES[0]));
}

//virtual
const char *CDatum_M7::GetRFecModeName(int Mode)
{
	return DATUM_M7_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetRFecMode(int &Mode, int Demodulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatum_M7::SetRFecMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFecMode(Mode, Demodulator))
		return MC_OK; // already set

	GetRFecMode(Mode, Demodulator);
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
int CDatum_M7::GetTFecModesCount()
{
	return (sizeof(DATUM_M7_FEC_MODES)/sizeof(DATUM_M7_FEC_MODES[0]));
}

//virtual
const char *CDatum_M7::GetTFecModeName(int Mode)
{
	return DATUM_M7_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetTFecMode(int &Mode, int Modulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatum_M7::SetTFecMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFecMode(Mode, Modulator))
		return MC_OK; // already set

	GetTFecMode(Mode, Modulator);
	return MC_COMMAND_NOT_SUPPORTED;
}

// Reed-Solomon

//virtual
int CDatum_M7::GetReedSolomonModesCount()
{
	return (sizeof(DATUM_M7_REED_SOLOMON_MODES)/sizeof(DATUM_M7_REED_SOLOMON_MODES[0]));
}

//virtual
const char *CDatum_M7::GetReedSolomonModeName(int Mode)
{
	return DATUM_M7_REED_SOLOMON_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetRReedSolomonMode(int &Mode, int Demodulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatum_M7::SetRReedSolomonMode(int &Mode, int Demodulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatum_M7::GetTReedSolomonMode(int &Mode, int Modulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatum_M7::SetTReedSolomonMode(int &Mode, int Modulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

// Differential encoding/decoding
//virtual
int CDatum_M7::GetDiffDecoderModeCount()
{
	return sizeof(DATUM_M7_DIFF_CODING_MODES)/sizeof(DATUM_M7_DIFF_CODING_MODES[0]);
}

//virtual
const char *CDatum_M7::GetDiffDecoderModeName(int Mode)
{
	return DATUM_M7_DIFF_CODING_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetDiffDecoderMode(int &Mode, int Demodulator)
{
	Mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	MC_ErrorCode EC = getSignedInt8Param(getDemodulatorSlotNumber(Demodulator), 43, Mode);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetDiffDecoderMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffDecoderMode(Mode, Demodulator))
		return MC_OK; // already set

	MC_ErrorCode EC = getSignedInt8Param(getDemodulatorSlotNumber(Demodulator), 43, Mode);

	GetDiffDecoderMode(Mode, Demodulator);
	return EC;
}

//virtual
int CDatum_M7::GetDiffEncoderModeCount()
{
	return sizeof(DATUM_M7_DIFF_CODING_MODES)/sizeof(DATUM_M7_DIFF_CODING_MODES[0]);
}

//virtual
const char *CDatum_M7::GetDiffEncoderModeName(int Mode)
{
	return DATUM_M7_DIFF_CODING_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetDiffEncoderMode(int &Mode, int Modulator)
{
	Mode = -1;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	MC_ErrorCode EC = getSignedInt8Param(getModulatorSlotNumber(Modulator), 43, Mode);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetDiffEncoderMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDiffEncoderMode(Mode, Modulator))
		return MC_OK; // already set

	MC_ErrorCode EC = getSignedInt8Param(getModulatorSlotNumber(Modulator), 43, Mode);

	GetDiffEncoderMode(Mode, Modulator);
	return EC;
}

// Scrambling/Descrambling

//virtual
int CDatum_M7::GetScramblerModesCount()
{
	return sizeof(DATUM_M7_SCRAMBLER_MODES)/sizeof(DATUM_M7_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatum_M7::doGetScramblerModeName(int mode)
{
	return DATUM_M7_SCRAMBLER_MODES[mode];
}

//virtual
MC_ErrorCode CDatum_M7::doGetScramblerMode(int &mode, int modulator)
{
	MC_ErrorCode EC = getSignedInt8Param(getModulatorSlotNumber(modulator), 44, mode);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::doSetScramblerMode(int &mode, int modulator)
{
	MC_ErrorCode EC = setSignedInt8Param(getModulatorSlotNumber(modulator), 44, mode);
	return EC;
}

//virtual
int CDatum_M7::GetDescramblerModesCount()
{
	return sizeof(DATUM_M7_SCRAMBLER_MODES)/sizeof(DATUM_M7_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatum_M7::doGetDescramblerModeName(int mode)
{
	return DATUM_M7_SCRAMBLER_MODES[mode];
}

//virtual
MC_ErrorCode CDatum_M7::doGetDescramblerMode(int &mode, int demodulator)
{
	MC_ErrorCode EC = getSignedInt8Param(getDemodulatorSlotNumber(demodulator), 44, mode);
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::doSetDescramblerMode(int &mode, int demodulator)
{
	MC_ErrorCode EC = setSignedInt8Param(getDemodulatorSlotNumber(demodulator), 44, mode);
	return EC;
}

// Alarms

//virtual
MC_ErrorCode CDatum_M7::GetUnitAlarms(unsigned int *&pAlarms)
{
	pAlarms = m_UnitAlarms;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = MC_DEVICE_NOT_RESPONDING; // TODO: implement it
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetInterfaceAlarms(unsigned int *&pAlarms)
{
	pAlarms = m_InterfaceAlarms;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = MC_DEVICE_NOT_RESPONDING; // TODO: implement it
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetModulatorAlarms(unsigned int *&pAlarms)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = MC_DEVICE_NOT_RESPONDING; // TODO: implement it
	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::GetDemodulatorAlarms(unsigned int *&pAlarms)
{
	pAlarms = m_DemodulatorAlarms;
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = MC_DEVICE_NOT_RESPONDING; // TODO: implement it

	return EC;
}

MC_ErrorCode CDatum_M7::getUnsignedInt32Param(unsigned char slot, unsigned char param, unsigned int &value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x03; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x00; // "get" (0 bytes)

	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);

	if (EC != MC_OK)
		return EC;

	value = ModemDataToUnsignedInt32(m_pRawReply + 11);

	return EC;
}

MC_ErrorCode CDatum_M7::getSignedInt32Param(unsigned char slot, unsigned char param, int &value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x03; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x00; // "get" (0 bytes)

	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);

	if (EC != MC_OK)
		return EC;

	value = ModemDataToSignedInt32(m_pRawReply + 11);

	return EC;
}

MC_ErrorCode CDatum_M7::setUnsignedInt32Param(unsigned char slot, unsigned char param, unsigned int value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x07; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x10; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x04; // "set" integer (4 bytes)

	// data itself
	m_pszCommand[CommandLength++] = (unsigned char)((value & (0xFF << 0)) >> 0);
	m_pszCommand[CommandLength++] = (unsigned char)((value & (0xFF << 8)) >> 8);
	m_pszCommand[CommandLength++] = (unsigned char)((value & (0xFF << 16)) >> 16);
	m_pszCommand[CommandLength++] = (unsigned char)((value & (0xFF << 24)) >> 24);

	
	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);
	return EC;
}

MC_ErrorCode CDatum_M7::getUnsignedInt16Param(unsigned char slot, unsigned char param, unsigned int &value)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

MC_ErrorCode CDatum_M7::getSignedInt16Param(unsigned char slot, unsigned char param, int &value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x03; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x00; // "get" (0 bytes)

	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);

	if (EC != MC_OK)
		return EC;

	value = ModemDataToSignedInt16(m_pRawReply + 11);

	return EC;
}

MC_ErrorCode CDatum_M7::setUnsignedInt16Param(unsigned char slot, unsigned char param, unsigned int value)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

MC_ErrorCode CDatum_M7::setSignedInt16Param(unsigned char slot, unsigned char param, int value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x05; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x10; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x02; // "set" integer (2 bytes)

	// data itself
	m_pszCommand[CommandLength++] = (unsigned char)((value & (0xFF << 0)) >> 0);
	m_pszCommand[CommandLength++] = (unsigned char)((value & (0xFF << 8)) >> 8);
	
	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);
	return EC;
}

MC_ErrorCode CDatum_M7::getUnsignedInt8Param(unsigned char slot, unsigned char param, unsigned int &value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x03; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x00; // "get" (0 bytes)

	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);

	if (EC != MC_OK)
		return EC;

	value = ModemDataToUnsignedInt8(m_pRawReply + 11);

	return EC;
}

MC_ErrorCode CDatum_M7::getSignedInt8Param(unsigned char slot, unsigned char param, int &value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x03; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x00; // "get" (0 bytes)

	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);

	if (EC != MC_OK)
		return EC;

	value = ModemDataToUnsignedInt8(m_pRawReply + 11);

	return EC;
}

MC_ErrorCode CDatum_M7::setUnsignedInt8Param(unsigned char slot, unsigned char param, unsigned int value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x04; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x10; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x01; // "set" integer (4 bytes)

	// data itself
	m_pszCommand[CommandLength++] = (unsigned char)((value & (0xFF << 0)) >> 0);

	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);
	return EC;
}

MC_ErrorCode CDatum_M7::setSignedInt8Param(unsigned char slot, unsigned char param, int value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x04; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x10; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x01; // "set" integer (4 bytes)

	// data itself
	m_pszCommand[CommandLength++] = (unsigned char)((value & (0xFF << 0)) >> 0);

	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);
	return EC;
}

MC_ErrorCode CDatum_M7::getStringParam(unsigned char slot, unsigned char param, std::string &value)
{
	value.empty();
	
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x03; // control and payload byte count
	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength++] = 0x00; // "get" (0 bytes)

	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);

	if (EC != MC_OK)
		return EC;

	value = (char *)(m_pRawReply + 11);

	return EC;
}

MC_ErrorCode CDatum_M7::setStringParam(unsigned char slot, unsigned char param, const std::string &value)
{
	int CommandLength = 0;
	m_pszCommand[CommandLength++] = 0xA5; // opening pad byte
	m_pszCommand[CommandLength++] = m_ModemAddress; // 
	m_pszCommand[CommandLength++] = m_ControllerAddress; // 
	m_pszCommand[CommandLength++] = 0x00; // remote address

	m_pszCommand[CommandLength++] = 0x00; // control and payload byte count
	m_pszCommand[CommandLength] = 0x04; // control and payload byte count = 3 + length + terminating zero
	for (int i = 0; i < value.length(); i++)
	{
		++m_pszCommand[CommandLength];
	}
	++CommandLength;
	m_pszCommand[CommandLength++] = 0x10; // control and payload byte count

	// payload
	m_pszCommand[CommandLength++] = slot; // slot number
	m_pszCommand[CommandLength++] = param; // command number
	m_pszCommand[CommandLength] = 0x01; // length + terminating zero
	for (i = 0; i < value.length(); i++)
	{
		++m_pszCommand[CommandLength];
	}
	++CommandLength;

	// data itself
	for (i = 0; i < value.length(); i++)
	{
		m_pszCommand[CommandLength++] = value[i];
	}
	m_pszCommand[CommandLength++] = 0; // terminating zero


	unsigned short crc = CRC_CCITT(m_pszCommand, CommandLength);
	memcpy(m_pszCommand+CommandLength, &crc, 2);
	CommandLength += 2;

	MC_ErrorCode EC = Command(CommandLength);
	return EC;
}

MC_ErrorCode CDatum_M7::getBooleanParam(unsigned char slot, unsigned char param, BOOL &bValue)
{
	unsigned int iValue = 0;
	MC_ErrorCode EC = getUnsignedInt8Param(slot, param, iValue);
	bValue = (iValue != 0);
	return EC;

}

MC_ErrorCode CDatum_M7::setBooleanParam(unsigned char slot, unsigned char param, BOOL bValue)
{
	unsigned int iValue = 0;
	if (bValue)
		iValue = 1;
	MC_ErrorCode EC = setUnsignedInt8Param(slot, param, iValue);
	return EC;
}

unsigned int CDatum_M7::ModemDataToUnsignedInt32(const unsigned char *pModemData)
{
	unsigned int value = pModemData[0];
	value |= pModemData[1] << 8;
	value |= pModemData[2] << 16;
	value |= pModemData[3] << 24;

	return value;
}

int CDatum_M7::ModemDataToSignedInt32(const unsigned char *pModemData)
{
	int value = pModemData[0];
	value |= pModemData[1] << 8;
	value |= pModemData[2] << 16;
	value |= pModemData[3] << 24;

	return value;
}

short CDatum_M7::ModemDataToSignedInt16(const unsigned char *pModemData)
{
	short value = pModemData[0];
	value |= pModemData[1] << 8;

	return value;
}

unsigned short CDatum_M7::ModemDataToUnsignedInt16(const unsigned char *pModemData)
{
	unsigned short value = pModemData[0];
	value |= pModemData[1] << 8;

	return value;
}

unsigned char CDatum_M7::ModemDataToUnsignedInt8(const unsigned char *pModemData)
{
	unsigned char value = pModemData[0];

	return value;
}

//////////////////////////////////////////////////////////////////////
//
// CDatum_M7_LMod_LDem

char *CDatum_M7_LMod_LDem::m_pszName="Datum M7 (L-band mod + L-band dem)";

CDatum_M7_LMod_LDem::CDatum_M7_LMod_LDem()
{
}

//virtual
CDatum_M7_LMod_LDem::~CDatum_M7_LMod_LDem()
{
}

//virtual
unsigned char CDatum_M7_LMod_LDem::getModulatorSlotNumber(int nModulator) const
{
	if (nModulator == 1)
		return 1;
	return 0;
}

//virtual
unsigned char CDatum_M7_LMod_LDem::getDemodulatorSlotNumber(int nDemodulator) const
{
	if (nDemodulator == 1)
		return 2;
	return 0;
}

