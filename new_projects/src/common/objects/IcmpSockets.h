/* File IcmpSockets.h
Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org
 
Revision history:
	11 Dec 2001 - initial creation
*/


#ifndef __ICMP_SOCKETS_H_INCLUDED__
#define __ICMP_SOCKETS_H_INCLUDED__

#include "IpSockets.h"

// ICMP message types (RFC 792)
const unsigned char ICMP_TYPE_ECHO_REPLY			= 0;
const unsigned char ICMP_TYPE_DEST_UNREACHABLE		= 3;
const unsigned char ICMP_TYPE_SOURCE_QUENCH			= 4;
const unsigned char ICMP_TYPE_REDIRECT				= 5;
const unsigned char ICMP_TYPE_ECHO_REQUEST			= 8;
const unsigned char ICMP_TYPE_ROUTER_ADVERTISEMENT	= 9;
const unsigned char ICMP_TYPE_ROUTER_SOLICITATION	= 10;
const unsigned char ICMP_TYPE_TIME_EXCEEDED			= 11;
const unsigned char ICMP_TYPE_BAD_PARAMETER			= 12;
const unsigned char ICMP_TYPE_TIMESTAMP_QUERY		= 13;
const unsigned char ICMP_TYPE_TIMESTAMP_REPLY		= 14;
const unsigned char ICMP_TYPE_INFORMATION_REQUEST	= 15;
const unsigned char ICMP_TYPE_INFORMATION_REPLY		= 16;
const unsigned char ICMP_TYPE_ADDRESS_MASK_REQUEST	= 17;
const unsigned char ICMP_TYPE_ADDRESS_MASK_REPLY	= 18;

// ICMP codes for "destination unreachable" message type
const unsigned char ICMP_CODE_NETWORK_UNREACHABLE		= 0x00;
const unsigned char ICMP_CODE_HOST_UNREACHABLE			= 0x01;
const unsigned char ICMP_CODE_PROTOCOL_UNREACHABLE		= 0x02;
const unsigned char ICMP_CODE_PORT_UNREACHABLE			= 0x03;
const unsigned char ICMP_CODE_FRAGMENTATION_REQUIRED	= 0x04;
const unsigned char ICMP_CODE_SOURCE_ROUTER_FAILED		= 0x05;
const unsigned char ICMP_CODE_DST_NETWORK_UNKNOWN_ERROR	= 0x06;
const unsigned char ICMP_CODE_DST_HOST_UNKNOWN_ERROR	= 0x07;
const unsigned char ICMP_CODE_DST_NETWORK_ADM_PROHIBITED= 0x09;
const unsigned char ICMP_CODE_DST_HOST_ADM_PROHIBITED	= 0x10;
const unsigned char ICMP_CODE_DST_NETWORK_TOS_PROHIBITED= 0x11;
const unsigned char ICMP_CODE_DST_HOST_TOS_PROHIBITED	= 0x12;
const unsigned char ICMP_CODE_COMM_ADM_PROHIBITED		= 0x13;



const unsigned short ICMP_MIN				= 8;
const unsigned int ICMP_DEFAULT_PACKET_SIZE	= 32;
const unsigned int ICMP_MAX_PACKET_SIZE		= 1024;

class CIcmpSocketBaseFunc : public CIpRawSocket
{
//Attributes
public:
	CIcmpSocketBaseFunc();
	virtual ~CIcmpSocketBaseFunc();
// Public methods
	// Echo requests
	LOGICAL SendEchoRequest(IPADDRESS_TYPE IpAddress, int nPacketSize = 32);
	LOGICAL SendEchoRequest(const char *pszIpAddress, int nPacketSize = 32);
	LOGICAL SendBroadcastEchoRequest(int nPacketSize = 32);
	LOGICAL SendRouterSolicitation(IPADDRESS_TYPE IpAddress);

	LOGICAL ReceiveEchoReply();

	// Address-mask requests
	LOGICAL SendAddressMaskRequest(IPADDRESS_TYPE  IpAddress);

