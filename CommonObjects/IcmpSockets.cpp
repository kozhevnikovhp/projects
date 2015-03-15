/* File IcmpSockets.cpp
Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org
 
Revision history:
	11 Dec 2001 - initial creation
*/

#include "stdafx.h"
#include "IcmpSockets.h"
#include "stdio.h" //debug

/////////////////////////////////////////////////////////////////////////////
// CIcmpSocketBaseFunc class implementation

CIcmpSocketBaseFunc::CIcmpSocketBaseFunc()
{
	m_bTimedOut = LOGICAL_FALSE;
	m_bBad = LOGICAL_FALSE;
	m_dwDelayTime = -1;
	m_SeqNumber = 0;
	m_dwLastEchoReqTime = GetTickCount() - 200;
	m_dwReceivedDataLength = 0;
	m_cReplyType = 0;
}

//virtual
CIcmpSocketBaseFunc::~CIcmpSocketBaseFunc()
{
}

LOGICAL CIcmpSocketBaseFunc::SendEchoRequest
(
	IPADDRESS_TYPE IpAddress,
	int nPacketSize /* = 32 */
)
{
	if (nPacketSize > GetMaxPacketSize())
		return LOGICAL_FALSE;
	memset(m_ucBuffer, 0, sizeof(m_ucBuffer));
	int nUserDataSize = nPacketSize;

	// Fill IP-header
	SIpHeader *pIpHeader = (SIpHeader *)(m_ucBuffer);
	int nIpHeaderLength = 0;
	if (HasOwnIpHeader())
	{
		nIpHeaderLength = FillIpHeader(pIpHeader, nPacketSize, m_uSrcIP, IpAddress);
		nUserDataSize -= nIpHeaderLength;
	}
	else
	{
		nIpHeaderLength = 0; // do not count IP-header octets
		nPacketSize -= sizeof(SIpHeader);
		nUserDataSize -= sizeof(SIpHeader);
	}

	// Fill ICMP-header
	SIcmpHeader	*pIcmpHeader = (SIcmpHeader *)(m_ucBuffer+nIpHeaderLength);
	pIcmpHeader->i_type  = ICMP_TYPE_ECHO_REQUEST;
	pIcmpHeader->i_code  = 0;
	pIcmpHeader->i_id    = CalcPacketID();
	pIcmpHeader->i_chksum= 0;
	pIcmpHeader->i_seq   = m_SeqNumber = CalcPacketSequenceNumber();
	pIcmpHeader->timestamp = CalcTimeStamp();
	nUserDataSize -= sizeof(SIcmpHeader);
	
	// Fill user data
	if (nUserDataSize < 0)
		nUserDataSize = 0;
	char *pUserData = (char *)(pIcmpHeader+1); // user data follows ICMP-header
	FillDataBuffer(pUserData, nUserDataSize);

	// Calc checksum
	unsigned int nIcmpAndUserSize = sizeof(SIcmpHeader) + nUserDataSize;
	pIcmpHeader->i_chksum = CalcCheckSum((unsigned short *)pIcmpHeader, nIcmpAndUserSize);

	// Send packet
	m_dwLastEchoReqTime = GetTickCount();
	unsigned int nSentBytes;
	LOGICAL bWritten = WriteTo(m_ucBuffer, nPacketSize, nSentBytes, IpAddress, 0);
	return bWritten;
}

LOGICAL CIcmpSocketBaseFunc::SendEchoRequest(const char *pszIpAddress, int nPacketSize /* = 32 */)
{
	IPADDRESS_TYPE IpAddress = inet_addr(pszIpAddress);
	return SendEchoRequest(IpAddress, nPacketSize);
}

LOGICAL CIcmpSocketBaseFunc::SendBroadcastEchoRequest(int nPacketSize /* = 32 */)
{
	return SendEchoRequest(INADDR_BROADCAST, nPacketSize);
}

