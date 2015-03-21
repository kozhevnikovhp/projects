/* File IcmpSockets.h
Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org
 
Revision history:
	11 Dec 2001 - initial creation
*/


#ifndef __ICMP_SOCKETS_H_INCLUDED__
#define __ICMP_SOCKETS_H_INCLUDED__

#include "raw.h"
#include <string>

namespace common {

namespace network {

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

class CIcmpSocketBaseFunc : public cIpRawSocket
{
//Attributes
public:
	CIcmpSocketBaseFunc();
	virtual ~CIcmpSocketBaseFunc();
// Public methods
	// Echo requests
	bool SendEchoRequest(IPADDRESS_TYPE IpAddress, int nPacketSize = 32);
	bool SendEchoRequest(const char *pszIpAddress, int nPacketSize = 32);
	bool SendBroadcastEchoRequest(int nPacketSize = 32);
	bool SendRouterSolicitation(IPADDRESS_TYPE IpAddress);

	bool ReceiveEchoReply();

	// Address-mask requests
	bool SendAddressMaskRequest(IPADDRESS_TYPE  IpAddress);

	bool SetPingTimeout(unsigned int dwTimeout);
	int GetMaxPacketSize() { return sizeof(m_ucBuffer); }
	unsigned long GetDelayTime() { return m_dwDelayTime; }
	unsigned long GetReceivedDataLength() { return m_dwReceivedDataLength; }
	bool IsTimedOut() { return m_bTimedOut; }
	bool IsBad() { return m_bBad; }
	bool IsEchoReply() { return m_cReplyType == ICMP_TYPE_ECHO_REPLY; }
	bool IsDestUnreachable() { return m_cReplyType == ICMP_TYPE_DEST_UNREACHABLE; }
	bool IsTtlExceed() { return m_cReplyType == ICMP_TYPE_TIME_EXCEEDED; }
    std::string getReplierIpAddressString();

// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
	virtual unsigned short CalcPacketID();
	virtual bool IsOurPacketID(const SIcmpHeader *pIcmpHdr);
	virtual unsigned short CalcPacketSequenceNumber();
	virtual bool IsOurPacketSequenceNumber(SIcmpHeader *pIcmpHdr);
	virtual unsigned long  CalcTimeStamp();
	virtual bool IsOurPacketTimeStamp(SIcmpHeader *pIcmpHdr);
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
	bool m_bTimedOut;
	bool m_bBad;
	unsigned long m_dwDelayTime;
	unsigned long m_dwReceivedDataLength;
	sockaddr m_ReplySource;
	char m_cReplyType;
};

class cIcmpSocket : public CIcmpSocketBaseFunc
{
//Attributes
public:
	cIcmpSocket();
	virtual ~cIcmpSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
// Protected members
};

class cIcmpFakeSocket : public CIcmpSocketBaseFunc
{
//Attributes
public:
	cIcmpFakeSocket();
	virtual ~cIcmpFakeSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
	// Own IP-header methods
	virtual bool HasOwnIpHeader() { return true; }
// Protected members
};

typedef struct sIpTraceRoute
{
	bool bReached;
	int nHops;
	IPADDRESS_TYPE HostAddresses[256];
	sIpTraceRoute()
	{
		bReached = false;
		nHops = 0;
	}
} sIpTraceRoute;

class cTracert : public cIcmpSocket
{
//Attributes
public:
	cTracert();
	virtual ~cTracert();
// Public methods
	sIpTraceRoute *getRoute(const char *pszIpAddress, int nMaxHops = 30);
	sIpTraceRoute *getRoute(IPADDRESS_TYPE IpAddress, int nMaxHops = 30);
	
// Public overridables
	virtual void OnTtlExceed(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, IPADDRESS_TYPE SourceIP, char *pData, unsigned int nDataSize);
// Public members

protected:
// Protected methods
// Protected overridables
// Protected members
	sIpTraceRoute m_Route;
	int m_nTtlExceededReplyCount; // number of replies TTL exceeded. Counted to skip node(s) if more then 1 reply come.
};


} // namespace network

} // namespace common


#endif //__ICMP_SOCKETS_H_INCLUDED__
