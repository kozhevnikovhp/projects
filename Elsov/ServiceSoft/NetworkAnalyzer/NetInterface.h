/* NetInterface.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	01 Jul 2006 - initial creation
	10 Aug 2006 - RIP requests
*/

#ifndef __NET_INTERFACE_H_INCLUDED__
#define __NET_INTERFACE_H_INCLUDED__

#include "IpSockets.h"
#include "Network.h"
#include "Rip.h"

//Forward declarations
class CNetwork;


// CNetInterface class - interface to network, has IP-address and subnet mask.
class CNetInterface : public CNetObject
{
	friend class CAdapter;
	friend class CHost;
//Constuctors/destructors
public:
	CNetInterface(CAdapter *pAdapter, IPADDRESS_TYPE uIpAddress, CSubnet *pSubnet);
	virtual ~CNetInterface();

// Public methods
public:
	LOGICAL IsMyAddress(IPADDRESS_TYPE uIpAddress);
	CSubnet *GetSubnet() { return m_pSubnet; }
	
// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	CNetInterface *GetNextOnAdapter() { return m_pNextOnAdapter; }
	void Chain(CNetInterface *pNextInterface) { m_pNextOnAdapter = pNextInterface; }
	CIcmpSocket *GetPinger() { return &m_Pinger; }
	LOGICAL SendEchoRequest(IPADDRESS_TYPE uIpAddress);
	LOGICAL SendRipFullTableRequest();
	IPADDRESS_TYPE TracertHost(CHost *pHost);
	LOGICAL Run();

// Protected overridables
protected:

// Protected members
protected:
	CNetInterface *m_pNextOnAdapter;
	CSubnet *m_pSubnet;
	CIcmpSocket m_Pinger;
	CTracert m_Tracert;
	CRipSocket m_RipSocket;
};

#endif //__NET_INTERFACE_H_INCLUDED__
