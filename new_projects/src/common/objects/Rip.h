/* File Rip.h - header for RIPv, RIPv2 protocols implementation

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	10 Aug 2006 - initial creation

*/



#ifndef __RIP_H_INCLUDED__
#define __RIP_H_INCLUDED__

#include "Udp.h"

const unsigned short RIP_PORT = 520;

// RIP-protocol (RFC-1058(v1), RFC-1388(v2))
const unsigned char RIP_COMMAND_QUERY = 1;
const unsigned char RIP_COMMAND_REPLY = 2;
const unsigned char RIP_VERSION_1 = 1;
const unsigned char RIP_VERSION_2 = 2;

#pragma pack(push, 1)

typedef struct
{
public:
	BOOL IsQuery() { return (command == RIP_COMMAND_QUERY); }
	BOOL IsVersion2() { return (version == RIP_VERSION_2); }

	unsigned char command;
	unsigned char version;
	unsigned short unused;
protected:
} SRipHeader;

typedef struct
{
	unsigned short AddressFamily;
	unsigned short RouteTag;
	unsigned long IpAddress;
	unsigned long SubnetMask;	
	unsigned long NextHop;	
	unsigned long Metric;	
} SRipEntry;

typedef struct
{
	SRipHeader header;
	SRipEntry entry;
} SRipHeaderAndEntry;

#pragma pack(pop)

class CRipSocket : public CUdpSocket
{
// Constructors/destructors
public:
	CRipSocket();
	virtual ~CRipSocket();
// Public methods
public:
	BOOL SendRipFullTableRequest(IPADDRESS_TYPE uIpAddress);
};

#endif // __RIP_H_INCLUDED__