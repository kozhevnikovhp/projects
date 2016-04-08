#include "stdafx.h"
#include "StelaM.h"
#include "../common/SDMS_OIDs.h"

static char CR=0x0D;
static char LF=0x0A;


const char *pszPublicCommunity = "public";
const char *pszPrivateCommunity = "private";

static char *GetNextNumberChar(char *pszStartSearch)
{
	// Skip this number
	while (*pszStartSearch != 0 && isdigit(*pszStartSearch))
		pszStartSearch++;
	// Skip spaces, tabs, letters and other non-digit symbols until next number
	while (*pszStartSearch != 0 && !isdigit(*pszStartSearch))
		pszStartSearch++;
	return pszStartSearch;
}

static const char *SDMS_MODULATION_TYPES[] = {
	"BPSK",
	"QPSK"
};

static const char *SDMS_FEC_MODES[] = {
	"None",
	"Viterbi 1/2",
	"Viterbi 3/4",
	"Viterbi 7/8"
};

CSdmsAPI::CSdmsAPI()
{
	m_bReady = TRUE;
	m_cCurrentPrompt = ' ';
}

CSdmsAPI::~CSdmsAPI()
{
}

//virtual
MC_ErrorCode CSdmsAPI::Command(char *pszCommand, int WaitTime/*= 0*/)
{
	strcpy(m_pszCommand, pszCommand);
	strcat(m_pszCommand, "\r\n");
	m_pszReply[0]=0;
	m_bReady = FALSE;
	Sleep(100);
	GetTerminal()->ClearAllBuffers();
	if (!GetTerminal()->WriteString(m_pszCommand))
		return MC_DEVICE_NOT_RESPONDING;
	Sleep(WaitTime);
	MC_ErrorCode EC = ReadReplyUntilPrompt();
	if (EC != MC_OK)
		return EC;
	Sleep(100);
	GetTerminal()->ClearAllBuffers();
	return MC_OK;
}

void CSdmsAPI::CutPrompt()
{
	char *pszEnd = strstr(m_pszReply, "Cons");
	if (pszEnd)
		*pszEnd = 0;
}

//virtual
CTerminal *CSdmsAPI::GetTerminal()
{
	switch (m_ControlType)
	{
	case MC_UNKNOWN_TYPE:
	case MC_SERIAL_PORT:
		return &m_SerialPort;
	case MC_TELNET:
		return &m_TelnetSocket;
	}
	return NULL;
}

//virtual
void CSdmsAPI::OnMcConnected()
{
	GetTerminal()->SetReadTimeout(3000);
	GetTerminal()->SetWriteTimeout(3000);
	if (IsMCConnectionAlive() != MC_OK)
	{
		m_bControllable = FALSE;
		return;
	}
	GoTop();
	
	if (IsMCConnectionAlive() != MC_OK)
	{
		m_bControllable = FALSE;
		return;
	}
}

MC_ErrorCode CSdmsAPI::GoTop()
{
	// Go up until simple user mode (but not greater than 10 exits to avoid infinite loops)
	int nExits = 0;
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;
	while (m_cCurrentPrompt != '>' && nExits < 10)
	{
		++nExits;
		EC = Exit();
		if (EC != MC_OK)
			return EC;
	}
	return MC_OK;
}

//virtual
void CSdmsAPI::OnMcSymbolReceived(char cSymbol)
{
	if (strlen(m_pszReply) < sizeof(m_pszReply))
	{
		if (cSymbol == LF)
		{
		}
		else if (cSymbol == CR)
		{
			strncat(m_pszReply, &CR, 1);
			strncat(m_pszReply, &LF, 1);
		}
		else if (cSymbol == 8)
		{
			strcat(m_pszReply, " ");
		}
		else
		{
			strncat(m_pszReply, &cSymbol, 1);
		}
	}
	if (cSymbol == '>' || cSymbol == '#')
	{
		m_bReady = TRUE;
		m_cCurrentPrompt = cSymbol;
	}
}

//virtual
MC_ErrorCode CSdmsAPI::GetDeviceVersionString(char *&pszString)
{
	pszString = m_szVersion;
	m_szVersion[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = Command("show version");
	if (EC == MC_OK)
	{
		CutPrompt();
		char *pszSystemInfo = GetMCReply();
		if (pszSystemInfo)
			strcpy(m_szVersion, m_pszReply);
	}
	
	return EC;
}

// Modulation type

//virtual
int CSdmsAPI::GetRModulationTypeCount()
{
	return sizeof(SDMS_MODULATION_TYPES)/sizeof(SDMS_MODULATION_TYPES[0]);
}

//virtual 
const char *CSdmsAPI::GetRModulationTypeName(int Type)
{
	return SDMS_MODULATION_TYPES[Type];
}

//virtual
int CSdmsAPI::GetTModulationTypeCount()
{
	return sizeof(SDMS_MODULATION_TYPES)/sizeof(SDMS_MODULATION_TYPES[0]);
}

//virtual 
const char *CSdmsAPI::GetTModulationTypeName(int Type)
{
	return SDMS_MODULATION_TYPES[Type];
}

// FEC

//virtual
int CSdmsAPI::GetRFecModesCount()
{
	return (sizeof(SDMS_FEC_MODES)/sizeof(SDMS_FEC_MODES[0]));
}

//virtual
const char *CSdmsAPI::GetRFecModeName(int Mode)
{
	return SDMS_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CSdmsAPI::GetRFecMode(int &Mode, int Demodulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;

	return MC_OK;
}

//virtual
MC_ErrorCode CSdmsAPI::SetRFecMode(int &Mode, int Demodulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateRFecMode(Mode, Demodulator))
		return MC_OK; // already set

	return MC_OK;
}

//virtual
int CSdmsAPI::GetTFecModesCount()
{
	return (sizeof(SDMS_FEC_MODES)/sizeof(SDMS_FEC_MODES[0]));
}

//virtual
const char *CSdmsAPI::GetTFecModeName(int Mode)
{
	return SDMS_FEC_MODES[Mode];
}

//virtual
MC_ErrorCode CSdmsAPI::GetTFecMode(int &Mode, int Modulator)
{
	Mode = 0; // None
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	
	return MC_OK;
}

//virtual
MC_ErrorCode CSdmsAPI::SetTFecMode(int &Mode, int Modulator)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	if (!NeedToUpdateTFecMode(Mode, Modulator))
		return MC_OK; // already set

	return MC_OK;
}

//virtual
MC_ErrorCode CSdmsAPI::GetRIfStatus(CDemIfStatus &Status, int Demodulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	EC = Command("demodulator status show");
	if (EC != MC_OK)
	{
		GoTop();
		return EC;
	}

	char *pszResult;
	// Lock status
	pszResult = strstr(m_pszReply, "Locked");
	Status.m_bCarrier = (pszResult != NULL);
	// EbNo
	pszResult = strstr(m_pszReply, "Eb/No");
	if (pszResult)
	{
		sscanf(pszResult, "Eb/No %lf", &Status.m_SN);
	}
	else
		return MC_DEVICE_NOT_RESPONDING;
	// Input level
	pszResult = strstr(m_pszReply, "Level");
	if (pszResult)
	{
		sscanf(pszResult, "Level %lf", &Status.m_InputLevel);
		Status.m_InputLevel /= 10;
	}
	else
		return MC_DEVICE_NOT_RESPONDING;
	// Offset
	pszResult = strstr(m_pszReply, "Offset");
	if (pszResult)
		sscanf(pszResult, "Offset %d", &Status.m_Offset);
	else
		return MC_DEVICE_NOT_RESPONDING;

	GoTop();

	return MC_OK;
}

//virtual
MC_ErrorCode CSdmsAPI::GetRFrequency(unsigned int &Frequency, int Demodulator)
{
	CDemIfParams params;
	MC_ErrorCode EC = GetRIfParams(params, Demodulator);
	Frequency = params.m_Frequency;
	return EC;
}

MC_ErrorCode CSdmsAPI::SetRFrequency(unsigned int &Frequency, int Demodulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	m_pszReply[0]=0;
	m_bReady = FALSE;
	sprintf(m_pszCommand, "demodulator if cxr-frequency %d", Frequency*1000);
	EC = Command(m_pszCommand, 5000);
	if (EC != MC_OK)
		return EC;

	EC = Command("demodulator if show");
	if (EC == MC_OK)
	{
		char *pszResult;
		// Frequency
		pszResult = strstr(m_pszReply, "Frequency");
		if (pszResult)
			sscanf(pszResult, "Frequency %ld", &Frequency);
		Frequency /= 1000;
	}

	GoTop();
	return EC;
}

//virtual
MC_ErrorCode CSdmsAPI::GetRIfParams(CDemIfParams &Params, int Demodulator)
{
	char szDummy[32];
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	EC = Command("demodulator if show");
	if (EC == MC_OK)
	{
		char *pszResult;
		
		// Frequency
		pszResult = strstr(m_pszReply, "Frequency");
		if (pszResult)
		{
			pszResult = GetNextNumberChar(pszResult);
			Params.m_Frequency = atoi(pszResult);
			Params.m_Frequency /= 1000; // Hz -> KHz
		}

		// BPSK/QPSK
		pszResult = strstr(m_pszReply, "Modulation Mode:");
		if (pszResult)
		{
			sscanf(pszResult, "Modulation Mode: %s", szDummy);
			if (!strcmp(szDummy, "BPSK"))
				Params.m_ModulationType = 0;
			else
				Params.m_ModulationType = 1;
		}

		// Spectrum inversion (absent in current implementation)
		pszResult = strstr(m_pszReply, "Modulation Spectrum:");
		if (pszResult)
		{
			sscanf(pszResult, "Modulation Spectrum: %s", szDummy);
			if (!strcmp(szDummy, "Normal"))
				Params.m_SpectrumMode = 0;
			else
				Params.m_SpectrumMode = 1;
		}
		
		// Sweep range
		pszResult = strstr(m_pszReply, "Sweep Range");
		if (pszResult)
		{
			pszResult = GetNextNumberChar(pszResult);
			Params.m_SearchRange = atoi(pszResult);
		}
	}

	GoTop();
	return EC;
}

