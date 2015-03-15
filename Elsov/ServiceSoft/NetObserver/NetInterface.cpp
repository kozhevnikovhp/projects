/* NetInterface.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	01 Jul 2006 - initial creation
	10 Aug 2006 - RIP requests
	03 Mar 2008 - NetBIOS stat requests
	25 Jul 2008 - NetBIOS stat requests from port 137 through raw sockets
*/

#include "stdafx.h"
#include "NetInterface.h"
#include "Adapter.h"
#include "Iphlpapi.h"
#include "DatabaseAPI.h"
#include "GuiAPI.h"

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

LOGICAL CNetInterface::SendEchoRequest(IPADDRESS_TYPE uDestIpAddress)
{
	return m_Pinger.SendEchoRequest(uDestIpAddress);
}

LOGICAL CNetInterface::SendNetBiosStatRequest(IPADDRESS_TYPE uDestIpAddress)
{
	TRACE("NetBios stat request\n");
	return m_NetBiosSocket.SendStatRequest(m_uIpAddress, uDestIpAddress);
}

LOGICAL CNetInterface::SendRipFullTableRequest()
{
	TRACE("Interface %s is sending RIP full table request\n", GetIpAddressString());
	return m_RipSocket.SendRipFullTableRequest(GetBroadcastAddress());
}

SIpTraceRoute *CNetInterface::TracertHost(CHost *pHost)
{
	LOGICAL bUpdated = LOGICAL_FALSE;
	
	// Send ARP (Windows-specific code)
	unsigned long MacAddress[2];
	unsigned long MacAddressLength = 6;
//	char szMacAddress[32];
//	char szAdd[16];
	DWORD ec = ::SendARP(pHost->GetIpAddress(), GetIpAddress(), MacAddress, &MacAddressLength);
	if (ec == NO_ERROR)
	{
		g_pWholeNetwork->AddHostToSubnet(pHost, GetSubnet());
		unsigned char *pcBytesOfMacAddress = (unsigned char *)MacAddress;
		if (pHost->SetMacAddress(pcBytesOfMacAddress, MacAddressLength))
			g_pWholeNetwork->HostChanged(pHost);
	}

	m_Tracert.SetSendTTL(g_pGui->GetMaxDistanceInHops());
	SIpTraceRoute *pRoute = m_Tracert.GetRoute(pHost->GetIpAddress());
	if (pRoute && pRoute->bReached)
	{
		if (pHost->GetDistance() != pRoute->nHops)
			bUpdated = g_pWholeNetwork->SetDistanceToHost(pHost, pRoute->nHops);

		if (pRoute->nHops == 1)
		{ // connected directly
			if (IsMyAddress(pHost->GetIpAddress()))
			{
				g_pWholeNetwork->AddHostToSubnet(pHost, GetSubnet()); // host of this iface's subnet
			}
		}
		else
		{
			for (int i = 1; i < pRoute->nHops; i++)
			{
				CHost *pHost1 = m_pAdapter->FindHost(pRoute->HostAddresses[i-1]);
				if (pHost1)
				{
					CHost *pHost2 = m_pAdapter->FindHost(pRoute->HostAddresses[i]);
					if (pHost2)
					{
						bUpdated |= g_pWholeNetwork->AddWanConnection(pHost1, pHost2);
						bUpdated |= pHost1->AnotherHostConnectedThrough(pHost2);
					}
				}
			}
		}
	}

	return pRoute;
}
