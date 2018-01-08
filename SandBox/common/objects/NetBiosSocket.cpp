/* File NetBiosSocket.cpp - netbios protocols implementation

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	03 Mar 2008 - initial creation
	25 Jul 2008 - Raw socket set as base class (to make it possible to send requests from port 137)
*/

#include "stdafx.h"
#include "NetBiosSocket.h"

int RFC883_UnpackName(unsigned char *pCompressedName, char *pszBufferForResult, int nBufferSize)
{
	const char ESC = 27;

	unsigned char label_len = *pCompressedName;
	int nOctetsProcessed = 1;
	
	unsigned char *pcCurrentOctet = pCompressedName+1;
	int i = 0;
	while (label_len != 0 && i < (nBufferSize-1))
	{
		for (int j = 0; j < label_len/2 && i < (nBufferSize-1); j++)
		{
			unsigned char hi = *pcCurrentOctet++ - 'A';
			unsigned char lo = *pcCurrentOctet++ - 'A';
			char c = hi << 4 | lo;
			pszBufferForResult[i++] = c;
			nOctetsProcessed += 2; // 2 octets processed to get 1 symbol
		}
		label_len = *pcCurrentOctet;
	}
	pszBufferForResult[i] = 0; // terminating zero

	// Cut tailing blanks and escape symbols
	for (i = i-1; i > 0; i--)
	{
		if (pszBufferForResult[i] == ' ')
			pszBufferForResult[i] = 0;
		if (pszBufferForResult[i] == ESC)
			pszBufferForResult[i] = 0;
	}

	return nOctetsProcessed+1;
}

/////////////////////////////////////////////////////////////////////
// Class CNetBiosSocket implementation


CNetBiosSocket::CNetBiosSocket()
{
	LOGICAL bSuccess = Create(AF_INET, SOCK_RAW, IPPROTO_IP);
	SetIpHeaderIncl(LOGICAL_TRUE);
}

//virtual
CNetBiosSocket::~CNetBiosSocket()
{
}

LOGICAL CNetBiosSocket::SendStatRequest(IPADDRESS_TYPE uSrcIP, IPADDRESS_TYPE uDstIP)
{
	unsigned char _body[38] = { 0x20, 0x43, 0x4B, 0x41, 0x41, 0x41, 0x41, 0x41, 
		0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
		0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
		0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x00, 0x21, 0x00, 0x01 };

	memset(&m_NetBiosStatRequest, 0, sizeof(m_NetBiosStatRequest));

	// Fill IP-header
	int nIpHeaderLength = FillIpHeader(&m_NetBiosStatRequest.IpHeader, sizeof(m_NetBiosStatRequest), uSrcIP, uDstIP);

	// Fill in UDP-header manually (to be able to send datagram from port 137)
	m_NetBiosStatRequest.UdpHeader.SrcPortNo =  htons(PORT_137);
	m_NetBiosStatRequest.UdpHeader.DstPortNo =  htons(PORT_137);
	unsigned short DgmSize = sizeof(SNameServiceDgm)-sizeof(SIpHeader);
	m_NetBiosStatRequest.UdpHeader.Length = htons(DgmSize);

	m_NetBiosStatRequest.NSHeader.TransactionID = 0xABCD;
	m_NetBiosStatRequest.NSHeader.QDcount = htons(1);

	memcpy(m_NetBiosStatRequest.body, _body, sizeof(_body));

	// CRC
	m_NetBiosStatRequest.UdpHeader.CRC = CalcPseudoCheckSum(&m_NetBiosStatRequest.IpHeader, (unsigned short *)&m_NetBiosStatRequest.UdpHeader, DgmSize);

	unsigned int nWrittenBytes = 0;
	return WriteTo(&m_NetBiosStatRequest, sizeof(m_NetBiosStatRequest), nWrittenBytes, uDstIP, PORT_137);
}