//virtual
MC_ErrorCode CSdmsAPI::GetTIfParams(CModIfParams &Params, int Modulator)
{
	char szDummy[32];
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	EC = Command("modulator if show");
	if (EC == MC_OK)
	{
		char *pszResult;
		
		// Frequency
		pszResult = strstr(m_pszReply, "Frequency");
		if (pszResult)
		{
			pszResult = GetNextNumberChar(pszResult);
			Params.m_Frequency = atoi(pszResult);
			Params.m_Frequency /= 1000; // Hz -> KHz
		}

		// On/off
		pszResult = strstr(m_pszReply, "Carrier Output");
		if (pszResult)
		{
			sscanf(pszResult, "Carrier Output: %s", szDummy);
			if (!strcmp(szDummy, "Enable"))
				Params.m_bOutputEnabled = TRUE;
			else
				Params.m_bOutputEnabled = FALSE;
		}

		// Output level
		pszResult = strstr(m_pszReply, "Level");
		if (pszResult)
			sscanf(pszResult, "Level %lf", &Params.m_OutputLevel);
		Params.m_OutputLevel /= 10;

		// BPSK/QPSK
		pszResult = strstr(m_pszReply, "Modulation Mode:");
		if (pszResult)
		{
			sscanf(pszResult, "Modulation Mode: %s", szDummy);
			if (!strcmp(szDummy, "BPSK"))
				Params.m_ModulationType = 0;
			else
				Params.m_ModulationType = 1;
		}

		// Spectrum inversion
		pszResult = strstr(m_pszReply, "Modulation Spectrum:");
		if (pszResult)
		{
			sscanf(pszResult, "Modulation Spectrum: %s", szDummy);
			if (!strcmp(szDummy, "Normal"))
				Params.m_SpectrumMode = 0;
			else
				Params.m_SpectrumMode = 1;
		}
	}

	GoTop();
	return EC;
}

MC_ErrorCode CSdmsAPI::IsModulatorOn(BOOL &bOn)
{
	CModIfParams Params;
	MC_ErrorCode EC = GetTIfParams(Params, 1);
	bOn = Params.m_bOutputEnabled;
	return EC;
}

//virtual
MC_ErrorCode CSdmsAPI::GetOutputLevel(double &Level, int Modulator)
{
	CModIfParams params;
	MC_ErrorCode EC = GetTIfParams(params, Modulator);
	Level = params.m_OutputLevel;
	return EC;
}

//virtual
MC_ErrorCode CSdmsAPI::SetOutputLevel(double &Level, int Modulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	m_pszReply[0]=0;
	m_bReady = FALSE;
	int NewLevel;
	if (Level<0.)
		NewLevel = (int)(Level*10.-0.01);
	else
		NewLevel = (int)(Level*10.+0.01);
	sprintf(m_pszCommand, "modulator if cxr-level %d", NewLevel);
	EC = Command(m_pszCommand, 5000);
	if (EC != MC_OK)
		return EC;

	EC = Command("modulator if show");
	if (EC == MC_OK)
	{
		char *pszResult;
		// Output level
		pszResult = strstr(m_pszReply, "Level");
		if (pszResult)
			sscanf(pszResult, "Level %lf", &Level);
		Level /= 10;
	}

	GoTop();
	return EC;
}

//virtual
MC_ErrorCode CSdmsAPI::SetTFrequency(unsigned int &Frequency, int Modulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	m_pszReply[0]=0;
	m_bReady = FALSE;
	sprintf(m_pszCommand, "modulator if cxr-frequency %d", Frequency*1000);
	EC = Command(m_pszCommand, 5000);
	if (EC != MC_OK)
		return EC;

	EC = Command("modulator if show");
	if (EC == MC_OK)
	{
		char *pszResult;
		// Frequency
		pszResult = strstr(m_pszReply, "Frequency");
		if (pszResult)
			sscanf(pszResult, "Frequency %ld", &Frequency);
		Frequency /= 1000;
	}

	GoTop();
	return EC;
}

// Data params

//virtual
MC_ErrorCode CSdmsAPI::GetRDataParams(CModemDataParams &Params, int Demodulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	EC = Command("demodulator data show");
	if (EC == MC_OK)
		ParseRDataShow(Params);

	GoTop();
	return EC;
}

//virtual
MC_ErrorCode CSdmsAPI::GetTDataParams(CModemDataParams &Params, int Modulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	EC = Command("modulator data show");
	if (EC == MC_OK)
		ParseTDataShow(Params);

	GoTop();
	return EC;
}

void CSdmsAPI::ParseDataRate(unsigned int &DataRate)
{
	char *pszResult = strstr(m_pszReply, "Data Rate");
	if (pszResult == NULL)
		pszResult = strstr(m_pszReply, "Bit Rate");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		DataRate = atoi(pszResult);
	}
}

void CSdmsAPI::ParseCodeRate(CModemDataParams &Params)
{
	char *pszResult = strstr(m_pszReply, "Code Rate");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		if (strncmp(pszResult, "1/2", 3))
			Params.m_FecMode = 1;
		else if (strncmp(pszResult, "3/4", 3))
			Params.m_FecMode = 2;
		else if (strncmp(pszResult, "7/8", 3))
			Params.m_FecMode = 3;
	}
}

void CSdmsAPI::ParseRClockSource(int &source)
{
	char szDummy[32];
	char *pszResult = strstr(m_pszReply, "Clock Source");
	if (pszResult)
		sscanf(pszResult, "Clock Source: %s", szDummy);
	if (stricmp(szDummy, "internal") == 0)
		source = 1;
	else if (stricmp(szDummy, "external") == 0)
		source = 2;
	else if (stricmp(szDummy, "mod") == 0)
		source = 3;
	else if (stricmp(szDummy, "rcv") == 0)
		source = 0;
	else
		source = DCS_Unknown;
}

void CSdmsAPI::ParseTClockSource(int &source)
{
	char szDummy[32];
	char *pszResult = strstr(m_pszReply, "Clock Source");
	if (pszResult)
		sscanf(pszResult, "Clock Source: %s", szDummy);
	if (stricmp(szDummy, "internal") == 0)
		source = 0;
	else if (stricmp(szDummy, "external") == 0)
		source = 2;
	else if (stricmp(szDummy, "tt") == 0)
		source = 1;
	else if (stricmp(szDummy, "rcv") == 0)
		source = 3;
	else
		source = DCS_Unknown;
}

//virtual
void CSdmsAPI::ParseRDataShow(CModemDataParams &Params)
{
	// Data rate
	ParseDataRate(Params.m_DataRate);

	// Code rate
	ParseCodeRate(Params);

	// Clock source
	ParseRClockSource(Params.m_ClockSource);
}

//virtual
void CSdmsAPI::ParseTDataShow(CModemDataParams &Params)
{
	// Data rate
	ParseDataRate(Params.m_DataRate);

	// Code rate
	ParseCodeRate(Params);

	// Clock source
	ParseTClockSource(Params.m_ClockSource);
}

//virtual
MC_ErrorCode CSdmsAPI::SetTDataRate(unsigned int &DataRate, int Modulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	m_pszReply[0]=0;
	m_bReady = FALSE;
	sprintf(m_pszCommand, "modulator data bit-rate %d", DataRate);
	EC = Command(m_pszCommand, 1000);
	if (EC != MC_OK)
		return EC;

	EC = Command("modulator data show");
	if (EC == MC_OK)
	{
		ParseDataRate(DataRate);
	}

	GoTop();
	return EC;
}

//virtual
MC_ErrorCode CSdmsAPI::SetRDataRate(unsigned int &DataRate, int Demodulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	m_pszReply[0]=0;
	m_bReady = FALSE;
	sprintf(m_pszCommand, "demodulator data bit-rate %d", DataRate);
	EC = Command(m_pszCommand, 1000);
	if (EC != MC_OK)
		return EC;

	EC = Command("demodulator data show");
	if (EC == MC_OK)
	{
		ParseDataRate(DataRate);
	}

	GoTop();
	return EC;
}

static const char *SDMS_R_CLOCK_SOURCES[] = {"RCV_Clock", "Internal", "External", "Mod_Clock"};
static const char *SDMS_T_CLOCK_SOURCES[] = {"Internal", "TT_Clock", "External", "RCV_Clock"};

//virtual
int CSdmsAPI::GetRDataClockSourcesCount()
{
	return sizeof(SDMS_R_CLOCK_SOURCES)/sizeof(SDMS_R_CLOCK_SOURCES[0]);
}

//virtual
const char *CSdmsAPI::GetRDataClockSourceName(int source)
{
	return SDMS_R_CLOCK_SOURCES[source];
}

//virtual
int CSdmsAPI::GetTDataClockSourcesCount()
{
	return sizeof(SDMS_T_CLOCK_SOURCES)/sizeof(SDMS_T_CLOCK_SOURCES[0]);
}

//virtual
const char *CSdmsAPI::GetTDataClockSourceName(int source)
{
	return SDMS_T_CLOCK_SOURCES[source];
}

//virtual
MC_ErrorCode CSdmsAPI::SetTDataClockSource(int &Source, int Modulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	m_pszReply[0] = 0;
	m_bReady = FALSE;
	sprintf(m_pszCommand, "modulator data clock-source %s", SDMS_T_CLOCK_SOURCES[Source]);
	EC = Command(m_pszCommand, 1000);
	if (EC != MC_OK)
		return EC;

	EC = Command("modulator data show");
	if (EC == MC_OK)
	{
		ParseTClockSource(Source);
	}

	GoTop();
	return EC;
}

//virtual
MC_ErrorCode CSdmsAPI::SetRDataClockSource(int &Source, int Modulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	m_pszReply[0] = 0;
	m_bReady = FALSE;
	sprintf(m_pszCommand, "demodulator data clock-source %s", SDMS_R_CLOCK_SOURCES[Source]);
	EC = Command(m_pszCommand, 1000);
	if (EC != MC_OK)
		return EC;

	EC = Command("demodulator data show");
	if (EC == MC_OK)
	{
		ParseRClockSource(Source);
	}

	GoTop();
	return EC;
}

// LNB
//virtual
MC_ErrorCode CSdmsAPI::GetLnbStatus(CLnbStatus &status, int Demodulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	EC = Command("demodulator lnb show");
	if (EC == MC_OK)
		ParseDemodulatorLnbShow(status);

	GoTop();
	return EC;
}

//virtual
void CSdmsAPI::ParseDemodulatorLnbShow(CLnbStatus &status)
{
	char *pszResult = strstr(m_pszReply, "Current Out");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		if (pszResult)
			status.m_Current = atof(pszResult)/1000; //mA to A
	}
	pszResult = strstr(m_pszReply, "Current Max");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		if (pszResult)
			status.m_CurrentMax = atof(pszResult)/1000; //mA to A
	}
	pszResult = strstr(m_pszReply, "Current Min");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		if (pszResult)
			status.m_CurrentMin = atof(pszResult)/1000; //mA to A
	}
}

// BUC
//virtual
MC_ErrorCode CSdmsAPI::GetBucStatus(CBucStatus &status, int Modulator)
{
	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = ModemPSM();
	if (EC != MC_OK)
		return EC;

	EC = Command("modulator buc show");
	if (EC == MC_OK)
		ParseModulatorBucShow(status);

	GoTop();
	return EC;
}

//virtual
void CSdmsAPI::ParseModulatorBucShow(CBucStatus &status)
{
	char *pszResult = strstr(m_pszReply, "Voltage Out");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		if (pszResult)
			status.m_Voltage = atof(pszResult);
	}

	pszResult = strstr(m_pszReply, "Current Out");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		if (pszResult)
			status.m_Current = atof(pszResult);
	}
	pszResult = strstr(m_pszReply, "Current Min");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		if (pszResult)
			status.m_CurrentMin = atof(pszResult);
	}
	pszResult = strstr(m_pszReply, "Current Max");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		if (pszResult)
			status.m_CurrentMax = atof(pszResult);
	}
}