LOGICAL CIcmpSocketBaseFunc::SendRouterSolicitation(IPADDRESS_TYPE IpAddress)
{
	// Fill IP-header
	SIpHeader *pIpHeader = (SIpHeader *)(m_ucBuffer);
	int nIpHeaderLength = 0;
	unsigned short nPacketSize = 28;
		if (HasOwnIpHeader())
	{
		nIpHeaderLength = FillIpHeader(pIpHeader, nPacketSize, m_uSrcIP, IpAddress);
	}
	else
	{
		nIpHeaderLength = 0; // do not count IP-header octets
		nPacketSize -= sizeof(SIpHeader);
	}

	// Fill ICMP-header
	SIcmpHeader	*pIcmpHeader = (SIcmpHeader *)(m_ucBuffer+nIpHeaderLength);
	pIcmpHeader->i_type  = ICMP_TYPE_ROUTER_SOLICITATION;
	pIcmpHeader->i_code  = 0;
	pIcmpHeader->i_id    = 0;
	pIcmpHeader->i_chksum= 0;
	pIcmpHeader->i_seq   = 0;
	pIcmpHeader->timestamp = 0;
	
	// Calc checksum
	pIcmpHeader->i_chksum = CalcCheckSum((unsigned short *)pIcmpHeader, nPacketSize);

	// Send packet
	m_dwLastEchoReqTime = GetTickCount();
	unsigned int nSentBytes;
	LOGICAL bWritten = WriteTo(m_ucBuffer, nPacketSize, nSentBytes, IpAddress, 0);
	return bWritten;
}

LOGICAL CIcmpSocketBaseFunc::ReceiveEchoReply()
{
	m_bTimedOut = LOGICAL_TRUE;
	m_cReplyType = -1;
	unsigned int nReadBytes;
	sockaddr_in From;
	ASSERT(sizeof(From) == sizeof(m_ReplySource));
	while (GetTickCount() - m_dwLastEchoReqTime < m_dwTimeout)
	{
		m_bBad = LOGICAL_FALSE;
		m_dwDelayTime = -1;

		if (!ReadFrom(m_ucBuffer, sizeof(m_ucBuffer), nReadBytes, &From))
		{
			if (GetLastErrorCode() == WSAETIMEDOUT)
				return LOGICAL_TRUE;
			return LOGICAL_FALSE;
		}
		
		SIpHeader *pIpHdr = (SIpHeader *)m_ucBuffer;
		unsigned short	IpHdrLen = pIpHdr->h_len * 4;

		if (nReadBytes < (unsigned long)(IpHdrLen + ICMP_MIN))
		{
			m_bBad = LOGICAL_TRUE;
			continue;
		}

		unsigned char *pData = m_ucBuffer + IpHdrLen + sizeof(SIcmpHeader);
		m_dwReceivedDataLength = nReadBytes;

		SIcmpHeader *pIcmpHdr = (SIcmpHeader *)(m_ucBuffer + IpHdrLen);
		m_bBad = FALSE;
		/*TRACE("CIcmpSocketBaseFunc: %d bytes from %d.%d.%d.%d\n", nReadBytes,
			From.sin_addr.S_un.S_un_b.s_b1,
			From.sin_addr.S_un.S_un_b.s_b2,
			From.sin_addr.S_un.S_un_b.s_b3,
			From.sin_addr.S_un.S_un_b.s_b4);*/
		
		switch (pIcmpHdr->i_type)
		{
		case ICMP_TYPE_ECHO_REPLY:
			if (!IsOurPacketID(pIcmpHdr))
				continue;
			if (!IsOurPacketSequenceNumber(pIcmpHdr))
				continue;
			if (!IsOurPacketTimeStamp(pIcmpHdr))
				continue;

			memcpy(&m_ReplySource, &From, sizeof(From));
			m_cReplyType = pIcmpHdr->i_type;
			m_bTimedOut = FALSE;
			m_dwDelayTime = GetTickCount() - pIcmpHdr->timestamp;
			OnEchoReceived(pIpHdr, pIcmpHdr, m_ReplySource.sin_addr.S_un.S_addr, pData, m_dwReceivedDataLength);
			return LOGICAL_TRUE;
		case ICMP_TYPE_DEST_UNREACHABLE:
			{
			unsigned char *pUserData = (unsigned char *)pIcmpHdr + 8 + sizeof(SIpHeader) + 4;
			unsigned short *pProcessID = (unsigned short *)pUserData;
			if (*pProcessID != CalcPacketID())
				continue;
			unsigned short *pSeqNum = pProcessID + 1;
			if (*pSeqNum != CalcPacketSequenceNumber())
				continue;

			memcpy(&m_ReplySource, &From, sizeof(From));
			m_cReplyType = pIcmpHdr->i_type;
			m_bTimedOut = LOGICAL_FALSE;
			OnDestUnreachable(pIpHdr, pIcmpHdr, m_ReplySource.sin_addr.S_un.S_addr, pData, m_dwReceivedDataLength);
		///	return LOGICAL_TRUE;
			}
		case ICMP_TYPE_TIME_EXCEEDED:
			{
			unsigned char *pUserData = (unsigned char *)pIcmpHdr + 8 + sizeof(SIpHeader) + 4;
			unsigned short *pProcessID = (unsigned short *)pUserData;
			if (*pProcessID != CalcPacketID())
				continue;
			unsigned short *pSeqNum = pProcessID + 1;
			if (*pSeqNum != CalcPacketSequenceNumber())
				continue;

			memcpy(&m_ReplySource, &From, sizeof(From));
			m_cReplyType = pIcmpHdr->i_type;
			m_bTimedOut = FALSE;
			OnTtlExceed(pIpHdr, pIcmpHdr, m_ReplySource.sin_addr.S_un.S_addr, pData, m_dwReceivedDataLength);
		//	return TRUE;
			}

		default:
			break;
		} // end of switch (pIcmpHdr->i_type)
	} // end while !timeout
	return LOGICAL_TRUE;
}

