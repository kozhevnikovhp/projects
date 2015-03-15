/* File NetBiosSocket.h - header for netbios protocols implementation

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	03 Mar 2008 - initial creation
	25 Jul 2008 - Raw socket set as base class (to make it possible to send requests from port 137)
*/



#ifndef __NETBIOS_SOCKET_H_INCLUDED__
#define __NETBIOS_SOCKET_H_INCLUDED__

#include "Udp.h"

const unsigned short PORT_137 = 137;
const unsigned short PORT_138 = 138;


#pragma pack(push, 1)

///////////////////////////////////////////////////////////////
// Name service, UDP port 137

typedef struct // RFC-883, 1002
{
	unsigned short TransactionID;
	
	unsigned char RD:1;		// Recursion Desired
	unsigned char TC:1;		// TrunCation
	unsigned char AA:1;		// Authoritative Answer 
	unsigned char OPcode:4;
	unsigned char QR:1;		// query(0) or response(1)

	unsigned char Rcode:4;		// Response code
	unsigned char NMflags:3;	// unused
	unsigned char RA:1;		// Recursion Available 
	
	unsigned short QDcount;
	unsigned short ANcount;
	unsigned short NScount;
	unsigned short ARcount;
public:
	LOGICAL IsTypeQuery() { return (QR == 0); }
	LOGICAL IsTypeAnswer() { return !IsTypeQuery(); }

	LOGICAL IsOperationQuery() { return (OPcode == 0); }
	LOGICAL IsOperationRegistration() { return (OPcode == 0x5); }
	LOGICAL IsOperationRelease() { return (OPcode == 0x6); }
	LOGICAL IsOperationWAck() { return (OPcode == 0x7); }
	LOGICAL IsOperationRefresh() { return (OPcode == 0x8); }

	int GetQuestionCount() { return ntohs(QDcount); }
	int GetAnswerCount() { return ntohs(ANcount); }
	int GetAdditionalRecordCount() { return ntohs(ARcount); }
} SNameServiceHeader;

typedef struct
{
	SIpHeader IpHeader;
	SUdpHeader UdpHeader;
	SNameServiceHeader NSHeader;
	unsigned char body[38];
} SNameServiceDgm;


///////////////////////////////////////////////////////////////
// Name service, UDP port 138

typedef struct // RFC-883, 1002
{
protected:
	unsigned char MSG_TYPE;			// 0x10...0x16
	unsigned char Flags;			// flags
	unsigned short DatagramID;		// Datagram ID 
	unsigned long SourceIP;			// Source IP-address
	unsigned short SourcePort;		// Source port No
	unsigned short DatagramLength;	// Datagram length
	unsigned short PacketOffset;	//

public:
	LOGICAL IsDirectUniqueDgm() { return (MSG_TYPE == 0x10); }
	LOGICAL IsDirectGroupDgm() { return (MSG_TYPE == 0x11); }
	LOGICAL IsBroadcastDgm() { return (MSG_TYPE == 0x12); }
	LOGICAL IsErrorDgm() { return (MSG_TYPE == 0x13); }
	LOGICAL IsDgmQueryRequest() { return (MSG_TYPE == 0x14); }
	LOGICAL IsPositiveResponse() { return (MSG_TYPE == 0x15); }
	LOGICAL IsNegativeResponse() { return (MSG_TYPE == 0x16); }

	IPADDRESS_TYPE GetIP() { return SourceIP; }	
} SNetbiosDatagramHeader;

#pragma pack(pop)

int RFC883_UnpackName(unsigned char *pCompressedName, char *pszBufferForResult, int nBufferSize);

class CNetBiosSocket : public CIpRawSocket
{
// Constructors/destructors
public:
	CNetBiosSocket();
	virtual ~CNetBiosSocket();
// Public methods
public:
	LOGICAL SendStatRequest(IPADDRESS_TYPE uSrcIP, IPADDRESS_TYPE uDstIP);
	virtual LOGICAL HasOwnIpHeader() { return LOGICAL_TRUE; }
	virtual unsigned char IpHeader_Proto(){ return IPPROTO_UDP; }

// Protected members
protected:
	SNameServiceDgm m_NetBiosStatRequest;
};

#endif // __NETBIOS_SOCKET_H_INCLUDED__