//virtual
MC_ErrorCode CSdmsAPI::GetTranceiverStatus()
{
	char *pszResult;
	
	m_TranceiverTemperature = -273.15;
	m_TranceiverPower = -1;

	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;

	EC = Command("device amplifier wavesat");
	if (EC != MC_OK)
		return EC;

	EC = Command("show");

	// Temperature
	pszResult = strstr(m_pszReply, "Temperature");
	if (pszResult)
	{
		pszResult = strchr(pszResult, ':');
		if (pszResult)
			sscanf(pszResult, ": %lf", &m_TranceiverTemperature);
	}
	else
		EC = MC_DEVICE_NOT_RESPONDING;
	
	// Power
	pszResult = strstr(m_pszReply, "power");
	if (pszResult)
	{
		pszResult = strchr(pszResult, ':');
		if (pszResult)
			sscanf(pszResult, ": %lf", &m_TranceiverPower);
	}
	else
		EC = MC_DEVICE_NOT_RESPONDING;

	GoTop();
	return EC;
}

//virtual
void CSdmsAPI::ParseSerialInterfaceStatus(SInterfaceStatus &status)
{
	char *pszResult = strstr(m_pszReply, "Input");
	if (pszResult)
	{
		while (*pszResult != 0 && !isdigit(*pszResult))
			pszResult++;
		// Input packets
		status.InputPackets = _atoi64(pszResult);
		// Input bytes
		pszResult = GetNextNumberChar(pszResult);
		status.InputBytes = _atoi64(pszResult);
		// Input errors
		pszResult = GetNextNumberChar(pszResult);
		status.InputErrors = _atoi64(pszResult);

		// Output packets
		pszResult = GetNextNumberChar(pszResult);
		status.OutputPackets = _atoi64(pszResult);
		// Output bytes
		pszResult = GetNextNumberChar(pszResult);
		status.OutputBytes = _atoi64(pszResult);
		// Output errors
		pszResult = GetNextNumberChar(pszResult);
		status.OutputErrors = _atoi64(pszResult);
	}
	
	// Input drops
	pszResult = strstr(m_pszReply, "Input queue");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		status.InputDrops = _atoi64(pszResult);
	}
	
	// Output drops
	pszResult = strstr(m_pszReply, "Output queue");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		// Skip "classes"
		pszResult = GetNextNumberChar(pszResult);
		// Skip bps
		pszResult = GetNextNumberChar(pszResult);
		// Skip pcts
		pszResult = GetNextNumberChar(pszResult);
		status.OutputDrops = _atoi64(pszResult);
	}
}	

//virtual
void CSdmsAPI::ParseEthernetInterfaceStatus(SInterfaceStatus &status)
{
	char *pszResult = strstr(m_pszReply, "Input");
	if (pszResult)
	{
		while (*pszResult != 0 && !isdigit(*pszResult))
			pszResult++;
		// Input packets
		status.InputPackets = _atoi64(pszResult);
		// Input bytes
		pszResult = GetNextNumberChar(pszResult);
		status.InputBytes = _atoi64(pszResult);
		// Input errors
		pszResult = GetNextNumberChar(pszResult);
		status.InputErrors = _atoi64(pszResult);

		// Output packets
		pszResult = GetNextNumberChar(pszResult);
		status.OutputPackets = _atoi64(pszResult);
		// Output bytes
		pszResult = GetNextNumberChar(pszResult);
		status.OutputBytes = _atoi64(pszResult);
		// Output errors
		pszResult = GetNextNumberChar(pszResult);
		status.OutputErrors = _atoi64(pszResult);
	}
	
	// Input drops
	pszResult = strstr(m_pszReply, "Input queue");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		status.InputDrops = _atoi64(pszResult);
	}
	
	// Output drops
	pszResult = strstr(m_pszReply, "Output queue");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		// Skip queue size
		pszResult = GetNextNumberChar(pszResult);
		// Skip max queue size
		pszResult = GetNextNumberChar(pszResult);
		status.OutputDrops = _atoi64(pszResult);
	}
}	

MC_ErrorCode CSdmsAPI::GetSerialInterfaceStatus(SInterfaceStatus &status, SSerialErrors &errors)
{
	memset(&status, 0xFF, sizeof(status));
	memset(&errors, 0xFF, sizeof(errors));

	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	if (m_cCurrentPrompt != '#')
		Enable();

	EC = Command("show interface serial 0");
	if (EC != MC_OK)
		return EC;
	ParseSerialInterfaceStatus(status);
	ParseSerialInterfaceReceiveErrors(errors);
	ParseSerialInterfaceSendErrors(errors);

	return MC_OK;
}

//virtual
void CSdmsAPI::ParseSerialInterfaceReceiveErrors(SSerialErrors &errors)
{
	char *pszResult = strstr(m_pszReply, "Receive errors:");
	if (pszResult)
	{
		// ReceiveCRC
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveCRC = _atoi64(pszResult);
		// ReceiveOverrun
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveOverrun = _atoi64(pszResult);
		// ReceiveLostCarrier
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveLostCarrier = _atoi64(pszResult);
		// ReceiveFrameViolations
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveFrameViolations = _atoi64(pszResult);
		// ReceiveNonOctet
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveNonOctet = _atoi64(pszResult);
		// ReceiveAbortSequence
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveAbortSequence = _atoi64(pszResult);
		// ReceiveDpll
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveDpll = _atoi64(pszResult);
	}
}

//virtual
void CSdmsAPI::ParseSerialInterfaceSendErrors(SSerialErrors &errors)
{
	char *pszResult = strstr(m_pszReply, "Send errors:");
	if (pszResult)
	{
		// SendUnderrun
		pszResult = GetNextNumberChar(pszResult);
		errors.SendUnderrun = _atoi64(pszResult);
		// SendLostCTS
		pszResult = GetNextNumberChar(pszResult);
		errors.SendLostCTS = _atoi64(pszResult);
	}
}


MC_ErrorCode CSdmsAPI::GetEthernetInterfaceStatus(SInterfaceStatus &status, SEthernetErrors &errors)
{
	memset(&status, 0xFF, sizeof(status));
	memset(&errors, 0xFF, sizeof(errors));

	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	if (m_cCurrentPrompt != '#')
		Enable();

	EC = Command("show interface ethernet 0");
	if (EC != MC_OK)
		return EC;
	ParseEthernetInterfaceStatus(status);
	ParseEthernetInterfaceReceiveErrors(errors);
	ParseEthernetInterfaceSendErrors(errors);

	return MC_OK;
}

//virtual
void CSdmsAPI::ParseEthernetInterfaceReceiveErrors(SEthernetErrors &errors)
{
	char *pszResult = strstr(m_pszReply, "Receive errors:");
	if (pszResult)
	{
		// Receive short packets
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveShort = _atoi64(pszResult);
		// Receive giant packets
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveGiant = _atoi64(pszResult);
		// Receive non-octets
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveNonOctet = _atoi64(pszResult);
		// Receive bad CRC
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveCRC = _atoi64(pszResult);
		// Receive overruns
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveOverrun = _atoi64(pszResult);
		// Receive collisions
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveCollision = _atoi64(pszResult);
		// Receive discarded
		pszResult = GetNextNumberChar(pszResult);
		errors.ReceiveDiscarded = _atoi64(pszResult);
	}
}

//virtual
void CSdmsAPI::ParseEthernetInterfaceSendErrors(SEthernetErrors &errors)
{
	char *pszResult = strstr(m_pszReply, "Send errors:");
	if (!pszResult)
		pszResult = strstr(m_pszReply, "Send  errors:");
	if (pszResult)
	{
		// SendUnderrun
		pszResult = GetNextNumberChar(pszResult);
		errors.SendUnderrun = _atoi64(pszResult);
		// SendLateCollisions
		pszResult = GetNextNumberChar(pszResult);
		errors.SendLateCollisions = _atoi64(pszResult);
		// SendDeffered
		pszResult = GetNextNumberChar(pszResult);
		errors.SendDeffered = _atoi64(pszResult);
		// SendLostCarrier
		pszResult = GetNextNumberChar(pszResult);
		errors.SendLostCarrier = _atoi64(pszResult);
		// SendHeartBeat
		pszResult = GetNextNumberChar(pszResult);
		errors.SendHeartBeat = _atoi64(pszResult);
		// SendRetries
		pszResult = GetNextNumberChar(pszResult);
		errors.SendRetries = _atoi64(pszResult);
		// SendRetransmitLimits
		pszResult = GetNextNumberChar(pszResult);
		errors.SendRetransmitLimits = _atoi64(pszResult);
	}
}

//virtual
MC_ErrorCode CSdmsAPI::GetEthernetIpAddress(char *pszIpAddress, char *pszIpSubnetMask)
{
	pszIpAddress[0] = 0; // empty string
	pszIpSubnetMask[0] = 0; // empty string
	MC_ErrorCode EC = Enable();
	if (EC != MC_OK)
		return EC;
	EC = Command("show interface ethernet 0");
	if (EC != MC_OK)
		return EC;
	char *pszReply = GetMCReply();
	char *pszOurSymbol = strstr(pszReply, "IP address");
	if (!pszOurSymbol)
		return MC_OK; // all addresses are removed
	// copy this line to temp buffer
	char pszLine[256];
	memset(pszLine, 0, sizeof(pszLine));
	int i = 0;
	while (i < sizeof(pszLine)/sizeof(pszLine[0]) && (*(pszOurSymbol+i) != 0) && (*(pszOurSymbol+i) != '\n') && (*(pszOurSymbol+i) != '\r'))
	{
		pszLine[i] = pszOurSymbol[i];
		if (pszLine[i] == '/') // replace slash symbol with blank one for easier parsing
			pszLine[i] = ' ';
		i++;
	}
	char *pszParsingStart = GetNextNumberChar(pszLine);
	int SubnetMaskLength = 0;
	sscanf(pszParsingStart, "%s %d", pszIpAddress, &SubnetMaskLength);
	struct in_addr in;
	unsigned long bits = 0;
	for (i = 0; i < SubnetMaskLength; i++)
		bits |= (1 << (31-i));
	in.S_un.S_un_b.s_b1 = HIBYTE(HIWORD(bits));
	in.S_un.S_un_b.s_b2 = LOBYTE(HIWORD(bits));
	in.S_un.S_un_b.s_b3 = HIBYTE(LOWORD(bits));
	in.S_un.S_un_b.s_b4 = LOBYTE(LOWORD(bits));
	char *pszMask = inet_ntoa(in);
	strcpy(pszIpSubnetMask, pszMask);

	return MC_OK;
}