	LOGICAL SetPingTimeout(DWORD a_dwTimeout);
	int GetMaxPacketSize() { return sizeof(m_ucBuffer); }
	unsigned long GetDelayTime() { return m_dwDelayTime; }
	unsigned long GetReceivedDataLength() { return m_dwReceivedDataLength; }
	LOGICAL IsTimedOut() { return m_bTimedOut; }
	LOGICAL IsBad() { return m_bBad; }
	LOGICAL IsEchoReply() { return m_cReplyType == ICMP_TYPE_ECHO_REPLY; }
	LOGICAL IsDestUnreachable() { return m_cReplyType == ICMP_TYPE_DEST_UNREACHABLE; }
	LOGICAL IsTtlExceed() { return m_cReplyType == ICMP_TYPE_TIME_EXCEEDED; }
	unsigned long GetReplierIpAddress() { return m_ReplySource.sin_addr.S_un.S_addr; }
	char *GetReplierIpAddressString() { return inet_ntoa(m_ReplySource.sin_addr); }

// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
	virtual unsigned short CalcPacketID() { return LOWORD(GetCurrentThreadId()); }
	virtual LOGICAL IsOurPacketID(SIcmpHeader *a_pIcmpHdr) { return a_pIcmpHdr->i_id == CalcPacketID(); }
	virtual unsigned short CalcPacketSequenceNumber() { return LOWORD(m_SocketSeqNumber); }
	virtual LOGICAL IsOurPacketSequenceNumber(SIcmpHeader *pIcmpHdr) { return pIcmpHdr->i_seq == CalcPacketSequenceNumber();}
	virtual unsigned long  CalcTimeStamp() { m_TimeStamp = GetTickCount(); return m_TimeStamp; }
	virtual LOGICAL IsOurPacketTimeStamp(SIcmpHeader *pIcmpHdr) { return (m_TimeStamp == pIcmpHdr->timestamp); }
	virtual void FillDataBuffer(char *pDataBuffer, unsigned int nUserDataSize);
	virtual void OnEchoReceived(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, IPADDRESS_TYPE SourceIP, unsigned char *pData, unsigned int nDataSize) {}
	virtual void OnDestUnreachable(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, IPADDRESS_TYPE SourceIP, unsigned char *pData, unsigned int nDataSize) {}
	virtual void OnTtlExceed(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, IPADDRESS_TYPE SourceIP, unsigned char *pData, unsigned int nDataSize) {}
	// Own IP-header methods
	virtual unsigned char IpHeader_Proto() { return IPPROTO_ICMP; }
	virtual unsigned char IpHeader_TTL() { return m_cTTL; }
	virtual unsigned char IpHeader_TOS() { return m_cTOS; }

// Protected members
	unsigned char m_ucBuffer[8192];
	unsigned short m_SeqNumber;
	unsigned long m_TimeStamp;
	unsigned long m_dwTimeout;
	unsigned long m_dwLastEchoReqTime;
	LOGICAL m_bTimedOut;
	LOGICAL m_bBad;
	unsigned long m_dwDelayTime;
	unsigned long m_dwReceivedDataLength;
	sockaddr_in m_ReplySource;
	char m_cReplyType;
};

class CIcmpSocket : public CIcmpSocketBaseFunc
{
//Attributes
public:
	CIcmpSocket();
	virtual ~CIcmpSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
// Protected members
};

class CIcmpFakeSocket : public CIcmpSocketBaseFunc
{
//Attributes
public:
	CIcmpFakeSocket();
	virtual ~CIcmpFakeSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
	// Own IP-header methods
	virtual LOGICAL HasOwnIpHeader() { return LOGICAL_TRUE; }
// Protected members
};

typedef struct SIpTraceRoute
{
	LOGICAL bReached;
	int nHops;
	IPADDRESS_TYPE HostAddresses[256];
	SIpTraceRoute()
	{
		bReached = LOGICAL_FALSE;
		nHops = 0;
	}
} SIpTraceRoute;

class CTracert : public CIcmpSocket
{
//Attributes
public:
	CTracert();
	virtual ~CTracert();
// Public methods
	SIpTraceRoute *GetRoute(const char *pszIpAddress, int nMaxHops = 30);
	SIpTraceRoute *GetRoute(IPADDRESS_TYPE IpAddress, int nMaxHops = 30);
	
// Public overridables
	virtual void OnTtlExceed(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, IPADDRESS_TYPE SourceIP, char *pData, unsigned int nDataSize);
// Public members

protected:
// Protected methods
// Protected overridables
// Protected members
	SIpTraceRoute m_Route;
	int m_nTtlExceededReplyCount; // number of replies TTL exceeded. Counted to skip node(s) if more then 1 reply come.
};



#endif //__ICMP_SOCKETS_H_INCLUDED__