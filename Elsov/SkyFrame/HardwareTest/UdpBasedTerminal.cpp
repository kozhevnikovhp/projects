#include "stdafx.h"
#include "UdpBasedTerminal.h"

CUdpBasedTerminal::CUdpBasedTerminal()
{
	m_IpAddress = 0;
	m_CurrentReadPos = 0;
	m_LastDatagramSize = 0;
}

//virtual
CUdpBasedTerminal::~CUdpBasedTerminal()
{
}

//virtual
LOGICAL CUdpBasedTerminal::Connect(const char *pszConnectionName)
{
	m_IpAddress = StringToAddress(pszConnectionName);
	SetConnectionName(pszConnectionName);
	return TRUE;
}

//virtual
LOGICAL CUdpBasedTerminal::Write(void *pszBuffer, unsigned int BytesToWrite, unsigned int &nWrittenBytes)
{
	return WriteTo(pszBuffer, BytesToWrite, nWrittenBytes, m_IpAddress, GetDefaultPort());
}

//virtual
LOGICAL CUdpBasedTerminal::Read(void *pszBuffer, unsigned int nBufferSize, unsigned int &nReadBytes)
{
	if (nBufferSize <= (m_LastDatagramSize-m_CurrentReadPos))
	{
		memcpy(pszBuffer, m_pDatagramBuffer+m_CurrentReadPos, nBufferSize);
		m_CurrentReadPos += nBufferSize;
		nReadBytes = nBufferSize;
		return TRUE;
	}
	else
	{
		nReadBytes = 0;
		if (m_CurrentReadPos < m_LastDatagramSize)
		{
			unsigned int nBytesToCopy = m_LastDatagramSize - m_CurrentReadPos;
			memcpy(pszBuffer, m_pDatagramBuffer+m_CurrentReadPos, nBytesToCopy);
			nReadBytes += nBytesToCopy;
		}
		m_CurrentReadPos = 0;
		unsigned long IpFrom;
		unsigned short PortFrom;
		if (!ReadFrom(m_pDatagramBuffer, GetMaxDatagramSize(), m_LastDatagramSize, IpFrom, PortFrom))
		{
			m_LastDatagramSize = 0;
			return FALSE;
		}
		unsigned int BytesThisReading = 0;
		if (!Read(pszBuffer, nBufferSize-nReadBytes, BytesThisReading))
			return FALSE;
		nReadBytes += BytesThisReading;
		return TRUE;
	}
}

//virtual
LOGICAL CUdpBasedTerminal::PeekInputSymbols(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes)
{
	return Read(pBufferToFill, nBufferSize, nReadBytes);
}

//virtual
LOGICAL CUdpBasedTerminal::SetReadTimeout(DWORD a_dwTimeout)
{
	return CIpSocket::SetReadTimeout(a_dwTimeout);
}

//virtual
LOGICAL CUdpBasedTerminal::SetWriteTimeout(DWORD a_dwTimeout)
{
	return CIpSocket::SetWriteTimeout(a_dwTimeout);
}