MC_ErrorCode CSdmsAPI::GetIpStatistics(SIpStatistics &statistics)
{
	memset(&statistics, 0xFF, sizeof(statistics));

	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	if (m_cCurrentPrompt != '#')
		Enable();

	EC = Command("show ip statistics");
	if (EC != MC_OK)
		return EC;

	// TotalPacketsReceived
	char *pszResult = strstr(m_pszReply, "Total packets received");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TotalPacketsReceived = _atoi64(pszResult);
	}
	// Bad checksums
	pszResult = strstr(m_pszReply, "Bad checksums");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BadCRC = _atoi64(pszResult);
	}
	// Packets too short
	pszResult = strstr(m_pszReply, "Packets too short");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsTooShort = _atoi64(pszResult);
	}
	// Not enough data
	pszResult = strstr(m_pszReply, "Not enough data");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.NotEnoughData = _atoi64(pszResult);
	}
	// IP header too short
	pszResult = strstr(m_pszReply, "IP header too short");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.IpHeaderTooShort = _atoi64(pszResult);
	}
	// IP packet smaller than IP hdr
	pszResult = strstr(m_pszReply, "IP packet smaller than IP hdr");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.IpPacketSmallerThanIpHeader = _atoi64(pszResult);
	}
	// Fragments received
	pszResult = strstr(m_pszReply, "Fragments received");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.FragmentsReceived = _atoi64(pszResult);
	}
	// Fragments dropped
	pszResult = strstr(m_pszReply, "Fragments dropped");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.FragmentsDropped = _atoi64(pszResult);
	}
	// Fragments timeouted
	pszResult = strstr(m_pszReply, "Fragments timeouted");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.FragmentsTimeouted = _atoi64(pszResult);
	}
	// Packets forwarded
	pszResult = strstr(m_pszReply, "Packets forwarded");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsForwarded = _atoi64(pszResult);
	}
	// Packets NOT forwarded
	pszResult = strstr(m_pszReply, "Packets NOT forwarded");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsNotForwarded = _atoi64(pszResult);
	}
	// Redirects sent
	pszResult = strstr(m_pszReply, "Redirects sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RedirectsSent = _atoi64(pszResult);
	}
	// Unknown protocol
	pszResult = strstr(m_pszReply, "Unknown protocol");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.UnknownProtocol = _atoi64(pszResult);
	}
	// Delivered to kernel
	pszResult = strstr(m_pszReply, "Delivered to kernel");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DeliveredToKernel = _atoi64(pszResult);
	}
	// Locally generated output
	pszResult = strstr(m_pszReply, "Locally generated output");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.LocallyGeneratedOutput = _atoi64(pszResult);
	}
	// Lost packets
	pszResult = strstr(m_pszReply, "Lost packets");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.LostPackets = _atoi64(pszResult);
	}
	// ReasseMbled (mistype in shell is fixed here)
	pszResult = strstr(m_pszReply, "Reassembled");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.Reassembled = _atoi64(pszResult);
	}
	else
	{
		pszResult = strstr(m_pszReply, "Reassebled");
		if (pszResult)
		{
			pszResult = GetNextNumberChar(pszResult);
			statistics.Reassembled = _atoi64(pszResult);
		}
	}
	// Fragmented
	pszResult = strstr(m_pszReply, "Fragmented");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.Fragmented = _atoi64(pszResult);
	}
	// Output fragments
	pszResult = strstr(m_pszReply, "Output fragments");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.OutputFragments = _atoi64(pszResult);
	}
	// Can't fragment
	pszResult = strstr(m_pszReply, "Can't fragment");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.CannotFragment = _atoi64(pszResult);
	}
	// Bad IP options
	pszResult = strstr(m_pszReply, "Bad IP options");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BadIpOptions = _atoi64(pszResult);
	}
	// No route drops
	pszResult = strstr(m_pszReply, "No route drops");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.NoRouteDrops = _atoi64(pszResult);
	}
	// Bad IP version
	pszResult = strstr(m_pszReply, "Bad IP version");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BadIpVersion = _atoi64(pszResult);
	}
	// Raw IP packets generated
	pszResult = strstr(m_pszReply, "Raw IP packets generated");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RawIpPacketsGenerated = _atoi64(pszResult);
	}
	// Too long IP packets
	pszResult = strstr(m_pszReply, "Too long IP packets");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TooLongPackets = _atoi64(pszResult);
	}
	// Access denied by FW
	pszResult = strstr(m_pszReply, "Access denied by FW");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.AccessDeniedByFW = _atoi64(pszResult);
	}
	// Switched to another interface
	pszResult = strstr(m_pszReply, "Switched to another interface");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.SwitchedToAnotherInterface = _atoi64(pszResult);
	}
	// Dropped while switching
	pszResult = strstr(m_pszReply, "Dropped while switching");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DroppedWhileSwitching = _atoi64(pszResult);
	}
	return MC_OK;
}

MC_ErrorCode CSdmsAPI::GetIcmpStatistics(SIcmpStatistics &statistics)
{
	memset(&statistics, 0xFF, sizeof(statistics));

	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	if (m_cCurrentPrompt != '#')
		Enable();

	EC = Command("show icmp statistics");
	if (EC != MC_OK)
		return EC;

	// Calls to icmp_error()
	char *pszResult = strstr(m_pszReply, "Calls to icmp_error()");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.CallsToIcmpError = _atoi64(pszResult);
	}
	// No error because old ip too short
	pszResult = strstr(m_pszReply, "No error because old ip too short");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.NoErrorBecauseOldIpTooShort = _atoi64(pszResult);
	}
	// No error because old packet was ICMP
	pszResult = strstr(m_pszReply, "No error because old packet was ICMP");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.NoErrorBecauseOldPacketWasIcmp = _atoi64(pszResult);
	}
	// ICMP code out of range received
	pszResult = strstr(m_pszReply, "ICMP code out of range received");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.IcmpCodeOutOfRangeReceived = _atoi64(pszResult);
	}
	// Too short packets
	pszResult = strstr(m_pszReply, "Too short packets");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TooShortPackets = _atoi64(pszResult);
	}
	// Bad checksum
	pszResult = strstr(m_pszReply, "Bad checksum");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BadChecksum = _atoi64(pszResult);
	}
	// Calculated bounds mismatch
	pszResult = strstr(m_pszReply, "Calculated bounds mismatch");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.CalculatedBoundsMismatch = _atoi64(pszResult);
	}
	// Number of responses
	pszResult = strstr(m_pszReply, "Number of responses");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.NumberOfResponses = _atoi64(pszResult);
	}
	// Broadcast/Multicast echo requests dropped
	pszResult = strstr(m_pszReply, "Broadcast/Multicast echo requests dropped");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BroadcastMulticastEchoRequestsDropped = _atoi64(pszResult);
	}
	// Broadcast/Multicast timestamp requests dropped
	pszResult = strstr(m_pszReply, "Broadcast/Multicast timestamp requests dropped");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BroadcastMulticastTimestampRequestsDropped = _atoi64(pszResult);
	}
	// ECHO-REPLY
	pszResult = strstr(m_pszReply, "ECHO-REPLY");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdEchoReply = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentEchoReply = _atoi64(pszResult);
	}
	// UNREACH
	pszResult = strstr(m_pszReply, "UNREACH");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdUnreach = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentUnreach = _atoi64(pszResult);
	}
	// SOURCE-QUENCH
	pszResult = strstr(m_pszReply, "SOURCE-QUENCH");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdSourceQuench = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentSourceQuench = _atoi64(pszResult);
	}
	// REDIRECT
	pszResult = strstr(m_pszReply, "REDIRECT");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdRedirect = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentRedirect = _atoi64(pszResult);
	}
	// ECHO
	pszResult = strstr(m_pszReply, "ECHO ");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdEcho = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentEcho = _atoi64(pszResult);
	}
	// ROUTER-ADVERT
	pszResult = strstr(m_pszReply, "ROUTER-ADVERT");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdRouterAdvert = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentRouterAdvert = _atoi64(pszResult);
	}
	// ROUTER-SOLICIT
	pszResult = strstr(m_pszReply, "ROUTER-SOLICIT");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdRouterSolicit = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentRouterSolicit = _atoi64(pszResult);
	}
	// TTL-EXCEED
	pszResult = strstr(m_pszReply, "TTL-EXCEED");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdTtlExceed = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentTtlExceed = _atoi64(pszResult);
	}
	// PARAM-PROBE
	pszResult = strstr(m_pszReply, "PARAM-PROBE");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdParamProbe = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentParamProbe = _atoi64(pszResult);
	}
	// TIME-STAMP
	pszResult = strstr(m_pszReply, "TIME-STAMP ");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdTimeStamp = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentTimeStamp = _atoi64(pszResult);
	}
	// TIME-STAMP-REPLY
	pszResult = strstr(m_pszReply, "TIME-STAMP-REPLY");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdTimeStampReply = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentTimeStampReply = _atoi64(pszResult);
	}
	// INFORMATION
	pszResult = strstr(m_pszReply, "INFORMATION ");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdInformation = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentInformation = _atoi64(pszResult);
	}
	// INFORMATION-REPLY
	pszResult = strstr(m_pszReply, "INFORMATION-REPLY");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdInformationReply = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentInformationReply = _atoi64(pszResult);
	}
	// MASK
	pszResult = strstr(m_pszReply, "MASK ");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdMask = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentMask = _atoi64(pszResult);
	}
	// MASK-REPLY
	pszResult = strstr(m_pszReply, "MASK-REPLY");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdMaskReply = _atoi64(pszResult);
		pszResult = GetNextNumberChar(pszResult);
		statistics.SentMaskReply = _atoi64(pszResult);
	}
	return MC_OK;
}

MC_ErrorCode CSdmsAPI::GetUdpStatistics(SUdpStatistics &statistics)
{
	memset(&statistics, 0xFF, sizeof(statistics));

	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	if (m_cCurrentPrompt != '#')
		Enable();

	EC = Command("show udp statistics");
	if (EC != MC_OK)
		return EC;

	// Total input packets
	char *pszResult = strstr(m_pszReply, "Total input packets");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TotalInputPackets = _atoi64(pszResult);
	}
	// Packet shorter than header
	pszResult = strstr(m_pszReply, "Packet shorter than header");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsShorterThanHeader = _atoi64(pszResult);
	}
	// Checksum error
	pszResult = strstr(m_pszReply, "Checksum error");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ChecksumErrors = _atoi64(pszResult);
	}
	// Data length larger than packet
	pszResult = strstr(m_pszReply, "Data length larger than packet");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DataLengthLargerThanPacket = _atoi64(pszResult);
	}
	// No socket on port
	pszResult = strstr(m_pszReply, "No socket on port");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.NoSocketOnPort = _atoi64(pszResult);
	}
	// Of above, arrived as broadcast
	pszResult = strstr(m_pszReply, "arrived as broadcast");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.OfAboveArrivedAsBroadcast = _atoi64(pszResult);
	}
	// Not delivered, input socket full
	pszResult = strstr(m_pszReply, "Not delivered, input socket full");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.NotDeliveredInputSocketFull = _atoi64(pszResult);
	}
	// Input packets missing pcb cache
	pszResult = strstr(m_pszReply, "Input packets missing pcb cache");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.InputPacketsMissingPcbCache = _atoi64(pszResult);
	}
	// Input packets not for hashed pcb
	pszResult = strstr(m_pszReply, "Input packets not for hashed pcb");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.InputPacketsNotForHashedPcb = _atoi64(pszResult);
	}
	// Total output packets
	pszResult = strstr(m_pszReply, "Total output packets");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TotalOutputPackets = _atoi64(pszResult);
	}
	return MC_OK;
}

