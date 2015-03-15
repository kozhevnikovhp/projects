/* SpySocketThread.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	15 Mar 2006 - initial creation

*/


#include "stdafx.h"
#include "SpySocketThread.h"
#include "Network.h"
#include "Adapter.h"

/////////////////////////////////////////////////////////////////////
// CSpySocketThread class implementation

//Constuctors/destructors
CSpySocketThread::CSpySocketThread(CAdapter *pAdapter)
{
	m_pAdapter = pAdapter;
}

//virtual
CSpySocketThread::~CSpySocketThread()
{
}

//virtual
void CSpySocketThread::OnThreadStarted()
{
	if (!IsOpened())
		Open();
	Bind(m_pAdapter->GetIpAddress());
	EnablePromiscMode();
}


//virtual
void CSpySocketThread::OnThreadEnded()
{
}

//virtual
LOGICAL CSpySocketThread::OnIpPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	return m_pAdapter->IpPacketCaptured(pIpHeader);
}

//virtual
void CSpySocketThread::OnIcmpPacket(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
/*	TRACE("%d ICMP\tlen = %5d (from %s\t to %s)\n", m_nPackets, ntohs(pIpHeader->total_len),
				AddressToString(szFrom, pIpHeader->sourceIP),
				AddressToString(szTo, pIpHeader->destIP));*/
	m_pAdapter->IcmpPacketCaptured(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
}

//virtual
void CSpySocketThread::OnIgmpPacket(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	m_pAdapter->IgmpPacketCaptured(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
}

//virtual
void CSpySocketThread::OnTcpPacket(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
/*	unsigned short SrcPortNo = ntohs(pTcpHeader->SrcPortNo);
	unsigned short DstPortNo = ntohs(pTcpHeader->DstPortNo);
	TRACE("%d TCP:%5d/%5d len = %5d (from %s\t to %s)\n", m_nPackets, SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
		AddressToString(szFrom, pIpHeader->sourceIP),
		AddressToString(szTo, pIpHeader->destIP));*/
	m_pAdapter->TcpPacketCaptured(pIpHeader, pTcpHeader, pUserData, nUserDataLength);
}

//virtual
void CSpySocketThread::OnUdpPacket(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	m_pAdapter->UdpPacketCaptured(pIpHeader, pUdpHeader, pUserData, nUserDataLength);
}

//virtual
void CSpySocketThread::OnUnknownProtoPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	char szFrom[16], szTo[16];
	TRACE("UNKNOWN: PROTO=%d, %s -> %s\n", pIpHeader->proto,
		AddressToString(szFrom, pIpHeader->sourceIP),
		AddressToString(szTo, pIpHeader->destIP));
}
