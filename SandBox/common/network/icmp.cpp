/* File IcmpSockets.cpp
Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org
 
Revision history:
	11 Dec 2001 - initial creation
*/

#include "icmp.h"
#include "misc.h"
#include "portable/thread.h"
#include <stdio.h> //debug
#include <string.h> //debug
#include <errno.h>

namespace common {

namespace network {

/////////////////////////////////////////////////////////////////////////////
// CIcmpSocketBaseFunc class implementation

CIcmpSocketBaseFunc::CIcmpSocketBaseFunc()
{
	m_bTimedOut = false;
	m_bBad = false;
	m_dwDelayTime = -1;
	m_SeqNumber = 0;
	m_dwLastEchoReqTime = common::portable::portableGetCurrentTimeMsec() - 200;
	m_dwReceivedDataLength = 0;
	m_cReplyType = 0;
}

//virtual
CIcmpSocketBaseFunc::~CIcmpSocketBaseFunc()
{
}

bool CIcmpSocketBaseFunc::SendEchoRequest
(
	IPADDRESS_TYPE IpAddress,
	int nPacketSize /* = 32 */
)
{
	if (nPacketSize > GetMaxPacketSize())
		return false;
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
	m_dwLastEchoReqTime = common::portable::portableGetCurrentTimeMsec();
	int nSentBytes;
	bool bWritten = WriteTo(m_ucBuffer, nPacketSize, nSentBytes, IpAddress, 0);
	return bWritten;
}

bool CIcmpSocketBaseFunc::SendEchoRequest(const char *pszIpAddress, int nPacketSize /* = 32 */)
{
	IPADDRESS_TYPE IpAddress = inet_addr(pszIpAddress);
	return SendEchoRequest(IpAddress, nPacketSize);
}

bool CIcmpSocketBaseFunc::SendBroadcastEchoRequest(int nPacketSize /* = 32 */)
{
	return SendEchoRequest(INADDR_BROADCAST, nPacketSize);
}

bool CIcmpSocketBaseFunc::SendRouterSolicitation(IPADDRESS_TYPE IpAddress)
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
	m_dwLastEchoReqTime = common::portable::portableGetCurrentTimeMsec();
	int nSentBytes;
	bool bWritten = WriteTo(m_ucBuffer, nPacketSize, nSentBytes, IpAddress, 0);
	return bWritten;
}

bool CIcmpSocketBaseFunc::ReceiveEchoReply()
{
	m_bTimedOut = true;
	m_cReplyType = -1;
	int nReadBytes;
	sockaddr From;
	while (common::portable::portableGetCurrentTimeMsec() - m_dwLastEchoReqTime < m_dwTimeout)
	{
		m_bBad = false;
		m_dwDelayTime = -1;

		if (!ReadFrom(&m_ucBuffer[0], sizeof(m_ucBuffer), nReadBytes, &From))
		{
			return (getLastErrorCode() == ETIMEDOUT);
		}
		
		SIpHeader *pIpHdr = (SIpHeader *)m_ucBuffer;
		unsigned short	IpHdrLen = pIpHdr->h_len * 4;

		if (nReadBytes < (int)(IpHdrLen + ICMP_MIN))
		{
			m_bBad = true;
			continue;
		}

		unsigned char *pData = m_ucBuffer + IpHdrLen + sizeof(SIcmpHeader);
		m_dwReceivedDataLength = nReadBytes;

		SIcmpHeader *pIcmpHdr = (SIcmpHeader *)(m_ucBuffer + IpHdrLen);
		m_bBad = false;
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
			m_bTimedOut = false;
			m_dwDelayTime = common::portable::portableGetCurrentTimeMsec() - pIcmpHdr->timestamp;
			OnEchoReceived(pIpHdr, pIcmpHdr, getIP(&m_ReplySource), pData, m_dwReceivedDataLength);
			return true;
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
			m_bTimedOut = false;
			OnDestUnreachable(pIpHdr, pIcmpHdr, getIP(&m_ReplySource), pData, m_dwReceivedDataLength);
		///	return true;
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
			m_bTimedOut = false;
			OnTtlExceed(pIpHdr, pIcmpHdr, getIP(&m_ReplySource), pData, m_dwReceivedDataLength);
		//	return TRUE;
		}

		default:
			break;
		} // end of switch (pIcmpHdr->i_type)
	} // end while !timeout
	return true;
}

std::string CIcmpSocketBaseFunc::getReplierIpAddressString()
{
    return addressToDotNotation(getIP(&m_ReplySource)); 
}

// Address-mask requests
bool CIcmpSocketBaseFunc::SendAddressMaskRequest(IPADDRESS_TYPE  IpAddress)
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
	m_dwLastEchoReqTime = common::portable::portableGetCurrentTimeMsec();
	int nSentBytes;
	bool bWritten = WriteTo(m_ucBuffer, nPacketSize, nSentBytes, IpAddress, 0);
	return bWritten;
}