MC_ErrorCode CSdmsAPI::GetTcpStatistics(STcpStatistics &statistics)
{
	memset(&statistics, 0xFF, sizeof(statistics));

	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	if (m_cCurrentPrompt != '#')
		Enable();

	EC = Command("show tcp statistics");
	if (EC != MC_OK)
		return EC;

	// Connections initiated
	char *pszResult = strstr(m_pszReply, "Connections initiated");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ConnectionsInitiated = _atoi64(pszResult);
	}
	// Connections accepted
	pszResult = strstr(m_pszReply, "Connections accepted");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ConnectionsAccepted = _atoi64(pszResult);
	}
	// Connections established
	pszResult = strstr(m_pszReply, "Connections established");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ConnectionsEstablished = _atoi64(pszResult);
	}
	// Connections dropped
	pszResult = strstr(m_pszReply, "Connections dropped");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ConnectionsDropped = _atoi64(pszResult);
	}
	// Embryonic connections dropped
	pszResult = strstr(m_pszReply, "Embryonic connections dropped");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.EmbryonicConnectionsDropped = _atoi64(pszResult);
	}
	// Conn. closed
	pszResult = strstr(m_pszReply, "Conn. closed");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ConnectionsClosed = _atoi64(pszResult);
	}
	// Segs where we tried to get rtt
	pszResult = strstr(m_pszReply, "Segs where we tried to get rtt");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.SegsWhereWeTriedToGetRTT = _atoi64(pszResult);
	}
	// Times we succeeded
	pszResult = strstr(m_pszReply, "Times we succeeded");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimesWeSucceeded = _atoi64(pszResult);
	}
	// Delayed acks sent
	pszResult = strstr(m_pszReply, "Delayed acks sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DelayedAcksSent = _atoi64(pszResult);
	}
	// Conn. dropped in rxmt timeout
	pszResult = strstr(m_pszReply, "Conn. dropped in rxmt timeout");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ConnectionsDroppedInRxmtTimeout = _atoi64(pszResult);
	}
	// Retransmit timeouts
	pszResult = strstr(m_pszReply, "Retransmit timeouts");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RetransmitTimeouts = _atoi64(pszResult);
	}
	// Persist timeouts
	pszResult = strstr(m_pszReply, "Persist timeouts");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PersistTimeouts = _atoi64(pszResult);
	}
	// Keepalive timeouts
	pszResult = strstr(m_pszReply, "Keepalive timeouts");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.KeepaliveTimeouts = _atoi64(pszResult);
	}
	// Keepalive probes sent
	pszResult = strstr(m_pszReply, "Keepalive probes sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.KeepaliveProbesSent = _atoi64(pszResult);
	}
	// Connections dropped in keepalive
	pszResult = strstr(m_pszReply, "Connections dropped in keepalive");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ConnectionsDroppedInKeepalive = _atoi64(pszResult);
	}
	// Total packets sent
	pszResult = strstr(m_pszReply, "Total packets sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TotalPacketsSent = _atoi64(pszResult);
	}
	// Data packets sent
	pszResult = strstr(m_pszReply, "Data packets sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DataPacketsSent = _atoi64(pszResult);
	}
	// Data bytes sent
	pszResult = strstr(m_pszReply, "Data bytes sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DataBytesSent = _atoi64(pszResult);
	}
	// Data packets retransmitted
	pszResult = strstr(m_pszReply, "Data packets retransmitted");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DataPacketsRetransmitted = _atoi64(pszResult);
	}
	// Data bytes retransmitted
	pszResult = strstr(m_pszReply, "Data bytes retransmitted");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DataBytesRetransmitted = _atoi64(pszResult);
	}
	// Ack-only packets sent
	pszResult = strstr(m_pszReply, "Ack-only packets sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.AckOnlyPacketsSent = _atoi64(pszResult);
	}
	// Window probes sent
	pszResult = strstr(m_pszReply, "Window probes sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.WindowProbesSent = _atoi64(pszResult);
	}
	// Packets sent with URG only
	pszResult = strstr(m_pszReply, "Packets sent with URG only");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsSentWithUrgOnly = _atoi64(pszResult);
	}
	// Window update-only packets sent
	pszResult = strstr(m_pszReply, "Window update-only packets sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.WindowUpdateOnlyPacketsSent = _atoi64(pszResult);
	}
	// Control (SYN|FIN|RST) packets sent
	pszResult = strstr(m_pszReply, "Control (SYN|FIN|RST) packets sent");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ControlPacketsSent = _atoi64(pszResult);
	}
	// Total packets received
	pszResult = strstr(m_pszReply, "Total packets received");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TotalPacketsReceived = _atoi64(pszResult);
	}
	// Packets received in sequence
	pszResult = strstr(m_pszReply, "Packets received in sequence");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsReceivedInSequence = _atoi64(pszResult);
	}
	// Bytes received in sequence
	pszResult = strstr(m_pszReply, "Bytes received in sequence");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BytesReceivedInSequence = _atoi64(pszResult);
	}
	// Packets received with ccksum errs
	pszResult = strstr(m_pszReply, "Packets received with ccksum errs");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsReceivedWithChecksumError = _atoi64(pszResult);
	}
	// Packets received with bad offset
	pszResult = strstr(m_pszReply, "Packets received with bad offset");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsReceivedWithBadOffset = _atoi64(pszResult);
	}
	// Packets received too short
	pszResult = strstr(m_pszReply, "Packets received too short");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsReceivedTooShort = _atoi64(pszResult);
	}
	// Duplicate-only packets received
	pszResult = strstr(m_pszReply, "Duplicate-only packets received");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DuplicateOnlyPacketsReceived = _atoi64(pszResult);
	}
	// Duplicate-only bytes received
	pszResult = strstr(m_pszReply, "Duplicate-only packets received");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DuplicateOnlyBytesReceived = _atoi64(pszResult);
	}
	// Packets with some duplicate data
	pszResult = strstr(m_pszReply, "Packets with some duplicate data");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsWithSomeDuplicateData = _atoi64(pszResult);
	}
	// Dup. bytes in part-dup. packets
	pszResult = strstr(m_pszReply, "Dup. bytes in part-dup. packets");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.DupBytesInPartDupPackets = _atoi64(pszResult);
	}
	// Out-of-order packets received
	pszResult = strstr(m_pszReply, "Out-of-order packets received");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.OutOfOrderPacketsReceived = _atoi64(pszResult);
	}
	// Out-of-order bytes received
	pszResult = strstr(m_pszReply, "Out-of-order bytes received");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.OutOfOrderBytesReceived = _atoi64(pszResult);
	}
	// Packets with data after window
	pszResult = strstr(m_pszReply, "Packets with data after window");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsWithDataAfterWindow = _atoi64(pszResult);
	}
	// Bytes rcvd after window
	pszResult = strstr(m_pszReply, "Bytes rcvd after window");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BytesRcvdAfterWindow = _atoi64(pszResult);
	}
	// Packets rcvd after "close"
	pszResult = strstr(m_pszReply, "Packets rcvd after \"close\"");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PacketsRcvdAfterClose = _atoi64(pszResult);
	}
	// Rcvd window probe packets
	pszResult = strstr(m_pszReply, "Rcvd window probe packets");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdWindowProbePackets = _atoi64(pszResult);
	}
	// Rcvd duplicate acks
	pszResult = strstr(m_pszReply, "Rcvd duplicate acks");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdDuplicateAcks = _atoi64(pszResult);
	}
	// Rcvd acks for unsent data
	pszResult = strstr(m_pszReply, "Rcvd acks for unsent data");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdAcksForUnsentData = _atoi64(pszResult);
	}
	// Rcvd ack packets
	pszResult = strstr(m_pszReply, "Rcvd ack packets");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdAckPackets = _atoi64(pszResult);
	}
	// Bytes acked by rcvd acks
	pszResult = strstr(m_pszReply, "Bytes acked by rcvd acks");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BytesAckedByRcvdAcks = _atoi64(pszResult);
	}
	// Rcvd window update packets
	pszResult = strstr(m_pszReply, "Rcvd window update packets");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.RcvdWindowUpdatePackets = _atoi64(pszResult);
	}
	// Segments dropped due to PAWS
	pszResult = strstr(m_pszReply, "Segments dropped due to PAWS");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.SegmentsDroppedDueToPAWS = _atoi64(pszResult);
	}
	// Times hdr predict ok for acks
	pszResult = strstr(m_pszReply, "Times hdr predict ok for acks");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimesHdrPredictOkForAcks = _atoi64(pszResult);
	}
	// Times hdr predict ok for data pkts
	pszResult = strstr(m_pszReply, "Times hdr predict ok for data pkts");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimesHdrPredictOkForDataPkts = _atoi64(pszResult);
	}
	// PCB cache misses
	pszResult = strstr(m_pszReply, "PCB cache misses");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.PcbCacheMisses = _atoi64(pszResult);
	}
	// Times cached RTT in route updated
	pszResult = strstr(m_pszReply, "Times cached RTT in route updated");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimesCachedRTTInRouteUpdated = _atoi64(pszResult);
	}
	// Times cached rttvar updated
	pszResult = strstr(m_pszReply, "Times cached rttvar updated");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimesCachedRTTVarUpdated = _atoi64(pszResult);
	}
	// Times cached ssthresh updated
	pszResult = strstr(m_pszReply, "Times cached ssthresh updated");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimesCachedSsThreshUpdated = _atoi64(pszResult);
	}
	// Times RTT initialized from route
	pszResult = strstr(m_pszReply, "Times RTT initialized from route");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimesRTTIninitializedFromRoute = _atoi64(pszResult);
	}
	// Times RTTVAR initialized from rt
	pszResult = strstr(m_pszReply, "Times RTTVAR initialized from rt");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimesRTTVarInitializedFromRT = _atoi64(pszResult);
	}
	// Times ssthresh initialized from rt
	pszResult = strstr(m_pszReply, "Times ssthresh initialized from rt");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimesSsThreshInitializedFromRT = _atoi64(pszResult);
	}
	// Timeout in persist state
	pszResult = strstr(m_pszReply, "Timeout in persist state");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.TimeoutInPersistState = _atoi64(pszResult);
	}
	// Bogus SYN, e.g. premature ACK
	pszResult = strstr(m_pszReply, "Bogus SYN, e.g. premature ACK");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.BogusSYNPrematureACK = _atoi64(pszResult);
	}
	// Resends due to MTU discovery
	pszResult = strstr(m_pszReply, "Resends due to MTU discovery");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ResendsDueToMTUDiscovery = _atoi64(pszResult);
	}
	// Listen queue overflows
	pszResult = strstr(m_pszReply, "Listen queue overflows");
	if (pszResult)
	{
		pszResult = GetNextNumberChar(pszResult);
		statistics.ListenQueueOverflows = _atoi64(pszResult);
	}
	return MC_OK;
}

