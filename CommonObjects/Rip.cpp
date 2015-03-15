/* File Rip.h - implementation for RIPv, RIPv2 protocols

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	10 Aug 2006 - initial creation

*/

#include "stdafx.h"
#include "Rip.h"

/////////////////////////////////////////////////////////////////////
// Class CRipSocket implementation

CRipSocket::CRipSocket()
{
}

//virtual
CRipSocket::~CRipSocket()
{
}

BOOL CRipSocket::SendRipFullTableRequest(IPADDRESS_TYPE uIpAddress)
{
	SRipHeaderAndEntry datagram;
	memset(&datagram, 0, sizeof(datagram));
	datagram.header.command = RIP_COMMAND_QUERY;
	datagram.header.version = 2;
	datagram.entry.AddressFamily = 0;
	datagram.entry.Metric = htonl(16);	
	unsigned int nWrittenBytes = 0;
	return WriteTo(&datagram, sizeof(SRipHeaderAndEntry), nWrittenBytes, uIpAddress, RIP_PORT);
}
