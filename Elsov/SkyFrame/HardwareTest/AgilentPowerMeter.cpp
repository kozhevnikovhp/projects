/*	AgilentPowerMeter.cpp: Monitoring and Control of Agilent power meters implementation.

	Author: Evgeny A. Kozhevnikov Moscow, Russia  evgeny@kozhevnikov.org

	Revision history:
		05 November 2003 - initial creation
	
*/

#include "stdafx.h"
#include "AgilentPowerMeter.h"

const char CR = 0x0D;
const char LF = 0x0A;
static char szCRLF[2] = {CR, LF};


//////////////////////////////////////////////////////////////////////////////
// CAgilentPowerMeter class implementation

CAgilentPowerMeter::CAgilentPowerMeter()
{
}

// virtual
CAgilentPowerMeter::~CAgilentPowerMeter()
{
}

//virtual
MC_ErrorCode CAgilentPowerMeter::Identify(BOOL &bIdentified)
{
	bIdentified = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	bIdentified = TRUE;
	return MC_OK;
}

MC_ErrorCode CAgilentPowerMeter::Command(int CommandLength)
{
	static char szCRLF[2] = {CR, LF};
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	GetTerminal()->ClearAllBuffers();
	GetTerminal()->Write((char *)m_pszCommand, CommandLength);
	GetTerminal()->Write(szCRLF, 2);

	MC_ErrorCode EC = ReadReplyUntilPrompt();
	// Analize error byte
	return EC;
}

//virtual
MC_ErrorCode CAgilentPowerMeter::ReadReplyUntilPrompt()
{
	memset(m_pszReply, 0, sizeof(m_pszReply));
	char cSymbol;
	time_t	start, finish;
	time(&start);
	do
	{
		if (!GetTerminal()->Read(&cSymbol, 1))
			break;
		if (cSymbol == CR || cSymbol == LF)
			break;
		strncat(m_pszReply, &cSymbol, 1);
		time(&finish);
	} while (difftime(finish, start) < 10);
	m_MCReplyLength = strlen(m_pszReply);
	if (m_MCReplyLength == 0)
		return MC_DEVICE_NOT_RESPONDING;
	return MC_OK;
}

//virtual
MC_ErrorCode CAgilentPowerMeter::GetDeviceVersionString(char *&pszString)
{
	pszString = "Agilent power meter";
	return MC_OK;
}

//virtual
void CAgilentPowerMeter::OnMcConnected()
{
	// Turn MC echo off
	strcpy(m_pszCommand, "SYST:COMM:SER:TRAN:ECHO OFF\r\n*cls\r\n");
	SendMC(m_pszCommand, strlen(m_pszCommand));
	ResetMeasurements();
}

//virtual
MC_ErrorCode CAgilentPowerMeter::ResetMeasurements()
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	strcpy(m_pszCommand, "MEAS1?\r\n");
	SendMC(m_pszCommand, strlen(m_pszCommand));
	return MC_OK;
}

//virtual
void CAgilentPowerMeter::OnMcDisconnected()
{
	// Turn MC echo on
	strcpy(m_pszCommand, "SYST:COMM:SER:TRAN:ECHO ON\r\n*cls\r\n");
	SendMC(m_pszCommand, strlen(m_pszCommand));
}

//virtual
MC_ErrorCode CAgilentPowerMeter::MeasurePower(double &Power, int Channel)
{
	if (!IsControllable())
		return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "*CLS\r\nABORT%d\r\nREAD%d?\r\n", Channel, Channel);
	MC_ErrorCode EC = Command(strlen(m_pszCommand));
	sscanf(m_pszReply, "%le", &Power);
	char szTemp[16];
	sprintf(szTemp, "%8.2lf", Power);
	sscanf(szTemp, "%lf", &Power);
	return EC;
}


//////////////////////////////////////////////////////////////////////////////
// CAgilentFrequencyCounter class implementation