//virtual
MC_ErrorCode CSdmsAPI::ReadReplyUntilPrompt()
{
	m_pszReply[0]=0;
	m_bReady = FALSE;
	char cSymbol = 0;

	time_t	start, finish;
	time(&start);
	do
	{
		if (GetTerminal()->Read(&cSymbol, 1))
		{
			OnMcSymbolReceived(cSymbol);
		}
		else
		{
			time(&finish);
			if (difftime(finish, start) > 10)
				return MC_DEVICE_NOT_RESPONDING;
		}
		time(&finish);
	} while (!m_bReady && difftime(finish, start) < 10);

//	TRACE("%s", m_pszReply);
	DumpMCReply();
	if (m_bReady)
		return MC_OK;
	return MC_DEVICE_NOT_RESPONDING;
}

//virtual
MC_ErrorCode CSdmsAPI::IsMCConnectionAlive()
{
	MC_ErrorCode EC = Command("");
	if (EC == MC_OK)
		return MC_OK;
	GetTerminal()->Disconnect();
	if (!GetTerminal()->Connect())
		return MC_DEVICE_NOT_RESPONDING;
	return Command("");
}

void CSdmsAPI::DumpMCReply()
{
	if (!IsLogFileEnabled()) return;
	CTime CurrentTime = CTime::GetCurrentTime();
	CString Time = CurrentTime.Format("%dof%B%Y,%A");
	char pszFile[128];
	char pszConnectionName[64];
	strcpy(pszConnectionName, GetConnectionName());
	char *pszComma = strchr(pszConnectionName, ':');
	if (pszComma)
		*pszComma = 0x0;
	sprintf(pszFile, "M&Cdump_%s_%s.dmp", pszConnectionName, LPCTSTR(Time));
	FILE *fd = fopen(pszFile, "a+");
	if (fd)
	{
		Time = CurrentTime.Format("%H:%M:%S");
		fprintf(fd, "%s\n%s", Time, m_pszReply);
	}
	fclose(fd);
}

MC_ErrorCode CSdmsAPI::Exit()
{
	if (m_cCurrentPrompt == '>')
		return MC_OK;

	MC_ErrorCode EC = Disable();
	if (EC != MC_OK)
		return EC;
	if (m_cCurrentPrompt == '>')
		return MC_OK;
	
	return Command("exit");
}

//virtual
MC_ErrorCode CSdmsAPI::TotalReset()
{
	MC_ErrorCode EC = GoTop();
	if (EC != MC_OK)
		return EC;
	return Reload();
}

char *CSdmsAPI::GetMCReply()
{
	int nShift = 0;
	CutPrompt();
	// Skip command echo
	if (strstr(m_pszReply, m_pszCommand))
		nShift = strlen(m_pszCommand);
	return m_pszReply + nShift;
}

MC_ErrorCode CSdmsAPI::Reload()
{
	MC_ErrorCode EC = Enable();
	if (EC != MC_OK)
		return EC;
	return Command("reload");
}

MC_ErrorCode CSdmsAPI::ConfigureTerminal()
{
	if (m_cCurrentPrompt != '#')
		Enable();
	return Command("configure terminal");
}

//virtual
MC_ErrorCode CSdmsAPI::ConfigureEthernet()
{
	MC_ErrorCode EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;
	return Command("interface ethernet 0");
}

//virtual
MC_ErrorCode CSdmsAPI::ConfigureSerial()
{
	MC_ErrorCode EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;
	return Command("interface serial 0");
}

// Ethernet interface
MC_ErrorCode CSdmsAPI::GetEthernetIpAddress(unsigned long &IpAddress, unsigned long &IpSubnetMask)
{
	IpAddress = IpSubnetMask = 0;
	char pszIpAddress[32], pszIpSubnetMask[32];
	MC_ErrorCode EC = GetEthernetIpAddress(pszIpAddress, pszIpSubnetMask);
	if (EC != MC_OK)
		return EC;
	IpAddress = inet_addr(pszIpAddress);
	IpSubnetMask = inet_addr(pszIpSubnetMask);
	return EC;
}

MC_ErrorCode CSdmsAPI::RemoveAllEthernetIpAddresses()
{
	static char szCurrentIpAddress[32];
	static char szCurrentSubnetMask[32];
	while (1)
	{
		MC_ErrorCode EC = GetEthernetIpAddress(szCurrentIpAddress, szCurrentSubnetMask);
		if (EC != MC_OK)
			return EC;
		if (strlen(szCurrentIpAddress) ==0 || strlen(szCurrentSubnetMask) == 0)
			break;
		EC = RemoveEthernetIpAddress(szCurrentIpAddress, szCurrentSubnetMask);
		if (EC != MC_OK)
			return EC;
	}
	return MC_OK;
}

//virtual
MC_ErrorCode CSdmsAPI::GetEthernetMacAddress(char *pszMacAddress)
{
	const char *pszSearchTemplate = "address is";
	pszMacAddress[0] = 0; // empty string
	MC_ErrorCode EC = Enable();
	if (EC != MC_OK)
		return EC;
	EC = Command("show interface ethernet 0");
	if (EC != MC_OK)
		return EC;
	char *pszReply = GetMCReply();
	char *pszOurSymbol = strstr(pszReply, pszSearchTemplate);
	if (!pszOurSymbol)
		return MC_GENERIC_ERROR; // no MAC-address
	pszOurSymbol += strlen(pszSearchTemplate);
	int SymbolsCopied = 0;
	while (*pszOurSymbol != 0 && *pszOurSymbol != CR && *pszOurSymbol != LF)
	{
		if (SymbolsCopied >= 12)
			break; // too many symbols - error!
		if (isdigit(*pszOurSymbol) ||
			isalpha(*pszOurSymbol))
		{
			if (SymbolsCopied != 0 && (SymbolsCopied % 2) == 0)
				strcat(pszMacAddress, ":");
			strncat(pszMacAddress, pszOurSymbol, 1);
			++SymbolsCopied;
		}
		++pszOurSymbol;
	}
	
	return MC_OK;
}

//virtual
MC_ErrorCode CSdmsAPI::SetEthernetMacAddress(const char *pszMacAddress)
{
	MC_ErrorCode EC = ConfigureEthernet();
	if (EC != MC_OK)
		return EC;
	strcpy(m_pszCommand, "mac-address ");
	// convert  mac-address to AB:CD:EF:GH:IJ:KL form
	char *pszOurSymbol = (char *)pszMacAddress;
	int SymbolsCopied = 0;
	while (*pszOurSymbol != 0 && *pszOurSymbol != CR && *pszOurSymbol != LF)
	{
		if (SymbolsCopied >= 12)
			break; // too many symbols - error!
		if (isdigit(*pszOurSymbol) ||
			isalpha(*pszOurSymbol))
		{
			if (SymbolsCopied != 0 && (SymbolsCopied % 2) == 0)
				strcat(m_pszCommand, ":");
			strncat(m_pszCommand, pszOurSymbol, 1);
			++SymbolsCopied;
		}
		++pszOurSymbol;
	}

	EC = Command(m_pszCommand);
	if (EC != MC_OK)
		return EC;
	EC = GoTop();

	return EC;
}

MC_ErrorCode CSdmsAPI::EnableInterface(BOOL bEnable)
{
	m_pszCommand[0] = 0;
	if (bEnable == TRUE)
	{
		strcpy(m_pszCommand, "no ");
	}
	strcat(m_pszCommand, "shutdown");
	return Command(m_pszCommand);
}

MC_ErrorCode CSdmsAPI::EnableEthernetInterface(BOOL &bEnable)
{
	MC_ErrorCode EC = ConfigureEthernet();
	if (EC != MC_OK)
		return EC;

	EnableInterface(bEnable);

	// TODO: Read enable/disable status and return bEnable

	// Temporary code. Set MAC-address (bug: if there is no startup-config, MAC-address can be multy-broadcast, Kirill said)
//	EC = Command("MAC-address 00:22:23:24:25:26");
	// End of temporary code
	
	EC = GoTop();
	return EC;
}

MC_ErrorCode CSdmsAPI::EnableSerialInterface(BOOL &bEnable)
{
	MC_ErrorCode EC = ConfigureSerial();
	if (EC != MC_OK)
		return EC;

	EnableInterface(bEnable);

	// TODO: Read enable/disable status and return bEnable
	EC = GoTop();
	return EC;
}

MC_ErrorCode CSdmsAPI::SetEthernetIpAddress(const char *pszIpAddress, const char *pszIpMask)
{
	// Remove all current IP-adresses first
	MC_ErrorCode EC = RemoveAllEthernetIpAddresses();
	
	EC = ConfigureEthernet();
	if (EC != MC_OK)
		return EC;
	sprintf(m_pszCommand, "ip address %s %s", pszIpAddress, pszIpMask);
	EC = Command(m_pszCommand);
	if (EC != MC_OK)
		return EC;
	EC = GoTop();
	return EC;
}

MC_ErrorCode CSdmsAPI::SetEthernetIpAddress(unsigned long IpAddress, unsigned long IpSubnetMask)
{
	char pszIpAddress[32], pszIpSubnetMask[32];
	AddressToString(pszIpAddress, IpAddress);
	AddressToString(pszIpSubnetMask, IpSubnetMask);
	MC_ErrorCode EC = SetEthernetIpAddress(pszIpAddress, pszIpSubnetMask);
	return EC;	
}

//virtual
MC_ErrorCode CSdmsAPI::RemoveEthernetIpAddress(const char *pszIpAddress, const char *pszIpMask)
{
	MC_ErrorCode EC = Enable();
	if (EC != MC_OK)
		return EC;
	EC = ConfigureEthernet();
	if (EC != MC_OK)
		return EC;
	sprintf(m_pszCommand, "no ip address %s", pszIpAddress);
	EC = Command(m_pszCommand);
	if (EC != MC_OK)
		return EC;
	char *pszError = strstr(GetMCReply(), "% Error");
	if (pszError)
		EC = MC_GENERIC_ERROR;
	GoTop();

	return EC;
}

// Serial interface

MC_ErrorCode CSdmsAPI::GetSerialIpAddress(char *pszIpAddress, char *pszIpSubnetMask)
{
	pszIpAddress[0] = 0; // empty string
	pszIpSubnetMask[0] = 0; // empty string
	MC_ErrorCode EC = Enable();
	if (EC != MC_OK)
		return EC;
	EC = Command("show interface serial 0");
	if (EC != MC_OK)
		return EC;
	char *pszReply = GetMCReply();
	char *pszInternetAddress = strstr(pszReply, "Internet address");
	if (!pszInternetAddress)
		return MC_OK; // all addresses are removed
	sscanf(pszInternetAddress, "Internet address: %s", pszIpAddress);

	char *pszSubnetMask = strstr(pszReply, "Subnet mask");
	if (!pszSubnetMask)
		return MC_OK;
	sscanf(pszSubnetMask, "Subnet mask: %s", pszIpSubnetMask);
	return MC_OK;
}

