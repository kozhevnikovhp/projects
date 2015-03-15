/* Pozharsky.cpp - implementation file for Pozharsky-designed Up/Down converters interfaces

	Author: Evgeny A. Kozhevnikov, Zelenograd, Moscow, Russia evgeny@kozhevnikov.org

Revision history:
	May 16 2003 - initial creation
	Aug 22 2003 - remote Tcp/Ip control added
	Aug 23 2004 - "Pozharsky" name became invisible :-((
	Oct 13 2004 - newer version (October2004) added
*/

#include "stdafx.h"
#include "Pozharsky.h"

const unsigned char cMasterNodeAddress	= 0xFF;	// master node (personal computer etc)
const unsigned char cPutDataToPort		= 0x02;	// put data to port
const unsigned char cGetDataFromPort	= 0x03;	// get data from port
const unsigned char cOK					= 0x00;	// Command execution OK
const unsigned char cCannotExecute		= 0x01;	// Cannot execute command
const unsigned char cInvalidCommand		= 0x02;	// Invalid command
const unsigned char cInvalidPort		= 0x03;	// Invalid port number
const unsigned char cInvalidData		= 0x04;	// Invalid data
const unsigned char cBadCRC				= 0xFF;	// Bad CRC


// CPozharskyUpDownConverter implementation

CPozharskyUpDownConverter::CPozharskyUpDownConverter()
{
	m_cMCNodeAddress = 0x00;
	m_Port = 0;
	m_CommandCode = 0;
}

CPozharskyUpDownConverter::~CPozharskyUpDownConverter()
{
}

// Common device commands

//virtual
CTerminal *CPozharskyUpDownConverter::GetTerminal()
{
	switch (m_ControlType)
	{
	case MC_UNKNOWN_TYPE:
	case MC_SERIAL_PORT:
		return &m_SerialPort;
	case MC_TELNET:
		return &m_TcpIpSocket;
	}
	return NULL;
}

//virtual
void CPozharskyUpDownConverter::OnMcConnected()
{
	if (m_ControlType == MC_TELNET)
	{
		GetTerminal()->SetWriteTimeout(2000);
		GetTerminal()->SetReadTimeout(2000);
	}
}

//virtual
BOOL CPozharskyUpDownConverter::CheckMCBusAddress(unsigned int Address)
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
	MC_ErrorCode EC = GetUpOutputFrequency(freq);
	if (EC == MC_OK)
		bMyAddress = TRUE;
	SetMCBusAddress(StoredAddress);
	return bMyAddress;
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::WriteConfiguration()
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cDataByte = 0xAA;
	MC_ErrorCode EC = WriteCommand(0x08, cDataByte);
	if (EC != MC_OK)
		return EC;
	return MC_OK;
}

unsigned char CalcCRC(unsigned char *pcDataBytes, unsigned char DataLength)
{
	unsigned char CRC = 0x00;
	for (unsigned int i = 0; i<DataLength; i++)
		CRC ^= pcDataBytes[i];
	return CRC;
}

MC_ErrorCode CPozharskyUpDownConverter::ReadCommand(unsigned char Port, unsigned char &cDataByte)
{
	MC_ErrorCode EC =  Command(cGetDataFromPort, Port, NULL, 0);
	if (EC != MC_OK)
		return EC;
	cDataByte = *m_pReplyData;
	return MC_OK;
}

MC_ErrorCode CPozharskyUpDownConverter::ReadCommand(unsigned char Port, unsigned short &sDataWord)
{
	sDataWord = 0;
	MC_ErrorCode EC = Command(cGetDataFromPort, Port, NULL, 0);
	if (EC != MC_OK)
		return EC;
	if (m_pReplyData == NULL)
	{
//		ASSERT(FALSE); // reading is successful, but m_pReplyData is not assigned
		return MC_GENERIC_ERROR; // prevents crash
	}
	sDataWord = *(m_pReplyData+1); 
	sDataWord += (*m_pReplyData)<<8;
	return MC_OK;
}

MC_ErrorCode CPozharskyUpDownConverter::WriteCommand(unsigned char Port, unsigned char &cDataByte)
{
	MC_ErrorCode EC = Command(cPutDataToPort, Port, &cDataByte, 1);
	if (EC == MC_OK)
		return MC_OK;
	return ReadCommand(Port, cDataByte);
}

