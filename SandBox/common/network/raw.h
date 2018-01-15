#ifndef __RAW_H_INCLUDED__
#define __RAW_H_INCLUDED__

#include "ip.h"

namespace common {

namespace network {

// cIpRawSocket - raw sockets classes will be derived from it
class IpRawSocket : public IpSocket
{
    typedef IpSocket inherited;

// Constructors/destructors
public:
    IpRawSocket();
    virtual ~IpRawSocket();
// Public methods
    unsigned char GetReceivedTTL() { return m_IPHeader.ttl; }
    unsigned char GetReceivedTOS() { return m_IPHeader.tos; }
	void SetSrcIP(IPADDRESS_TYPE uIP) { m_uSrcIP = uIP; }
// Public overridables
    virtual bool ReadFrom(void *pBufferToFill, size_t nBufferSize, int &nReadBytes, IPADDRESS_TYPE &IpAddress, IPPORT &PortNo);
// Public members

protected:
// Protected methods
	void ClearIpHeader();
	void ExtractIPHeader(void *pBuffer);
	bool SetIpHeaderIncl(bool bIncludeIpHeader);
	int FillIpHeader(SIpHeader *pIpHeader, int nPacketSize, unsigned long uSrcIP, unsigned long uDstIP);
// Protected overridables
	virtual bool ReadFrom(void *pBufferToFill, size_t nBufferSize, int &nReadBytes, struct sockaddr *pSrc);
	// Own IP-header methods
	virtual bool HasOwnIpHeader() { return false; }
	virtual int FillIpHeaderOptions(SIpHeader *pIpHeader) { return 0; } // no any option by default; returns length of options field in octets
	virtual unsigned char IpHeader_Proto(){ return IPPROTO_IP; }
	virtual unsigned char IpHeader_TTL() { return m_cTTL; }
	virtual unsigned char IpHeader_TOS() { return m_cTOS; }
// Protected members
	SIpHeader m_IPHeader;
	IPADDRESS_TYPE m_uSrcIP; // this address will be substituted in IP-header for sockets having their own ones
};


} // namespace network

} // namespace common

#endif // __RAW_H_INCLUDED__