MC_ErrorCode CSdmsAPI::GetSerialIpAddress(unsigned long &IpAddress, unsigned long &IpSubnetMask)
{
	IpAddress = IpSubnetMask = 0;
	char pszIpAddress[32], pszIpSubnetMask[32];
	MC_ErrorCode EC = GetSerialIpAddress(pszIpAddress, pszIpSubnetMask);
	if (EC != MC_OK)
		return EC;
	IpAddress = inet_addr(pszIpAddress);
	IpSubnetMask = inet_addr(pszIpSubnetMask);
	return EC;
}

MC_ErrorCode CSdmsAPI::SetSerialIpAddress(const char *pszIpAddress, const char *pszIpMask)
{
	static char szCurrentIpAddress[32];
	static char szCurrentIpMask[32];
	MC_ErrorCode EC;
	// Remove all current IP-adresses first
	while (1)
	{
		EC = Enable();
		if (EC != MC_OK)
			return EC;
		EC = Command("show interface serial 1");
		if (EC != MC_OK)
			return EC;
		char *pszReply = GetMCReply();
		char *pszInternetAddress = strstr(pszReply, "Internet address");
		if (!pszInternetAddress)
			break; // all addresses are removed already
		memset(szCurrentIpAddress, 0, sizeof(szCurrentIpAddress));
		sscanf(pszInternetAddress, "Internet address: %s", szCurrentIpAddress);
		char *pszSubnetMask = strstr(pszReply, "Subnet mask");
		memset(szCurrentIpMask, 0, sizeof(szCurrentIpMask));
		sscanf(pszSubnetMask, "Subnet mask: %s", szCurrentIpMask);
		EC = GoTop();
		if (EC != MC_OK)
			return EC;
		EC = RemoveSerialIpAddress(szCurrentIpAddress, szCurrentIpMask);
		if (EC != MC_OK)
			return EC;
	}
	EC = Enable();
	if (EC != MC_OK)
		return EC;
	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;
	EC = Command("interface serial 1");
	if (EC != MC_OK)
		return EC;
	sprintf(m_pszCommand, "ip address %s %s", pszIpAddress, pszIpMask);
	EC = Command(m_pszCommand);
	if (EC != MC_OK)
		return EC;
	EC = GoTop();
	return EC;
}

MC_ErrorCode CSdmsAPI::SetSerialIpAddress(unsigned long IpAddress, unsigned long IpSubnetMask)
{
	char pszIpAddress[32], pszIpSubnetMask[32];
	AddressToString(pszIpAddress, IpAddress);
	AddressToString(pszIpSubnetMask, IpSubnetMask);
	MC_ErrorCode EC = SetSerialIpAddress(pszIpAddress, pszIpSubnetMask);
	return EC;	
}

MC_ErrorCode CSdmsAPI::RemoveSerialIpAddress(const char *pszIpAddress, const char *pszIpMask)
{
	MC_ErrorCode EC = Enable();
	if (EC != MC_OK)
		return EC;
	EC = ConfigureTerminal();
	if (EC != MC_OK)
		return EC;
	EC = Command("interface serial 1");
	if (EC != MC_OK)
		return EC;
	sprintf(m_pszCommand, "no ip address %s %s", pszIpAddress, pszIpMask);
	EC = Command(m_pszCommand);
	if (EC != MC_OK)
		return EC;
	char *pszError = strstr(GetMCReply(), "% Error");
	if (pszError)
		EC = MC_GENERIC_ERROR;
	GoTop();

	return EC;
}

// Software
MC_ErrorCode CSdmsAPI::SetupSoftware
(
	const char *pszFtpServerIpAddress,
	const char *pszFileName,
	const char *pszFtpUserName,
	const char *pszFtpPassword
)
{
	MC_ErrorCode EC = Enable();
	if (EC != MC_OK)
		return EC;
	
	sprintf(m_pszCommand, "copy ftp boot-image %s %s %s %s", pszFtpServerIpAddress, pszFileName, pszFtpUserName, pszFtpPassword);
	EC = Command(m_pszCommand);

	time_t	start, finish;
	time(&start);
	// wait until prompt
	do
	{
		if (ReadReplyUntilPrompt() == MC_OK)
			return MC_OK;
		time(&finish);
	} while (difftime(finish, start) < 60);
	return MC_DEVICE_NOT_RESPONDING;
}

// Startup config
MC_ErrorCode CSdmsAPI::LoadStartupConfig
(
	const char *pszFtpServerIpAddress,
	const char *pszFileName,
	const char *pszFtpUserName,
	const char *pszFtpPassword
)
{
	char szReply1[2048], szReply2[2048];
	memset(szReply1, 0, sizeof(szReply1));
	memset(szReply2, 0, sizeof(szReply2));
	MC_ErrorCode EC = Enable();
	if (EC != MC_OK)
		return EC;
	
	sprintf(m_pszCommand, "copy ftp startup-config %s %s %s %s", pszFtpServerIpAddress, pszFileName, pszFtpUserName, pszFtpPassword);
	EC = Command(m_pszCommand);
	if (EC != MC_OK)
		return EC;
	if (strstr(m_pszReply, "rror"))
		return MC_OK;
	strcpy(szReply1, m_pszReply);

/*	time_t	start, finish;
	time(&start);
	// wait until prompt
	do
	{
		if (ReadReplyUntilPrompt() == MC_OK)
		{
			strcpy(szReply2, m_pszReply);
			EC = MC_OK;
			break;
		}
		time(&finish);
	} while (difftime(finish, start) < 10);*/
	ReadReplyUntilPrompt();
	strcpy(szReply2, m_pszReply);
	strcpy(m_pszReply, szReply1);
	strcat(m_pszReply, szReply2);
	return EC;
}

/////////////////////////////////////////////////////////////////////
// CVsatSnmpSocket

CVsatSnmpSocket::CVsatSnmpSocket()
{
	m_IpAddress = 0;
	m_RequestID = 0;
}

//virtual
CVsatSnmpSocket::~CVsatSnmpSocket()
{
}

LOGICAL CVsatSnmpSocket::SendGetRequest(const std::string &strCommunity, const cSnmpOID &OID)
{
	LOGICAL bRet = SendSnmpGetRequest(m_IpAddress, strCommunity, OID, ++m_RequestID);
	if (bRet)
		portableSleep(200);
	return bRet;
}

LOGICAL CVsatSnmpSocket::SendSetIntegerRequest(const std::string &strCommunity, const cSnmpOID &OID, int iValue)
{
	LOGICAL bRet = SendSnmpSetIntegerRequest(m_IpAddress, strCommunity, OID, ++m_RequestID, iValue);
	if (bRet)
		portableSleep(200);
	return bRet;
}

LOGICAL CVsatSnmpSocket::SendSetUnsignedIntegerRequest(const std::string &strCommunity, const cSnmpOID &OID, unsigned int uiValue)
{
	LOGICAL bRet = SendSnmpSetUnsignedIntegerRequest(m_IpAddress, strCommunity, OID, ++m_RequestID, uiValue);
	if (bRet)
		portableSleep(200);
	return bRet;
}

//virtual
LOGICAL CVsatSnmpSocket::Connect(const char *pszConnectionName)
{
	m_IpAddress = StringToAddress(pszConnectionName);
	SetConnectionName(pszConnectionName);
	return TRUE;
}

//virtual
LOGICAL CVsatSnmpSocket::Write(void *pszBuffer, unsigned int BytesToWrite, unsigned int &nWrittenBytes)
{
	return WriteTo(pszBuffer, BytesToWrite, nWrittenBytes, m_IpAddress, GetDefaultPort());
}

//virtual
LOGICAL CVsatSnmpSocket::Read(void *pszBuffer, unsigned int nBufferSize, unsigned int &nReadBytes)
{
	IPADDRESS_TYPE IpAddress;
	unsigned short PortNo;
	return ReadFrom(pszBuffer, nBufferSize, nReadBytes, IpAddress, PortNo);
}

//virtual
LOGICAL CVsatSnmpSocket::PeekInputSymbols(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes)
{
	return Read(pBufferToFill, nBufferSize, nReadBytes);
}

//virtual
LOGICAL CVsatSnmpSocket::SetReadTimeout(DWORD a_dwTimeout)
{
	return CIpSocket::SetReadTimeout(a_dwTimeout);
}

//virtual
LOGICAL CVsatSnmpSocket::SetWriteTimeout(DWORD a_dwTimeout)
{
	return CIpSocket::SetWriteTimeout(a_dwTimeout);
}


/////////////////////////////////////////////////////////////////////
// CSdmsVsat

//virtual
void CSdmsVsat::OnMcConnected()
{
	if (m_ControlType == MC_SNMP)
	{
		m_SnmpSocket.SetReadTimeout(3000);
		m_SnmpSocket.SetWriteTimeout(3000);
	}
	else
		CSdmsAPI::OnMcConnected();
}

//virtual
CTerminal *CSdmsVsat::GetTerminal()
{
	switch (m_ControlType)
	{
	case MC_UNKNOWN_TYPE:
	case MC_SERIAL_PORT:
	case MC_TELNET:
		return CSdmsAPI::GetTerminal();
	case MC_SNMP:
		return &m_SnmpSocket;
	}
	return NULL;
}

//virtual
MC_ErrorCode CSdmsVsat::GetUpTime(unsigned int &Time)
{
	Time = 0;

	MC_ErrorCode EC = IsMCConnectionAlive();
	if (EC != MC_OK)
		return EC;

	EC = Enable();
	if (EC != MC_OK)
		return EC;

	EC = Command("show uptime");
	if (EC != MC_OK)
		return EC;

	int days = 0, hours = 0, mins = 0, secs = 0;
	char *pszResult = strstr(m_pszReply, "System");
	if (pszResult)
	{
		int nScanfed = sscanf(pszResult, "System up %d days, %d hours, %d minutes %d seconds", &days, &hours, &mins, &secs);
		if (nScanfed >= 3)
		{
			Time = mins + hours*60 + days*24*60;
		}
		else
			return MC_GENERIC_ERROR;
	}
	GoTop();
	return MC_OK;
}

