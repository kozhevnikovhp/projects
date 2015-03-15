/* NetInterface.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	01 Jul 2006 - initial creation
	10 Aug 2006 - RIP requests
*/

#include "stdafx.h"
#include "NetInterface.h"
#include "Adapter.h"
#include "Iphlpapi.h"
#include "DatabaseAPI.h"

/////////////////////////////////////////////////////////////////////
// CNetInterface class implementation

CNetInterface::CNetInterface(CAdapter *pAdapter, IPADDRESS_TYPE uIpAddress, CSubnet *pSubnet)
	: CNetObject(pAdapter, uIpAddress, NET_OBJECT_ID_UNKNOWN)
{
	m_pNextOnAdapter = NULL;
	m_pSubnet = pSubnet;
	SetSubnetMask(pSubnet->GetSubnetMask());
	m_Pinger.Bind(0, uIpAddress);
	m_Pinger.EnableBroadcasting();
	m_Tracert.Bind(0, uIpAddress);
}

//virtual
CNetInterface::~CNetInterface()
{
	if (m_pNextOnAdapter)
		delete m_pNextOnAdapter; // Recursive destructions. I hope the will not be thousands of interface there :-)
}

LOGICAL CNetInterface::Run()
{
	return (TRUE);
}

LOGICAL CNetInterface::IsMyAddress(IPADDRESS_TYPE uIpAddress)
{
	return (GetSubnet()->IsMyAddress(uIpAddress));
}

LOGICAL CNetInterface::SendEchoRequest(IPADDRESS_TYPE uIpAddress)
{
	return m_Pinger.SendEchoRequest(uIpAddress);
}

LOGICAL CNetInterface::SendRipFullTableRequest()
{
	TRACE("Interface %s is sending RIP full table request\n", GetIpAddressString());
	return m_RipSocket.SendRipFullTableRequest(GetBroadcastAddress());
}

// returns last hop if SUCCESS, else 0
IPADDRESS_TYPE CNetInterface::TracertHost(CHost *pHost)
{
	LOGICAL bUpdated = LOGICAL_FALSE;
	IPADDRESS_TYPE uConnectedThroughIP = 0;
	
	// Send ARP (Windows-specific code)
	unsigned long MacAddress[2];
	unsigned long MacAddressLength = 6;
	DWORD ec = ::SendARP(pHost->GetIpAddress(), GetIpAddress(), MacAddress, &MacAddressLength);
	if (ec == NO_ERROR)
	{
		GetSubnet()->AddHost(pHost);
		uConnectedThroughIP = GetIpAddress(); // address of this interface
	}

	SIpTraceRoute *pRoute = m_Tracert.GetRoute(pHost->GetIpAddress());
	if (pRoute && pRoute->bReached)
	{
		if (pHost->GetDistance() != pRoute->nHops)
			bUpdated = pHost->SetDistance(pRoute->nHops);

		if (pRoute->nHops == 1)
		{ // connected directly
			if (IsMyAddress(pHost->GetIpAddress()))
			{
				GetSubnet()->AddHost(pHost); // host of this iface's subnet
			}
			uConnectedThroughIP = GetIpAddress(); // address of this interface
		}
		else
		{
			uConnectedThroughIP = pRoute->HostAddresses[pRoute->nHops-2];
			for (int i = 1; i < pRoute->nHops-1; i++)
			{
				CHost *pHost1 = m_pAdapter->FindHost(pRoute->HostAddresses[i-1]);
				if (pHost1)
				{
					CHost *pHost2 = m_pAdapter->FindHost(pRoute->HostAddresses[i]);
					if (pHost2)
					{
						bUpdated |= pHost1->AddWanConnection(pHost2);
						bUpdated |= pHost2->AddWanConnection(pHost1);
					}
				}
			}
		}
	}

	if (bUpdated)
	{
		m_pAdapter->NetworkChanged();
	}

	return uConnectedThroughIP;
}