MC_ErrorCode CPozharskyUpDownConverter::WriteCommand(unsigned char Port, unsigned short &sDataWord)
{
	unsigned char cDataBytes[2];
	cDataBytes[0] = (unsigned char)((sDataWord & 0xFF00)>>8);
	cDataBytes[1] = (unsigned char)(sDataWord & 0x00FF);
	MC_ErrorCode EC = Command(cPutDataToPort, Port, cDataBytes, 2);
	if (EC == MC_OK)
		return MC_OK;
	return ReadCommand(Port, sDataWord);
}

MC_ErrorCode CPozharskyUpDownConverter::Command(unsigned char CommandCode, unsigned char Port, unsigned char &cDataByte)
{
	MC_ErrorCode EC = Command(CommandCode, Port, &cDataByte, 1);
	if (EC != MC_OK)
		return EC;
	cDataByte = *m_pReplyData;
	return MC_OK;
}

MC_ErrorCode CPozharskyUpDownConverter::Command(unsigned char CommandCode, unsigned char Port, unsigned char *pcDataBytes, unsigned char DataLength)
{
	m_pReplyData = NULL;
	m_Port = Port;
	m_CommandCode = CommandCode;
	
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	GetTerminal()->ClearAllBuffers();

	// Fill command buffer
	m_cCommand[0] = 0xAA;					// synchro byte
	m_cCommand[1] = 0xAB;					// synchro byte
	m_cCommand[2] = 0xAC;					// synchro byte
	m_cCommand[3] = cMasterNodeAddress; 	// src - master node (personal computer etc.)
	m_cCommand[4] = m_cMCNodeAddress;		// dest - slave node (up/down converter)
	if (CommandCode == cGetDataFromPort)
		m_cCommand[5] = 2;				// data length
	else
		m_cCommand[5] = DataLength + 2;
	// Data block CRC
	m_cCommand[6] = CommandCode ^ Port ^ CalcCRC(pcDataBytes, DataLength);
	// Header CRC
	m_cCommand[7] = CalcCRC(m_cCommand, 7);
	// Command code
	m_cCommand[8] = CommandCode;
	// Port number
	m_cCommand[9] = Port;
	// Data copy
	memcpy(m_cCommand+10, pcDataBytes, DataLength);

	// Send command
	for (int j=0; j<8+m_cCommand[5]; j++)
	{
		//TRACE("Command[%d]=\t%d\n", j, m_cCommand[j]);
	}
	GetTerminal()->Write((char *)m_cCommand, 8+m_cCommand[5]);

	MC_ErrorCode EC = ReadReplyUntilPrompt();
	return EC;
}