//virtual
MC_ErrorCode CSdmsVsat::IsMCConnectionAlive()
{
	if (m_ControlType == MC_SNMP)
	{
		if (!m_SnmpSocket.IsOpened())
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::IsMCConnectionAlive();

	return MC_OK;
}

//virtual
MC_ErrorCode CSdmsVsat::GetDeviceStatusString(char *&pszString)
{
	pszString = m_szStatus;
	m_szStatus[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;

	MC_ErrorCode EC = Enable();
	if (EC != MC_OK)
		return EC;
	
	EC = Command("show uptime");
	if (EC == MC_OK)
	{
		CutPrompt();
		char *pszSystemInfo = GetMCReply();
		if (pszSystemInfo)
			strcpy(m_szStatus, pszSystemInfo);
	}
	GoTop();
	
	return EC;
}

//virtual
MC_ErrorCode CSdmsVsat::GetTIfParams(CModIfParams &Params, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen));
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfOffset, OidModulatorIfOffsetLen));
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen));
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen));
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfModulation, OidModulatorIfModulationLen));
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen));
			LOGICAL bGotReply = LOGICAL_FALSE, bGotAnyReply = LOGICAL_FALSE;
			cSnmpDatagram reply;
			do
			{
				memset(&reply, 0, sizeof(reply));
				bGotReply = m_SnmpSocket.GetSnmpReply(reply);
				bGotAnyReply |= bGotReply;
				if (reply.m_ErrorCode != 0 || reply.m_StatusCode != 0)
					continue;
				if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen))
				{
					Params.m_Frequency = reply.m_Variable.m_iIntegerValue/1000;
				}
				else if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfOffset, OidModulatorIfOffsetLen))
				{
					Params.m_Offset = reply.m_Variable.m_iIntegerValue;
				}
				else if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen))
				{
					Params.m_OutputLevel = reply.m_Variable.m_iIntegerValue/10.;
				}
				else if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen))
				{
					Params.m_bOutputEnabled = (reply.m_Variable.m_iIntegerValue == 2);
				}
				else if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfModulation, OidModulatorIfModulationLen))
				{
					if (reply.m_Variable.m_iIntegerValue == 1)
						Params.m_ModulationType = 0;
					else
						Params.m_ModulationType = 1;
				}
				else if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen))
				{
					Params.m_SpectrumMode = reply.m_Variable.m_iIntegerValue -1;
				}
			} while (bGotReply);
			if (bGotAnyReply)
				return MC_OK;
			else
				return MC_DEVICE_NOT_RESPONDING;
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::GetTIfParams(Params, Modulator);
}

//MC_ErrorCode IsModulatorOn(BOOL &bOn);
//virtual
MC_ErrorCode CSdmsVsat::SetTFrequency(unsigned int &Frequency, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendSetUnsignedIntegerRequest(pszPrivateCommunity, cSnmpOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen), Frequency*1000);
			return GetTFrequency(Frequency, Modulator);
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::SetTFrequency(Frequency, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::GetTFrequency(unsigned int &Frequency, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen));
			LOGICAL bGotReply = LOGICAL_FALSE, bGotAnyReply = LOGICAL_FALSE;
			cSnmpDatagram reply;
			do
			{
				memset(&reply, 0, sizeof(reply));
				bGotReply = m_SnmpSocket.GetSnmpReply(reply);
				bGotAnyReply |= bGotReply;
				if (reply.m_ErrorCode != 0 || reply.m_StatusCode != 0)
					continue;
				if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen))
				{
					Frequency = reply.m_Variable.m_iIntegerValue/1000;
				}
			} while (bGotReply);
			if (bGotAnyReply)
				return MC_OK;
			else
				return MC_DEVICE_NOT_RESPONDING;
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::GetTFrequency(Frequency, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::SetOutputLevel(double &Level, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		int iLevel = (int)(Level*10);
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendSetIntegerRequest(pszPrivateCommunity, cSnmpOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen), iLevel);
			return GetOutputLevel(Level, Modulator);
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::SetOutputLevel(Level, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::GetOutputLevel(double &Level, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen));
			LOGICAL bGotReply = LOGICAL_FALSE, bGotAnyReply = LOGICAL_FALSE;
			cSnmpDatagram reply;
			do
			{
				memset(&reply, 0, sizeof(reply));
				bGotReply = m_SnmpSocket.GetSnmpReply(reply);
				bGotAnyReply |= bGotReply;
				if (reply.m_ErrorCode != 0 || reply.m_StatusCode != 0)
					continue;
				if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen))
				{
					Level = reply.m_Variable.m_iIntegerValue/10.;
				}
			} while (bGotReply);
			if (bGotAnyReply)
				return MC_OK;
			else
				return MC_DEVICE_NOT_RESPONDING;
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::GetOutputLevel(Level, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::IsOutputOn(BOOL &bOn, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen));
			LOGICAL bGotReply = LOGICAL_FALSE, bGotAnyReply = LOGICAL_FALSE;
			cSnmpDatagram reply;
			do
			{
				memset(&reply, 0, sizeof(reply));
				bGotReply = m_SnmpSocket.GetSnmpReply(reply);
				bGotAnyReply |= bGotReply;
				if (reply.m_ErrorCode != 0 || reply.m_StatusCode != 0)
					continue;
				if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen))
				{
					bOn = (reply.m_Variable.m_iIntegerValue == 2);
				}
			} while (bGotReply);
			if (bGotAnyReply)
				return MC_OK;
			else
				return MC_DEVICE_NOT_RESPONDING;
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::IsOutputOn(bOn, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::TurnOutputOn(BOOL &bOn, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		int iEnable = 1;
		if (bOn)
			iEnable = 2;
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendSetIntegerRequest(pszPrivateCommunity, cSnmpOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen), iEnable);
			return IsOutputOn(bOn, Modulator);
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::TurnOutputOn(bOn, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::GetModulatorShift(int &Shift, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfOffset, OidModulatorIfOffsetLen));
			LOGICAL bGotReply = LOGICAL_FALSE, bGotAnyReply = LOGICAL_FALSE;
			cSnmpDatagram reply;
			do
			{
				memset(&reply, 0, sizeof(reply));
				bGotReply = m_SnmpSocket.GetSnmpReply(reply);
				bGotAnyReply |= bGotReply;
				if (reply.m_ErrorCode != 0 || reply.m_StatusCode != 0)
					continue;
				if (reply.m_Variable.m_OID.isTheSameOID(OidModulatorIfOffset, OidModulatorIfOffsetLen))
				{
					Shift = reply.m_Variable.m_iIntegerValue;
				}
			} while (bGotReply);
			if (bGotAnyReply)
				return MC_OK;
			else
				return MC_DEVICE_NOT_RESPONDING;
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::GetModulatorShift(Shift, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::SetModulatorShift(int &Shift, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendSetIntegerRequest(pszPrivateCommunity, cSnmpOID(OidModulatorIfOffset, OidModulatorIfOffsetLen), Shift);
			return GetModulatorShift(Shift, Modulator);
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::SetModulatorShift(Shift, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::GetRModulationType(int &Type, int Demodulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen));
			LOGICAL bGotReply = LOGICAL_FALSE, bGotAnyReply = LOGICAL_FALSE;
			cSnmpDatagram reply;
			do
			{
				memset(&reply, 0, sizeof(reply));
				bGotReply = m_SnmpSocket.GetSnmpReply(reply);
				bGotAnyReply |= bGotReply;
				if (reply.m_ErrorCode != 0 || reply.m_StatusCode != 0)
					continue;
				if (reply.m_Variable.m_OID.isTheSameOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen))
				{
					Type = reply.m_Variable.m_iIntegerValue - 1;
				}
			} while (bGotReply);
			if (bGotAnyReply)
				return MC_OK;
			else
				return MC_DEVICE_NOT_RESPONDING;
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::GetRModulationType(Type, Demodulator);
}

//virtual
MC_ErrorCode CSdmsVsat::SetRModulationType(int &Type, int Demodulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			int iModulation = Type + 1;
			m_SnmpSocket.SendSetIntegerRequest(pszPrivateCommunity, cSnmpOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen), iModulation);
			return GetRModulationType(Type, Demodulator);
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::SetRModulationType(Type, Demodulator);
}

//virtual
MC_ErrorCode CSdmsVsat::GetTModulationType(int &Type, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			m_SnmpSocket.SendGetRequest(pszPublicCommunity, cSnmpOID(OidModulatorIfModulation, OidModulatorIfModulationLen));
			LOGICAL bGotReply = LOGICAL_FALSE, bGotAnyReply = LOGICAL_FALSE;
			cSnmpDatagram reply;
			do
			{
				memset(&reply, 0, sizeof(reply));
				bGotReply = m_SnmpSocket.GetSnmpReply(reply);
				bGotAnyReply |= bGotReply;
				if (reply.m_ErrorCode != 0 || reply.m_StatusCode != 0)
					continue;
				if (reply.m_Variable.m_OID.isTheSameOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen))
				{
					Type = reply.m_Variable.m_iIntegerValue - 1;
				}
			} while (bGotReply);
			if (bGotAnyReply)
				return MC_OK;
			else
				return MC_DEVICE_NOT_RESPONDING;
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::GetRModulationType(Type, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::SetTModulationType(int &Type, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		if (IsMCConnectionAlive() == MC_OK)
		{
			int iModulation = Type + 1;
			m_SnmpSocket.SendSetIntegerRequest(pszPrivateCommunity, cSnmpOID(OidModulatorIfModulation, OidModulatorIfModulationLen), iModulation);
			return GetTModulationType(Type, Modulator);
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::SetTModulationType(Type, Modulator);
}

//virtual
MC_ErrorCode CSdmsVsat::GetRDataParams(CModemDataParams &Params, int Demodulator)
{
	if (m_ControlType == MC_SNMP)
	{
		return MC_OK;
		if (IsMCConnectionAlive() == MC_OK)
		{
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::GetRDataParams(Params, Demodulator);
}

//virtual
MC_ErrorCode CSdmsVsat::GetTDataParams(CModemDataParams &Params, int Modulator)
{
	if (m_ControlType == MC_SNMP)
	{
		return MC_OK;
		if (IsMCConnectionAlive() == MC_OK)
		{
		}
		else
			return MC_DEVICE_NOT_CONTROLLABLE;
	}
	else
		return CSdmsAPI::GetTDataParams(Params, Modulator);
}

char *CSdmsVsat4900L::m_pszName="SDMS-VSAT-4900L";
char *CSdmsVsatM500L::m_pszName="SDMS-VSAT-M500L";


//////////////////////////////////////////
// class CSdmsVsat4900L

// Reed-Solomon
//virtual
const char *CSdmsVsat4900L::GetReedSolomonModeName(int Mode)
{
	return "";
}

//virtual
int CSdmsVsat4900L::GetReedSolomonModesCount()
{
	return 0;
}

//virtual
MC_ErrorCode CSdmsVsat4900L::GetRReedSolomonMode(int &Mode, int Demodulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CSdmsVsat4900L::SetRReedSolomonMode(int &Mode, int Demodulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CSdmsVsat4900L::GetTReedSolomonMode(int &Mode, int Modulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CSdmsVsat4900L::SetTReedSolomonMode(int &Mode, int Modulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}


//////////////////////////////////////////
// class CSdmsVsatM500L

// Reed-Solomon
//virtual
const char *CSdmsVsatM500L::GetReedSolomonModeName(int Mode)
{
	return "";
}

//virtual
int CSdmsVsatM500L::GetReedSolomonModesCount()
{
	return 0;
}

//virtual
MC_ErrorCode CSdmsVsatM500L::GetRReedSolomonMode(int &Mode, int Demodulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CSdmsVsatM500L::SetRReedSolomonMode(int &Mode, int Demodulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CSdmsVsatM500L::GetTReedSolomonMode(int &Mode, int Modulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}

//virtual
MC_ErrorCode CSdmsVsatM500L::SetTReedSolomonMode(int &Mode, int Modulator)
{
	return MC_COMMAND_NOT_SUPPORTED;
}


