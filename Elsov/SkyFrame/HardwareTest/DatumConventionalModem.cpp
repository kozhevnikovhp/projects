#include "stdafx.h"
#include "DatumConventionalModem.h"

//virtual
MC_ErrorCode CDatumConventionalModem::ReadReplyUntilPrompt()
{
	memset(m_pDataBytes, 0, sizeof(m_pDataBytes));
	memset(m_pRawReply, 0, sizeof(m_pRawReply));
	unsigned char byte;
	BOOL bPad = TRUE;
	BOOL bOpened = FALSE;
	BOOL bMyReply = FALSE;
	BOOL bMyModem = FALSE;
	BOOL bCommandCode = FALSE;
	BOOL bStatusByte = FALSE;
	BOOL bErrorByte = FALSE;
	BOOL bDataBytes = FALSE;
	BOOL bClosed = FALSE;
	m_ReturnedDataLength = 0;
	m_RawReplyLength = 0;

	while (1)
	{
		byte = 0x00;
		if (!GetTerminal()->Read((char *)&byte, 1))
			return MC_DEVICE_NOT_RESPONDING;
		m_pRawReply[m_RawReplyLength++] = byte;

		if (bPad && byte == PAD)
			continue; // bypass leading pad bytes
		bPad = FALSE;

		if (!bOpened && byte == REPLY_OPEN_FLAG)
		{
			bOpened = TRUE;
			continue;
		}
		if (!bOpened)
			break; // error in reply! TO DO: handle it!

		if (!bMyReply && byte == m_ControllerAddress)
		{
			bMyReply = TRUE;
			continue;
		}
		if (!bMyReply)
			break;

		if (!bMyModem && byte == m_ModemAddress)
		{
			bMyModem = TRUE;
			continue;
		}
		if (!bMyModem)
			break;

		if (!bCommandCode)
		{
			bCommandCode = TRUE;
			continue;
		}
		if (!bCommandCode)
			break;

		if (!bStatusByte)
		{
			bStatusByte = TRUE;
			m_cStatusByte = byte;
			continue;
		}
		if (!bStatusByte)
			break;

		if (!bErrorByte)
		{
			bErrorByte = TRUE;
			m_cErrorByte = byte;
			continue;
		}
		if (!bErrorByte)
			break;

		if (!bDataBytes)
		{
			bDataBytes = TRUE;
			m_cDataBytes = byte;
			continue;
		}
		if (!bDataBytes)
			break;

		if (m_ReturnedDataLength < m_cDataBytes)
		{
			m_pDataBytes[m_ReturnedDataLength++] = byte;
			continue;
		}

		if (!bClosed && byte == CLOSE_FLAG)
		{
			bClosed = TRUE;
			continue;
		}

		// here checksum is (byte variable)
		if (bClosed)
			break;

	}

	// Analize error byte
	MC_ErrorCode EC = MC_DEVICE_NOT_RESPONDING;
	if (m_ReturnedDataLength > 0)
	{
		EC = MC_OK;
		switch (m_cErrorByte>>4)
		{
		case 0x00: EC = MC_OK; break;				// all right
		case 0x01: EC = MC_INVALID_COMMAND; break;	// read-only value
		case 0x02: EC = MC_INVALID_COMMAND; break;	// invalid request
		case 0x03: EC = MC_COMMAND_NOT_SUPPORTED; break; // option n/a
		case 0x04: EC = MC_INVALID_COMMAND; break;	// read-only value
		default:   EC = MC_GENERIC_ERROR; break;	// all other cases
		}
	}

	return MC_OK;
}

int CDatumConventionalModem::FillCommandBuffer(unsigned char CommandCode, unsigned char Mode, unsigned char *pcDataBytes, unsigned char nDataBytes)
{
	int i=0;
	m_pszCommand[i++] = PAD; // opening pad byte
	m_pszCommand[i++] = COMMAND_OPEN_FLAG; // opening flag
	m_pszCommand[i++] = m_ModemAddress; // 
	m_pszCommand[i++] = m_ControllerAddress; // 
	m_pszCommand[i++] = CommandCode;

	unsigned char ModeByte = GetModeField(Mode);
	m_pszCommand[i++] = ModeByte;

	m_pszCommand[i++] = nDataBytes;
	for (int j=0; j<nDataBytes; j++)
	{
		m_pszCommand[i++] = pcDataBytes[j]; // data bytes
	}
	m_pszCommand[i++] = (char)0x96; // closing flag
	m_pszCommand[i++] = CheckSum(1, i-1);
	m_pszCommand[i++] = PAD; // closing pad byte
	return i;
}

unsigned char CDatumConventionalModem::CheckSum(int Start, int Stop)
{
	return CheckSum(m_pszCommand, Start, Stop);
}

unsigned char CDatumConventionalModem::CheckSum(unsigned char *pcData, int Start, int Stop)
{
	unsigned int sum = 0;
	for (int i = Start; i <= Stop; i++)
		sum += pcData[i];
	sum = 256 - sum;
	sum = sum % 256;
	unsigned char ret = (unsigned char)sum;
	return ret;
}


