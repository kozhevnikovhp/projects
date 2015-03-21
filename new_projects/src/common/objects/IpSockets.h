/* File IpSockets.h
Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org
 
Revision history:
	11 Dec 2001 - initial creation
	25 Jul 2008 - Raw sockets: pseudo checksums for UDP/TCP, SRC IpAddress in IP-header etc
*/
#ifndef __IP_SOCKETS_H_INCLUDED__
#define __IP_SOCKETS_H_INCLUDED__

#include <Winsock2.h>
#include "Portable.h"

// Additional constants
const IPADDRESS_TYPE LOCAL_HOST = 0x0100007F; // ("127.0.0.1:)
const IPADDRESS_TYPE INADDR_ANY_HOST = 0x010000E0; //("224.0.0.1")
const IPADDRESS_TYPE INADDR_ANY_ROUTER = 0x020000E0; //("224.0.0.2")

typedef	union
{
	struct { unsigned char b1,b2,b3,b4; } bytes;
	unsigned long addr;
} SOctetConverter;

#pragma pack(push, 1)

// IP header definition
typedef struct 
{
	unsigned char	h_len:4;			// length of the header
	unsigned char	version:4;			// Version of IP
	unsigned char	tos;				// Type of service
	unsigned short	total_len;			// total length of packet
	unsigned short	ident;				// unique identifier
	unsigned short	frag_and_flags;		// flags
	unsigned char	ttl;				// time to live value
	unsigned char	proto;				// protocol (TCP, UDP, etc.)
	unsigned short	checksum;			// IP checksum
	unsigned long	sourceIP;			// source IP address
	unsigned long	destIP;				// destination IP address
} SIpHeader;

typedef struct
{
	unsigned short SrcPortNo;
	unsigned short DstPortNo;
	unsigned short Length;
	unsigned short CRC;
} SUdpHeader;

typedef struct
{
	unsigned short	SrcPortNo;
	unsigned short	DstPortNo;
	unsigned long	Other[5];
} STcpHeader;

struct SPseudoHeader
{
	unsigned int src_addr; // адрес отправителя 
	unsigned int dst_addr; // адрес получателя 
	unsigned char zero ; //начальная установка 
	unsigned char proto; // протокол
	unsigned short length; // длина заголовка 
};
 
// ICMP header definition
typedef struct
{
	unsigned char	i_type;						// ICMP packet type
	unsigned char	i_code;						// type subcode
	unsigned short	i_chksum;					// packet checksum
	unsigned short	i_id;						// unique packet ID
	unsigned short	i_seq;						// packet sequence number
	unsigned long	timestamp;					// timestamp
} SIcmpHeader;

// IGMP-protocol (RFC-1112, 2236, 3376)
typedef struct
{
	unsigned char type;
	unsigned char unused;
	unsigned short CRC;
	unsigned long IP;			// Group address
} SIgmpHeader;

#pragma pack(pop)


