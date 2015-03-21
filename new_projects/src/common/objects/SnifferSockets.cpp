// Class CSnifferSocket implementation

#include "stdafx.h"
#include "SnifferSockets.h"

#ifndef SIO_RCVALL
#define SIO_RCVALL 0x98000001
#endif


CSnifferSocket::CSnifferSocket()
{
	Open();
}

//virtual
CSnifferSocket::~CSnifferSocket()
{
}

LOGICAL CSnifferSocket::Open()
{
	return Create(AF_INET, SOCK_RAW, IPPROTO_IP);
}

LOGICAL CSnifferSocket::Bind(unsigned long InterfaceIpAddress)
{
	return CIpSocket::Bind(0, InterfaceIpAddress);
}

LOGICAL CSnifferSocket::EnablePromiscMode()
{
	unsigned long	flag = 1;  // ‘лаг PROMISC ¬кл/выкл.
	ioctlsocket(m_Socket, SIO_RCVALL, &flag);
	return LOGICAL_TRUE;
}

LOGICAL CSnifferSocket::DisablePromiscMode()
{
	unsigned long	flag = 0;  // ‘лаг PROMISC ¬кл/выкл.
	ioctlsocket(m_Socket, SIO_RCVALL, &flag);
	return LOGICAL_TRUE;
}

LOGICAL CSnifferSocket::WaitForPacket()
{
	struct sockaddr_in Src;
	unsigned int nReadBytes = 0;
	LOGICAL bSuccess = ReadFrom(m_szBufferForPackets, sizeof(m_szBufferForPackets), nReadBytes, &Src);
	if (!bSuccess)
		return LOGICAL_FALSE;
	
	SIpHeader *pIpHeader = (SIpHeader *)m_szBufferForPackets;
	unsigned short	nIpHdrLen = pIpHeader->h_len * 4;
	unsigned char *pUserData = m_szBufferForPackets + nIpHdrLen;
	unsigned int nUserDataLength = nReadBytes - nIpHdrLen;
	LOGICAL bProcessed = OnIpPacket(pIpHeader, pUserData, nUserDataLength);
	if (bProcessed)
		return LOGICAL_TRUE; // no more processing required

	SIcmpHeader *pIcmpHeader;
	SIgmpHeader *pIgmpHeader;
	STcpHeader *pTcpHeader;
	SUdpHeader *pUdpHeader;
	
	switch (pIpHeader->proto)
	{
	case IPPROTO_TCP:
		pTcpHeader = (STcpHeader *)pUserData;
		pUserData += sizeof(STcpHeader);
		nUserDataLength -= sizeof(STcpHeader);
		OnTcpPacket(pIpHeader, pTcpHeader, pUserData, nUserDataLength);
		break;
	case IPPROTO_UDP:
		pUdpHeader = (SUdpHeader *)pUserData;
		pUserData += sizeof(SUdpHeader);
		nUserDataLength -= sizeof(SUdpHeader);
		OnUdpPacket(pIpHeader, pUdpHeader, pUserData, nUserDataLength);
		break;
	case IPPROTO_ICMP:
		pIcmpHeader = (SIcmpHeader *)pUserData;
		pUserData += sizeof(SIcmpHeader);
		nUserDataLength -= sizeof(SIcmpHeader);
		OnIcmpPacket(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case IPPROTO_IGMP:
		pIgmpHeader = (SIgmpHeader *)pUserData;
		pUserData += sizeof(SIgmpHeader);
		nUserDataLength -= sizeof(SIgmpHeader);
		OnIgmpPacket(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
		break;
	default:
		OnUnknownProtoPacket(pIpHeader, pUserData, nUserDataLength);
		break;
	} // end of switch (pIpHeader->proto)

	return LOGICAL_TRUE; // successfully read and processed
}


/////////////////////////////////////////////////////////////////////
// CSnifferThread class implementation

//Constuctors/destructors
CSnifferThread::CSnifferThread()
{
}

//virtual
CSnifferThread::~CSnifferThread()
{
}

//virtual
LOGICAL CSnifferThread::ThreadBody()
{
	return WaitForPacket();
}

//virtual
void CSnifferThread::OnThreadStopping()
{
	Close();
}