bool CIcmpSocketBaseFunc::SetPingTimeout
(
	unsigned int dwTimeout // in milliseconds
)
{
	m_dwTimeout = dwTimeout;
	return (SetReadTimeout(m_dwTimeout) && SetWriteTimeout(m_dwTimeout));
}

//virtual
unsigned short CIcmpSocketBaseFunc::CalcPacketID()
{
	return (unsigned short)(common::portable::getCurrentThreadID() & 0xFFFF);
}

//virtual
bool CIcmpSocketBaseFunc::IsOurPacketID(const SIcmpHeader *pIcmpHdr)
{
	return (pIcmpHdr->i_id == CalcPacketID());
}

//virtual
unsigned short CIcmpSocketBaseFunc::CIcmpSocketBaseFunc::CalcPacketSequenceNumber()
{
	return (unsigned short)(m_SocketSeqNumber & 0xFFFF);
}

//virtual
bool CIcmpSocketBaseFunc::IsOurPacketSequenceNumber(SIcmpHeader *pIcmpHdr)
{
	return (pIcmpHdr->i_seq == CalcPacketSequenceNumber());
}

//virtual
unsigned long  CIcmpSocketBaseFunc::CalcTimeStamp()
{
	m_TimeStamp = common::portable::portableGetCurrentTimeMsec();
	return m_TimeStamp;
}

//virtual
bool CIcmpSocketBaseFunc::IsOurPacketTimeStamp(SIcmpHeader *pIcmpHdr)
{
	return (m_TimeStamp == pIcmpHdr->timestamp);
}

//virtual
void CIcmpSocketBaseFunc::FillDataBuffer
(
	char *pDataBuffer,
	unsigned int nUserDataSize
)
{
	for (unsigned int i = 0; i < nUserDataSize; i++)
		*(pDataBuffer+i) = 'A' + ((char)i % 26);
}


/////////////////////////////////////////////////////////////////////////////
// cIcmpSocket class implementation

cIcmpSocket::cIcmpSocket()
{
	create(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	SetPingTimeout(1000);
}

//virtual
cIcmpSocket::~cIcmpSocket()
{
}

/////////////////////////////////////////////////////////////////////////////
// cIcmpFakeSocket class implementation

cIcmpFakeSocket::cIcmpFakeSocket()
{
	create(AF_INET, SOCK_RAW, IPPROTO_IP);
	SetIpHeaderIncl(true);
	SetPingTimeout(1000);
}

//virtual
cIcmpFakeSocket::~cIcmpFakeSocket()
{
}


/////////////////////////////////////////////////////////////////////////////
// cTracert class implementation

cTracert::cTracert()
{
	m_LastErrorCode = 0;
}

//virtual
cTracert::~cTracert()
{
}

sIpTraceRoute *cTracert::getRoute(const char *pszIpAddress, int nMaxHops/* = 30*/)
{
	IPADDRESS_TYPE IpAddress = inet_addr(pszIpAddress);
	return getRoute(IpAddress, nMaxHops);
}

sIpTraceRoute *cTracert::getRoute(IPADDRESS_TYPE IpAddress, int nMaxHops/* = 30*/)
{
	m_Route.bReached = 0; //unreached yet
	for (char nHops = 1; nHops < nMaxHops; nHops++)
	{
		if (m_Route.bReached != 0)
			break;
		
		if (!SetSendTTL(nHops))
		{
			storeLastErrorCode();
			return NULL;
		}
		if (!SendEchoRequest(IpAddress))
		{
			storeLastErrorCode();
			return NULL;
		}
		
		m_nTtlExceededReplyCount = 0;
		if (!ReceiveEchoReply())
		{
			storeLastErrorCode();
			return NULL;
		}
		if (IsTimedOut() || IsBad())
		{
			m_Route.bReached = 0; //unreached
			break;
		}
		IPADDRESS_TYPE ReplierIP = getIP(&m_ReplySource);
		
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
            std::string strReplier = addressToDotNotation(ReplierIP);
			printf("Host unreachable. Reported by %s\n", strReplier.c_str());
			break;
		}
		m_Route.nHops = nHops;

        std::string strReplier = addressToDotNotation(m_Route.HostAddresses[nHops-1]);
		printf("nHops = %d, replier = %s\n", nHops, strReplier.c_str());
	}
	return &m_Route;
}

//virtual
void cTracert::OnTtlExceed(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, IPADDRESS_TYPE SourceIP, char *pData, unsigned int nDataSize)
{
	++m_nTtlExceededReplyCount;
}

} // namespace network

} // namespace common
