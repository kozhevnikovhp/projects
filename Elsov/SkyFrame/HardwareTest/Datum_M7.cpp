#include "stdafx.h"
#include "Datum_M7.h"


//////////////////////////////////////////////////////////////////////
//
// 

static const char *DATUM_M7_MODULATION_TYPES[] = {
	"BPSK",
	"QPSK",
	"OQPSK",
	"8PSK",
	"8QAM",
	"16QAM"
};

static const char *DATUM_M7_FEC_MODES[] = {
	"Viterbi 1/2",
	"Viterbi 3/4",
	"Viterbi 7/8"
};

static const char *DATUM_M7_SCRAMBLER_MODES[] = {
	"Disable",		// 0
	"IESS-308",		// 1
	"IESS-309",		// 2
	"V.35",			// 3
	"Alt V.35",		// 4
	"Intelsat",		// 5
	"Alt Intelsat",	// 6
	"TPC Synchro"	// 7
};

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
	return MC_OK;
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
	Type = 0;
	//if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	MC_ErrorCode EC = MC_DEVICE_NOT_RESPONDING;

	unsigned char bb[] = { 0xA5, 0x64, 0xCC, 0x00, 0x00, 0x03, 0x00, 0x01, 0x20, 0x00, 0xBB, 0xCC };
	int len = 10;
	unsigned short crc = CRC_CCITT(bb, len);
	memcpy(bb+len, &crc, 2);

	return EC;
}

//virtual
MC_ErrorCode CDatum_M7::SetRModulationType(int &Type, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRModulationType(Type, Demodulator))
		return MC_OK; // already set

	return MC_COMMAND_NOT_SUPPORTED;
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
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatum_M7::SetTModulationType(int &Type, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTModulationType(Type, Modulator))
		return MC_OK; // already set

	GetTModulationType(Type, Modulator);
	return MC_COMMAND_NOT_SUPPORTED;
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

// Scrambling/Descrambling

//virtual
int CDatum_M7::GetScramblerModesCount()
{
	return sizeof(DATUM_M7_SCRAMBLER_MODES)/sizeof(DATUM_M7_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatum_M7::GetScramblerModeName(int Mode)
{
	if (Mode < 0 || Mode >= GetDescramblerModesCount())
		return "";
	return DATUM_M7_SCRAMBLER_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetScramblerMode(int &mode, int Modulator)
{
	mode = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatum_M7::SetScramblerMode(int &mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateScramblerMode(mode, Modulator))
		return MC_OK; // already set

	GetScramblerMode(mode, Modulator);
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
int CDatum_M7::GetDescramblerModesCount()
{
	return sizeof(DATUM_M7_SCRAMBLER_MODES)/sizeof(DATUM_M7_SCRAMBLER_MODES[0]);
}

//virtual
const char *CDatum_M7::GetDescramblerModeName(int Mode)
{
	if (Mode < 0 || Mode >= GetDescramblerModesCount())
		return "";
	return DATUM_M7_SCRAMBLER_MODES[Mode];
}

//virtual
MC_ErrorCode CDatum_M7::GetDescramblerMode(int &mode, int Demodulator)
{
	mode = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CDatum_M7::SetDescramblerMode(int &mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateDescramblerMode(mode, Demodulator))
		return MC_OK; // already set


	GetDescramblerMode(mode, Demodulator);
	return MC_COMMAND_NOT_SUPPORTED;
}


//////////////////////////////////////////////////////////////////////
//
// CDatum_M7_LMod_LDem

char *CDatum_M7_LMod_LDem::m_pszName="Datum M7 (L-band mod + L-band dem)";

CDatum_M7_LMod_LDem::CDatum_M7_LMod_LDem()
{
	unsigned char bb[] = { 0xA5, 0x00, 0x00, 0x03, 0x00, 0x02, 0x11, 0x00 };
	unsigned short crc = CRC_CCITT(bb, sizeof(bb));
}

//virtual
CDatum_M7_LMod_LDem::~CDatum_M7_LMod_LDem()
{
}