// Address-mask requests
LOGICAL CIcmpSocketBaseFunc::SendAddressMaskRequest(IPADDRESS_TYPE  IpAddress)
{
	// Fill IP-header
	SIpHeader *pIpHeader = (SIpHeader *)(m_ucBuffer);
	int nIpHeaderLength = 0;
	unsigned short nPacketSize = 32;
	if (HasOwnIpHeader())
	{
		nIpHeaderLength = FillIpHeader(pIpHeader, nPacketSize, m_uSrcIP, IpAddress);
	}
	else
	{
		nIpHeaderLength = 0; // do not count IP-header octets
		nPacketSize -= sizeof(SIpHeader);
	}

	// Fill ICMP-header
	SIcmpHeader	*pIcmpHeader = (SIcmpHeader *)(m_ucBuffer+nIpHeaderLength);
	pIcmpHeader->i_type  = ICMP_TYPE_ADDRESS_MASK_REQUEST;
	pIcmpHeader->i_code  = 0;
	pIcmpHeader->i_id    = CalcPacketID();
	pIcmpHeader->i_chksum= 0;
	pIcmpHeader->i_seq   = m_SeqNumber = CalcPacketSequenceNumber();
	pIcmpHeader->timestamp = CalcTimeStamp();
	
	// Calc checksum
	pIcmpHeader->i_chksum = CalcCheckSum((unsigned short *)pIcmpHeader, nPacketSize);

	// Send packet
	m_dwLastEchoReqTime = GetTickCount();
	unsigned int nSentBytes;
	LOGICAL bWritten = WriteTo(m_ucBuffer, nPacketSize, nSentBytes, IpAddress, 0);
	return bWritten;
}