// CIpSocket - class for all IP-sockets
class CIpSocket
{
//Constructors/destructors
public:
	CIpSocket();
	virtual ~CIpSocket();
// Public methods
	unsigned long GetLastErrorCode() { return m_dwLastErrorCode; }
	inline SOCKET GetSocket() { return m_Socket; }
	inline LOGICAL IsOpened() { return (m_Socket != INVALID_SOCKET); }
	LOGICAL Bind(unsigned short uPort, IPADDRESS_TYPE InterfaceIpAddress = INADDR_ANY);
	LOGICAL SetReadTimeout(unsigned long a_dwTimeout);
	LOGICAL ReadOverlapped(char *pBufferToFill, int nBufferSize, unsigned short PortNo, void *pData, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	LOGICAL SetWriteTimeout(unsigned long a_dwTimeout);
	LOGICAL SetSendTTL(unsigned char ttl);
	LOGICAL SetSendTOS(unsigned char tos);
	LOGICAL EnableBroadcasting(LOGICAL bEnable = TRUE);
	LOGICAL WriteBroadcast(void *pBufferToSend, unsigned int nBytesToSend, unsigned int &nSentBytes, unsigned short PortNo);
	
	// Error codes
	LOGICAL IsEC_Unreach() { return GetLastErrorCode() == WSAEHOSTUNREACH; }
	LOGICAL IsEC_Timeout() { return GetLastErrorCode() == WSAETIMEDOUT; }
// Public overridables
	virtual void Close();
	virtual LOGICAL ReadFrom(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes, IPADDRESS_TYPE &IpAddress, unsigned short &PortNo);
	virtual LOGICAL WriteTo(void *pBufferToSend, unsigned int nBytesToSend, unsigned int &nSentBytes, IPADDRESS_TYPE IpAddress, unsigned short PortNo);
	virtual LOGICAL WriteTo(void *pBufferToSend, unsigned int nBytesToSend, unsigned int &nSentBytes, char *pszIpAddress, unsigned short PortNo);
// Public members

protected:
// Protected methods
	LOGICAL Create(int af, int type, int protocol);
	LOGICAL SetOption(int level, int OptionName, char *pcValue, int OptionLength);
	LOGICAL GetOption(int level, int OptionName, char *pcValue, int &OptionLength);
	unsigned short CalcCheckSum(unsigned short *pData, unsigned short);
	unsigned short CalcPseudoCheckSum(SIpHeader *pIpHeader, unsigned short *pData, unsigned short);

// Protected overridables
	virtual LOGICAL ReadFrom(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes, struct sockaddr_in *a_pSrc);
	virtual LOGICAL WriteTo(void *pBufferToSend, unsigned int nBytesToSend, unsigned int &nSentBytes, struct sockaddr_in *a_pDest);

// Protected members
	SOCKET m_Socket;
	unsigned long m_dwLastErrorCode;
	unsigned int m_SocketSeqNumber; // numerates all sockets of this application
	static unsigned int m_SocketCount;
	static unsigned int m_MaxSocketSeqNumber;

	unsigned char m_cTTL;
	unsigned char m_cTOS;
};

// CUserSocket - UDP-stack and TCP-stack sockets classes will be derived from it

class CUserSocket : public CIpSocket
{
//Attributes
public:
	CUserSocket();
	virtual ~CUserSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
	virtual unsigned short GetDefaultPort() { return 0; } // must be overriden
// Protected members
};


// CIpRawSocket - raw sockets classes will be derived from it
class CIpRawSocket : public CIpSocket
{
//Attributes
public:
	CIpRawSocket();
	virtual ~CIpRawSocket();
// Public methods
	unsigned char GetReceivedTTL() { return m_IPHeader.ttl; }
	unsigned char GetReceivedTOS() { return m_IPHeader.tos; }
	void SetSrcIP(IPADDRESS_TYPE uIP) { m_uSrcIP = uIP; }
// Public overridables
	virtual LOGICAL ReadFrom(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes, IPADDRESS_TYPE &IpAddress, unsigned short &PortNo);
// Public members

protected:
// Protected methods
	void ClearIpHeader();
	void ExtractIPHeader(void *pBuffer);
	LOGICAL SetIpHeaderIncl(LOGICAL bIncludeIpHeader);
	int FillIpHeader(SIpHeader *pIpHeader, int nPacketSize, unsigned long uSrcIP, unsigned long uDstIP);
// Protected overridables
	virtual LOGICAL ReadFrom(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes, struct sockaddr_in *a_pSrc);
	// Own IP-header methods
	virtual LOGICAL HasOwnIpHeader() { return FALSE; }
	virtual int FillIpHeaderOptions(SIpHeader *pIpHeader) { return 0; } // no any option by default; returns length of options field in octets
	virtual unsigned char IpHeader_Proto(){ return IPPROTO_IP; }
	virtual unsigned char IpHeader_TTL() { return m_cTTL; };
	virtual unsigned char IpHeader_TOS() { return m_cTOS; }
// Protected members
	SIpHeader m_IPHeader;
	IPADDRESS_TYPE m_uSrcIP; // this address will be substituted in IP-header for sockets having their own ones
};

////////////////////////////////////////////////////////////////////////
// Misc
IPADDRESS_TYPE StringToAddress(const char *pszAddress);
char *AddressToString(char *pszAddress, IPADDRESS_TYPE Address);
int Count1s(IPADDRESS_TYPE Address);
IPADDRESS_TYPE GetSubnetMaskByLength(int nMaskLength);
int CalcMaxPossibleHosts(IPADDRESS_TYPE uSubnetMask); // including 0th and last broadcast addresses, just 2**MaskLength
int CalcMaxPossibleHosts(int nMaskLength); // including 0th and last broadcast addresses, just 2**MaskLength


#endif //__IP_SOCKETS_H_INCLUDED__