//virtual
void CPozharskyUpDownConverter::OnBeforeSendMC(char *pszBufferToSend, unsigned int nBytesToSend)
{
	m_cMCNodeAddress = pszBufferToSend[4];
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::ReadReplyUntilPrompt()
{
	m_MCReplyLength = 0;
	//Read reply from device (header first)
	if (!GetTerminal()->Read((char *)m_cReply, 9))
		return MC_DEVICE_NOT_RESPONDING;
	m_MCReplyLength += 9;
	
	if (m_cReply[0] != 0xAA || m_cReply[1] != 0xAB || m_cReply[2] != 0xAC)
		return MC_GENERIC_ERROR;
	if (m_cReply[3] == cMasterNodeAddress)
	{
		if (m_cReply[4] != m_cMCNodeAddress)
			return MC_INVALID_ADDRESS;
	}
	else
	{
		if (m_cReply[3] != m_cMCNodeAddress)
			return MC_INVALID_ADDRESS;
	}
	
	if (m_cReply[8] == cBadCRC)
		return MC_BAD_CRC; // bad CRC in command

	if (m_cReply[7] != CalcCRC(m_cReply, 7))
		return MC_BAD_CRC; // bad CRC in the header of reply
	
	// Read data
	unsigned int DataLength = m_cReply[5]-1;
	if (!GetTerminal()->Read((char *)m_cReply+9, DataLength)) // -1 is because we read first byte already
		return MC_DEVICE_NOT_RESPONDING;
	m_MCReplyLength += DataLength;

	if (m_cReply[8] != m_CommandCode)
		return MC_GENERIC_ERROR; // bad command

	return ParseMCReply();
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::IsTurnedOn(BOOL &bOn)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	bOn = FALSE;
	unsigned char cData = 0;
	MC_ErrorCode EC = ReadCommand(0x05, cData);
	if (EC != MC_OK)
		return EC;
	bOn = cData;
	return MC_OK;
}

// On/Off
//virtual
MC_ErrorCode CPozharskyUpDownConverter::TurnOn(BOOL &bOn)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cOn = 0x00;
	if (bOn)
		cOn = 0x01;
	MC_ErrorCode EC = WriteCommand(0x05, cOn);
	if (EC != MC_OK)
		return EC;
	bOn = cOn;
	return MC_OK;
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::IsUpTurnedOn(BOOL &bOn)
{
	return IsTurnedOn(bOn);
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::TurnUpOn(BOOL &bOn)
{
	return TurnOn(bOn);
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::IsDownTurnedOn(BOOL &bOn)
{
	return IsTurnedOn(bOn);
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::TurnDownOn(BOOL &bOn)
{
	return TurnOn(bOn);
}

// Frequency

//virtual
MC_ErrorCode CPozharskyUpDownConverter::GetFrequency(unsigned int &Frequency)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	Frequency = 0;
	unsigned short freq = 0;
	MC_ErrorCode EC = ReadCommand(0x02, freq);
	if (EC != MC_OK)
		return EC;
	Frequency = freq;
	return MC_OK;
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::SetFrequency(unsigned int &Frequency)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned short sDataWord = (unsigned short)Frequency;
	MC_ErrorCode EC = WriteCommand(0x02, sDataWord);
	if (EC != MC_OK)
		return EC;
	return MC_OK;
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::GetUpOutputFrequency(unsigned int &Frequency)
{
	return GetFrequency(Frequency);
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::SetUpOutputFrequency(unsigned int &Frequency)
{
	return SetFrequency(Frequency);
}

// Frequency

//virtual
MC_ErrorCode CPozharskyUpDownConverter::GetDownInputFrequency(unsigned int &Frequency)
{
	return GetFrequency(Frequency);
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::SetDownInputFrequency(unsigned int &Frequency)
{
	return SetFrequency(Frequency);
}

// Temperature

//virtual
MC_ErrorCode CPozharskyUpDownConverter::GetTemperature(double &T)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cDataByte = 0x01; // any value is valid
	MC_ErrorCode EC = WriteCommand(0x10, cDataByte);
	if (EC != MC_OK)
		return EC;
	Sleep(500);
	EC = ReadCommand(0x10, cDataByte);
	if (EC != MC_OK)
		return EC;
	T = cDataByte;
	return MC_OK;
}

///virtual
MC_ErrorCode CPozharskyUpDownConverter::GetUpTemperature(double &T)
{
	return GetTemperature(T);
}

//virtual
MC_ErrorCode CPozharskyUpDownConverter::GetDownTemperature(double &T)
{
	return GetTemperature(T);
}


// CPozharskyUpDownConverterVer2002 implementation

CPozharskyUpDownConverterVer2002::CPozharskyUpDownConverterVer2002()
{
}

CPozharskyUpDownConverterVer2002::~CPozharskyUpDownConverterVer2002()
{
}

// Input attenuator

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2002::SetInputAtt(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cDataByte = (unsigned char)(AttValue);
	MC_ErrorCode EC = WriteCommand(0x03, cDataByte);
	if (EC != MC_OK)
		return EC;
	return MC_OK;
}

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2002::GetInputAtt(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	AttValue = 0;
	unsigned char att = 0x00;
	MC_ErrorCode EC = ReadCommand(0x03, att);
	if (EC != MC_OK)
		return EC;
	AttValue = att;
	return MC_OK;
}

// Output attenuator

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2002::SetOutputAtt(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cDataByte = (unsigned char)(AttValue);
	MC_ErrorCode EC = WriteCommand(0x07, cDataByte);
	if (EC != MC_OK)
		return EC;
	return MC_OK;
}

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2002::GetOutputAtt(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	AttValue = 0;
	unsigned char att = 0x00;
	MC_ErrorCode EC = ReadCommand(0x07, att);
	if (EC != MC_OK)
		return EC;
	AttValue = att;
	return MC_OK;
}

// Gain/loss

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2002::SetGainLoss(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cDataByte = (unsigned char)(AttValue);
	MC_ErrorCode EC = WriteCommand(0x04, cDataByte);
	if (EC != MC_OK)
		return EC;
	return MC_OK;
}

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2002::GetGainLoss(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	AttValue = 0;
	unsigned char att = 0x00;
	MC_ErrorCode EC = ReadCommand(0x04, att);
	if (EC != MC_OK)
		return EC;
	AttValue = att;
	return MC_OK;
}

MC_ErrorCode CPozharskyUpDownConverterVer2002::ParseMCReply()
{
	unsigned char *pData = m_cReply+9;
	if (pData[0] != m_Port)
		return MC_GENERIC_ERROR; // invalid port
	if (pData[1] == cInvalidCommand)
		return MC_INVALID_COMMAND;
	if (pData[1] == cInvalidPort)
		return MC_INVALID_ADDRESS;
	if (pData[1] == cCannotExecute)
		return MC_GENERIC_ERROR;
	if (pData[1] == cInvalidData)
		return MC_INVALID_PARAMETERS;
	if (pData[1] == MC_BAD_CRC)
		return MC_BAD_CRC;
	if (m_cReply[6] != CalcCRC(m_cReply+8, m_cReply[5]))
		return MC_BAD_CRC;
	if (pData[1] != cOK)
		return MC_GENERIC_ERROR;

	m_pReplyData = pData + 2;

	return MC_OK;
}

// CPozharskyUpConverterKvv140Ver2002 implementation

char *CPozharskyUpConverterKvv140Ver2002::m_pszName = "Up Converter KVV-140 (ver. 2002)";

CPozharskyUpConverterKvv140Ver2002::CPozharskyUpConverterKvv140Ver2002()
{
}

CPozharskyUpConverterKvv140Ver2002::~CPozharskyUpConverterKvv140Ver2002()
{
}

// Common device commands

// Up converter methods

// On/Off

// Input attenuator

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2002::SetUpInputAtt(double &AttValue)
{
	return SetInputAtt(AttValue);
}

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2002::GetUpInputAtt(double &AttValue)
{
	return GetInputAtt(AttValue);
}

// Output attenuator

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2002::SetUpOutputAtt(double &AttValue)
{
	return SetOutputAtt(AttValue);
}

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2002::GetUpOutputAtt(double &AttValue)
{
	return GetOutputAtt(AttValue);
}

// Gain/loss

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2002::SetUpGainLoss(double &AttValue)
{
	return SetGainLoss(AttValue);
}

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2002::GetUpGainLoss(double &AttValue)
{
	return GetGainLoss(AttValue);
}

// Temperature

// CPozharskyUpDownConverterVer2004 implementation

CPozharskyUpDownConverterVer2004::CPozharskyUpDownConverterVer2004()
{
}

CPozharskyUpDownConverterVer2004::~CPozharskyUpDownConverterVer2004()
{
}

// Input attenuator

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2004::SetInputAtt(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	double AttCode = -100*AttValue + 4096;
	unsigned short cDataWord = (unsigned short)(AttCode);
	MC_ErrorCode EC = WriteCommand(0x14, cDataWord);
	if (EC != MC_OK)
		return EC;
	return MC_OK;
}

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2004::GetInputAtt(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	AttValue = -1;
	unsigned short AttCode = 0x0000;
	MC_ErrorCode EC = ReadCommand(0x14, AttCode);
	if (EC != MC_OK)
		return EC;
	AttValue  = (4096 - AttCode)/100;
	return MC_OK;
}

MC_ErrorCode CPozharskyUpDownConverterVer2004::ParseMCReply()
{
	unsigned char *pData = m_cReply+9;
	if (m_Port == 0x14)
	{ // attenuator command, differ from other ones
		// I do not know why but converter replies 17, 3, <two-byte code> here. No checks for now
		if (m_cReply[6] != CalcCRC(m_cReply+8, m_cReply[5]))
			return MC_BAD_CRC;

		m_pReplyData = pData + 2;
	}
	else
	{ // other commands (temperature, frequency, etc)
		if (pData[0] != m_Port)
			return MC_GENERIC_ERROR; // invalid port
		if (pData[1] == cInvalidCommand)
			return MC_INVALID_COMMAND;
		if (pData[1] == cInvalidPort)
			return MC_INVALID_ADDRESS;
		if (pData[1] == cCannotExecute)
			return MC_GENERIC_ERROR;
		if (pData[1] == cInvalidData)
			return MC_INVALID_PARAMETERS;
		if (pData[1] == MC_BAD_CRC)
			return MC_BAD_CRC;
		if (m_cReply[6] != CalcCRC(m_cReply+8, m_cReply[5]))
			return MC_BAD_CRC;
		if (pData[1] != cOK)
			return MC_GENERIC_ERROR;

		m_pReplyData = pData + 2;
	}

	return MC_OK;
}

// CPozharskyUpConverterKvv140Ver2004 implementation

char *CPozharskyUpConverterKvv140Ver2004::m_pszName = "Up Converter KVV-140 (ver. 2004)";

CPozharskyUpConverterKvv140Ver2004::CPozharskyUpConverterKvv140Ver2004()
{
}

CPozharskyUpConverterKvv140Ver2004::~CPozharskyUpConverterKvv140Ver2004()
{
}

// Common device commands

// Up converter methods

// Output

// Frequency

// Input attenuator

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2004::SetUpInputAtt(double &AttValue)
{
	return SetInputAtt(AttValue);
}

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2004::GetUpInputAtt(double &AttValue)
{
	return GetInputAtt(AttValue);
}

// Temperature


// CPozharskyUpConverterKvv140Ver2011 implementation

char *CPozharskyUpConverterKvv140Ver2011::m_pszName = "Up Converter KVV-140 (ver. 2011)";

CPozharskyUpConverterKvv140Ver2011::CPozharskyUpConverterKvv140Ver2011()
{
}

CPozharskyUpConverterKvv140Ver2011::~CPozharskyUpConverterKvv140Ver2011()
{
}

// Common device commands

// Up converter methods

// Output

// Frequency

// Input attenuator

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2011::SetUpInputAtt(double &AttValue)
{
	return SetInputAtt(AttValue);
}

//virtual
MC_ErrorCode CPozharskyUpConverterKvv140Ver2011::GetUpInputAtt(double &AttValue)
{
	return GetInputAtt(AttValue);
}

// Temperature


// CPozharskyDownConverterKvn140Ver2002 implementation

char *CPozharskyDownConverterKvn140Ver2002::m_pszName = "Down Converter KVN-140 (ver. 2002)";

CPozharskyDownConverterKvn140Ver2002::CPozharskyDownConverterKvn140Ver2002()
{
}

CPozharskyDownConverterKvn140Ver2002::~CPozharskyDownConverterKvn140Ver2002()
{
}

// Common device commands

// Down converter methods

// Input attenuator

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2002::SetDownInputAtt(double &AttValue)
{
	return SetInputAtt(AttValue);
}

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2002::GetDownInputAtt(double &AttValue)
{
	return GetInputAtt(AttValue);
}

// Output attenuator

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2002::SetDownOutputAtt(double &AttValue)
{
	return SetOutputAtt(AttValue);
}

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2002::GetDownOutputAtt(double &AttValue)
{
	return GetOutputAtt(AttValue);
}

// Gain/loss

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2002::GetDownGainLoss(double &AttValue)
{
	return GetGainLoss(AttValue);
}

// Spectrum inversion

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2002::EnableDownSpectralInv(BOOL &bEnabled)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cOn = 0x00;
	if (bEnabled)
		cOn = 0x01;
	MC_ErrorCode EC = WriteCommand(0x11, cOn);
	if (EC != MC_OK)
		return EC;
	bEnabled = cOn;
	return MC_OK;
}

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2002::IsDownSpectralInvEnabled(BOOL &bEnabled)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	bEnabled = FALSE;
	unsigned char cData = 0;
	MC_ErrorCode EC = ReadCommand(0x11, cData);
	if (EC != MC_OK)
		return EC;
	bEnabled = cData;
	return MC_OK;
}

// Temperature

// CPozharskyDownConverterKvn140Ver2004 implementation

char *CPozharskyDownConverterKvn140Ver2004::m_pszName = "Down Converter KVN-140 (ver. 2004)";

CPozharskyDownConverterKvn140Ver2004::CPozharskyDownConverterKvn140Ver2004()
{
}

CPozharskyDownConverterKvn140Ver2004::~CPozharskyDownConverterKvn140Ver2004()
{
}

// Common device commands

// Down converter methods

// Frequency

// Input attenuator

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2004::SetDownInputAtt(double &AttValue)
{
	return SetInputAtt(AttValue);
}

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2004::GetDownInputAtt(double &AttValue)
{
	return GetInputAtt(AttValue);
}

// Spectrum inversion

//virtual
/*MC_ErrorCode CPozharskyDownConverterKvn140Ver2004::EnableDownSpectralInv(BOOL &bEnabled)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cOn = 0x00;
	if (bEnabled)
		cOn = 0x01;
	MC_ErrorCode EC = WriteCommand(0x11, cOn);
	if (EC != MC_OK)
		return EC;
	bEnabled = cOn;
	return MC_OK;
}*/

//virtual
/*MC_ErrorCode CPozharskyDownConverterKvn140Ver2004::IsDownSpectralInvEnabled(BOOL &bEnabled)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	bEnabled = FALSE;
	unsigned char cData = 0;
	MC_ErrorCode EC = ReadCommand(0x11, cData);
	if (EC != MC_OK)
		return EC;
	bEnabled = cData;
	return MC_OK;
}*/

// Temperature

// CPozharskyUpDownConverterVer2011 implementation

CPozharskyUpDownConverterVer2011::CPozharskyUpDownConverterVer2011()
{
}

CPozharskyUpDownConverterVer2011::~CPozharskyUpDownConverterVer2011()
{
}

// Input attenuator

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2011::SetInputAtt(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cDataWord = (unsigned char)(AttValue*2);
	MC_ErrorCode EC = WriteCommand(0x15, cDataWord);
	if (EC != MC_OK)
		return EC;
	return MC_OK;
}

//virtual
MC_ErrorCode CPozharskyUpDownConverterVer2011::GetInputAtt(double &AttValue)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	AttValue = -1;
	unsigned char AttCode = 0x00;
	MC_ErrorCode EC = ReadCommand(0x15, AttCode);
	if (EC != MC_OK)
		return EC;
	AttValue  = AttCode/2.;
	return MC_OK;
}

MC_ErrorCode CPozharskyUpDownConverterVer2011::ParseMCReply()
{
	unsigned char *pData = m_cReply+9;
	if (m_Port == 0x14)
	{ // attenuator command, differ from other ones
		// I do not know why but converter replies 17, 3, <two-byte code> here. No checks for now
		if (m_cReply[6] != CalcCRC(m_cReply+8, m_cReply[5]))
			return MC_BAD_CRC;

		m_pReplyData = pData + 2;
	}
	else
	{ // other commands (temperature, frequency, etc)
		if (pData[0] != m_Port)
			return MC_GENERIC_ERROR; // invalid port
		if (pData[1] == cInvalidCommand)
			return MC_INVALID_COMMAND;
		if (pData[1] == cInvalidPort)
			return MC_INVALID_ADDRESS;
		if (pData[1] == cCannotExecute)
			return MC_GENERIC_ERROR;
		if (pData[1] == cInvalidData)
			return MC_INVALID_PARAMETERS;
		if (pData[1] == MC_BAD_CRC)
			return MC_BAD_CRC;
		if (m_cReply[6] != CalcCRC(m_cReply+8, m_cReply[5]))
			return MC_BAD_CRC;
		if (pData[1] != cOK)
			return MC_GENERIC_ERROR;

		m_pReplyData = pData + 2;
	}

	return MC_OK;
}

// CPozharskyDownConverterKvn140Ver2011 implementation

char *CPozharskyDownConverterKvn140Ver2011::m_pszName = "Down Converter KVN-140 (ver. 2011)";

CPozharskyDownConverterKvn140Ver2011::CPozharskyDownConverterKvn140Ver2011()
{
}

CPozharskyDownConverterKvn140Ver2011::~CPozharskyDownConverterKvn140Ver2011()
{
}

// Common device commands

// Down converter methods

// Frequency

// Input attenuator

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2011::SetDownInputAtt(double &AttValue)
{
	return SetInputAtt(AttValue);
}

//virtual
MC_ErrorCode CPozharskyDownConverterKvn140Ver2011::GetDownInputAtt(double &AttValue)
{
	return GetInputAtt(AttValue);
}

// Spectrum inversion

//virtual
/*MC_ErrorCode CPozharskyDownConverterKvn140Ver2004::EnableDownSpectralInv(BOOL &bEnabled)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	unsigned char cOn = 0x00;
	if (bEnabled)
		cOn = 0x01;
	MC_ErrorCode EC = WriteCommand(0x11, cOn);
	if (EC != MC_OK)
		return EC;
	bEnabled = cOn;
	return MC_OK;
}*/

//virtual
/*MC_ErrorCode CPozharskyDownConverterKvn140Ver2004::IsDownSpectralInvEnabled(BOOL &bEnabled)
{
	if (!IsControllable()) return MC_DEVICE_NOT_RESPONDING;
	bEnabled = FALSE;
	unsigned char cData = 0;
	MC_ErrorCode EC = ReadCommand(0x11, cData);
	if (EC != MC_OK)
		return EC;
	bEnabled = cData;
	return MC_OK;
}*/

// Temperature