LOGICAL CIcmpSocketBaseFunc::SetPingTimeout
(
	DWORD a_dwTimeout // in milliseconds
)
{
	m_dwTimeout = a_dwTimeout;
	return SetReadTimeout(m_dwTimeout) && SetWriteTimeout(m_dwTimeout);
}

void CIcmpSocketBaseFunc::FillDataBuffer
(
	char *pDataBuffer,
	unsigned int nUserDataSize
)
{
	for (unsigned int i = 0; i < nUserDataSize; i++)
		*(pDataBuffer+i) = 'A' + ((BYTE)i % 26);
}


/////////////////////////////////////////////////////////////////////////////
// CIcmpSocket class implementation

CIcmpSocket::CIcmpSocket()
{
	Create(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	SetPingTimeout(1000);
}

//virtual
CIcmpSocket::~CIcmpSocket()
{
}

/////////////////////////////////////////////////////////////////////////////
// CIcmpFakeSocket class implementation

CIcmpFakeSocket::CIcmpFakeSocket()
{
	Create(AF_INET, SOCK_RAW, IPPROTO_IP);
	SetIpHeaderIncl(LOGICAL_TRUE);	
	SetPingTimeout(1000);
}

//virtual
CIcmpFakeSocket::~CIcmpFakeSocket()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTracert class implementation

CTracert::CTracert()
{
	m_dwLastErrorCode = 0;
}

//virtual
CTracert::~CTracert()
{
}

SIpTraceRoute *CTracert::GetRoute(const char *pszIpAddress, int nMaxHops/* = 30*/)
{
	IPADDRESS_TYPE IpAddress = inet_addr(pszIpAddress);
	return GetRoute(IpAddress, nMaxHops);
}

SIpTraceRoute *CTracert::GetRoute(IPADDRESS_TYPE IpAddress, int nMaxHops/* = 30*/)
{
	m_Route.bReached = 0; //unreached yet
	for (char nHops = 1; nHops < nMaxHops; nHops++)
	{
		if (m_Route.bReached != 0)
			break;
		
		if (!SetSendTTL(nHops))
		{
			m_dwLastErrorCode = GetLastErrorCode();
			return NULL;
		}
		if (!SendEchoRequest(IpAddress))
		{
			m_dwLastErrorCode = GetLastErrorCode();
			return NULL;
		}
		
		m_nTtlExceededReplyCount = 0;
		if (!ReceiveEchoReply())
		{
			m_dwLastErrorCode = GetLastErrorCode();
			return NULL;
		}
		if (IsTimedOut() || IsBad())
		{
			m_Route.bReached = 0; //unreached
			break;
		}
		unsigned long ReplierIP = GetReplierIpAddress();
		
		m_Route.HostAddresses[nHops-1] = 0;
		if (IsEchoReply())
		{
			m_Route.HostAddresses[nHops-1] = ReplierIP;
			if (ReplierIP == IpAddress)
				m_Route.bReached = 1; //reached
		}
		else if (IsTtlExceed())
		{
			if (m_nTtlExceededReplyCount > 1)
				m_Route.HostAddresses[nHops-1] = 0; // mark many replies as unknown
			else
				m_Route.HostAddresses[nHops-1] = ReplierIP;
		}
		else if (IsDestUnreachable())
		{
			m_Route.bReached = 0; //unreached
			char szReplier[16]; AddressToString(szReplier, ReplierIP);
			printf("Host unreachable. Reported by %s\n", szReplier);
			break;
		}
		m_Route.nHops = nHops;

			char szReplier[16]; AddressToString(szReplier, m_Route.HostAddresses[nHops-1]);
			printf("nHops = %d, replier = %s\n", nHops, szReplier);
	}
	return &m_Route;
}

//virtual
void CTracert::OnTtlExceed(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, IPADDRESS_TYPE SourceIP, char *pData, unsigned int nDataSize)
{
	++m_nTtlExceededReplyCount;
}