CAgilentFrequencyCounter::CAgilentFrequencyCounter()
{
	m_CurrentChannel = -1;
}

// virtual
CAgilentFrequencyCounter::~CAgilentFrequencyCounter()
{
}

//virtual
MC_ErrorCode CAgilentFrequencyCounter::Identify(BOOL &bIdentified)
{
	bIdentified = FALSE;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	bIdentified = TRUE;
	return MC_OK;
}

MC_ErrorCode CAgilentFrequencyCounter::Command(char *pszCommand)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	strcpy(m_pszCommand, pszCommand);
	return Command(strlen(pszCommand));
}

MC_ErrorCode CAgilentFrequencyCounter::Command(int CommandLength)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	GetTerminal()->ClearAllBuffers();
	GetTerminal()->Write((char *)m_pszCommand, CommandLength);
	GetTerminal()->Write(szCRLF, 2);

	MC_ErrorCode EC = ReadReplyUntilPrompt();
	GetTerminal()->ClearAllBuffers();
	// Analize error byte
	return EC;
}

//virtual
MC_ErrorCode CAgilentFrequencyCounter::ReadReplyUntilPrompt()
{
	memset(m_pszReply, 0, sizeof(m_pszReply));
	char cSymbol;
	time_t	start, finish;
	time(&start);
	do
	{
		if (!GetTerminal()->Read(&cSymbol, 1))
			break;
		time(&finish);
		if (cSymbol == '>')
			break;
		strncat(m_pszReply, &cSymbol, 1);
	} while (difftime(finish, start) < 10);
	m_MCReplyLength = strlen(m_pszReply);
	if (m_MCReplyLength == 0)
		return MC_DEVICE_NOT_RESPONDING;
	return MC_OK;
}

//virtual
MC_ErrorCode CAgilentFrequencyCounter::GetDeviceVersionString(char *&pszString)
{
	pszString = m_szVersion;
	m_szVersion[0] = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	strcpy(m_pszCommand, "*IDN?");
	MC_ErrorCode EC = Command("*IDN?");
	strcpy(m_szVersion, m_pszReply);
//	pszString = "Agilent frequency counter";
	return EC;
}

//virtual
void CAgilentFrequencyCounter::OnMcConnected()
{
	ResetMeasurements();
	Command(":INIT:CONT ON\r\n");
}

//virtual
MC_ErrorCode CAgilentFrequencyCounter::ResetMeasurements()
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	MC_ErrorCode EC = Command(":ABORT");
	return EC;
}

//virtual
void CAgilentFrequencyCounter::OnMcDisconnected()
{
}

//virtual
MC_ErrorCode CAgilentFrequencyCounter::MeasureFrequency(double &Frequency, int Channel)
{
	Frequency = 0;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	MC_ErrorCode EC;
	if (Channel != m_CurrentChannel)
	{
		sprintf(m_pszCommand, ":FUNC 'FREQ %d'", Channel);
		EC = Command(m_pszCommand);
		if (EC == MC_OK)
		{
			m_CurrentChannel = Channel;
			Sleep(2000); // Sleep to be sure that channel switching is completed
		}
		else
			return EC;
	}
	sprintf(m_pszCommand, "DATA?\r\n");
	EC = Command(strlen(m_pszCommand));
	char *pszResult = m_pszReply;
	while (*pszResult != 0 && !isdigit(*pszResult))
		++pszResult;
	sscanf(pszResult, "%le", &Frequency);
	return EC;
}


// CAgilentE4418B class implementation

char *CAgilentE4418B::m_pszName = "Agilent E4418B Power Meter";

CAgilentE4418B::CAgilentE4418B()
{
}

// virtual
CAgilentE4418B::~CAgilentE4418B()
{
}


// CAgilent53150A class implementation

char *CAgilent53150A::m_pszName = "Agilent 53150A Frequency Counter";

CAgilent53150A::CAgilent53150A()
{
}

// virtual
CAgilent53150A::~CAgilent53150A()
{
}

