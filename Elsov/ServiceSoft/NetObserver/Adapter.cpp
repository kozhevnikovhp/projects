/* Adapter.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	01 Jul 2006 - initial creation
	10 Aug 2006 - RIP requests
	21 Aug 2006 - tracert and keepalive
	28 Nov 2006 - loading network objects and their properties from database
	20 May 2007 - transit traffic processing and storing it in DB
	16 Nov 2007 - global IDatabaseAPI pointer instead of member variables
	14 Feb 2006 - background threads functionality moved from those thread to adapter class
	03 Mar 2008 - NetBIOS stat requests
	13 Mar 2008 - NetBIOS stat request answer processing moved to CHost class
	24 Mar 2007 - global CNetwork pointer instead of member variable
	28 Mar 2008 - organize hosts behind routers thread
	15 Apr 2008 - organize hosts behind routers - some real functionality
	13 May 2008 - dynamically recalculated WAN connection elasticity (based on tracert counts)
	09 Sep 2008 - scheduler and schedule thread
	10 Sep 2008 - got rid of keep-alive thread, its functionality moved to scheduler
	11 Sep 2008 - got rid of subnet thread, its functionality moved to scheduler
*/

#include "stdafx.h"
#include "Adapter.h"
#include "Network.h"
#include "NetInterface.h"
#include "SpySocketThread.h"
#include "BackgroundThread.h"
#include "Rip.h"
#include "Portable.h"


/////////////////////////////////////////////////////////////////////
// class CScheduleItem

CScheduleItem::CScheduleItem(CSchedule *pSchedule, CNetObject *pObject, SCHEDULE_ACTION pAction, time_t TargetTime)
{
	m_pSchedule = pSchedule;
	m_pObject = pObject;
	m_pAction = pAction;
	m_TargetTime = TargetTime;
}

// copy constructor
CScheduleItem::CScheduleItem(CScheduleItem *pItem)
{
	m_pSchedule = pItem->m_pSchedule;
	m_pObject = pItem->m_pObject;
	m_pAction = pItem->m_pAction;
	m_TargetTime = pItem->m_TargetTime;
}

//virtual
CScheduleItem::~CScheduleItem()
{
}

LOGICAL CScheduleItem::Execute()
{
	return m_pAction(this);
}

/////////////////////////////////////////////////////////////////////
// class CSchedule

CSchedule::CSchedule()
{
}

//virtual
CSchedule::~CSchedule()
{
}

void CSchedule::NewItem(CNetObject *pObject, SCHEDULE_ACTION pAction, time_t TargetTime)
{
	m_CriticalSection.Enter();
	CScheduleItem Item(this, pObject, pAction, TargetTime);
	AddItem(Item);
	m_CriticalSection.Leave();
}

void CSchedule::AddItem(CScheduleItem &Item)
{
	push(Item);
}

// Return true if executed, and false if next item is still in the future
LOGICAL CSchedule::ExecuteNextItem()
{
	time_t CurrentTime; time(&CurrentTime);
	CStackCriticalSection section(&m_CriticalSection, NULL);
	if (empty())
		return LOGICAL_FALSE;
	CScheduleItem &Item = top();
	if (Item.GetTargetTime() > CurrentTime)
		return LOGICAL_FALSE; // just wait some time

	LOGICAL bNeedNextTime = Item.Execute();
	if (bNeedNextTime)
	{
		CScheduleItem NextItem(&Item);
		AddItem(NextItem);
	}
	pop();

	return LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////
// CAdapter class implementation

CAdapter::CAdapter()
{
	m_uLastRipRouterRequestTime = 0;
	
	m_pSpyThread = NULL;
	m_pSpyThread = new CSpySocketThread(this);

	m_pOrganizeThread = NULL;
	m_pOrganizeThread = new COrganizeThread(this);

	m_pDatabaseThread = NULL;
	m_pDatabaseThread = new CDatabaseThread(this);

	m_pTracertThread = NULL;
	m_pTracertThread = new CTracertThread(this);

	m_pScheduleThread = NULL;
	m_pScheduleThread = new CScheduleThread(this, &m_Schedule);

	m_pFirstInterface = NULL;
	m_uLastRipRouterRequestTime = portableGetCurrentTimeMsec();

	memset(m_szMacAddress, 0, sizeof(m_szMacAddress));
	m_bInitialized = LOGICAL_FALSE;
}

//virtual
CAdapter::~CAdapter()
{
	if (m_pSpyThread)
	{
		TRACE("Spy thread is stopping\n");
		m_pSpyThread->Stop();
		TRACE("Spy thread is stopped\n");
		delete m_pSpyThread;
		TRACE("Spy thread object is deleted\n");
	}

	if (m_pOrganizeThread)
	{
		TRACE("Organize thread is stopping\n");
		m_pOrganizeThread->Stop();
		TRACE("Organize thread is stopped\n");
		delete m_pOrganizeThread;
		TRACE("Organize thread object is deleted\n");
	}
	
	if (m_pDatabaseThread)
	{
		TRACE("Database thread is stopping\n");
		m_pDatabaseThread->Stop();
		TRACE("Database thread is stopped\n");
		delete m_pDatabaseThread;
		TRACE("Database thread object is deleted\n");
	}
	
	if (m_pTracertThread)
	{
		TRACE("Tracert thread is stopping\n");
		m_pTracertThread->Stop();
		TRACE("Tracert thread is stopped\n");
		delete m_pTracertThread;
		TRACE("Tracert thread object is deleted\n");
	}
	
	if (m_pScheduleThread)
	{
		TRACE("Schedule thread is stopping\n");
		m_pScheduleThread->Stop();
		TRACE("Schedule thread is stopped\n");
		delete m_pScheduleThread;
		TRACE("Schedule thread object is deleted\n");
	}
	
	if (m_pFirstInterface)
		delete m_pFirstInterface;
}

void CAdapter::Init(char *pszMacAddress)
{
	strcpy(m_szMacAddress, pszMacAddress);
	m_bInitialized = LOGICAL_TRUE;
}

LOGICAL CAdapter::Run()
{
	if (m_bInitialized != LOGICAL_TRUE)
		return LOGICAL_FALSE;
	
	ASSERT(strlen(m_szMacAddress) > 0);
	if (!m_pFirstInterface)
		return LOGICAL_FALSE;
	if (m_pSpyThread)
		m_pSpyThread->Run();
	if (m_pOrganizeThread)
		m_pOrganizeThread->Run();
	if (m_pDatabaseThread)
		m_pDatabaseThread->Run();
	if (m_pTracertThread)
		m_pTracertThread->Run();
	if (m_pScheduleThread)
		m_pScheduleThread->Run();
	return LOGICAL_TRUE;
}

IPADDRESS_TYPE CAdapter::GetIpAddress() const
{
	return m_pFirstInterface->GetIpAddress();
}

CIcmpSocket *CAdapter::GetBestPinger(IPADDRESS_TYPE uIpAddress) const
{
	CNetInterface *pBestInterface = GetBestInterface(uIpAddress);
	return pBestInterface->GetPinger();
}

CNetInterface *CAdapter::GetBestInterface(IPADDRESS_TYPE uIpAddress) const
{
	int n1CountBest = 0;
	CNetInterface *pBestInterface = m_pFirstInterface;
	for (CNetInterface *pIface = m_pFirstInterface; pIface; pIface = pIface->GetNextOnAdapter())
	{
		IPADDRESS_TYPE uInterfaceSubnetID = pIface->GetIpAddress() & pIface->GetSubnetMask();
		IPADDRESS_TYPE uAnd = uIpAddress & uInterfaceSubnetID;
		// count 1s
		int n1Count = Count1s(uIpAddress & uInterfaceSubnetID);
		if (n1Count > n1CountBest)
		{
			n1CountBest = n1Count;
			pBestInterface = pIface;
		}
	}
	return pBestInterface;
}

CHost *CAdapter::FindHost(IPADDRESS_TYPE uIpAddress)
{
	return (CHost *)m_HostTable.FindObject(uIpAddress);
}

CSubnet *CAdapter::FindSubnet(IPADDRESS_TYPE uIpAddress)
{
	return (CSubnet *)m_SubnetTable.FindObject(uIpAddress);
}

void CAdapter::SendRipFullTableRequest()
{
	unsigned long uCurrentTime = portableGetCurrentTimeMsec();
	if (uCurrentTime - m_uLastRipRouterRequestTime >= 60*1000)
	{
		for (CNetInterface *pIface = m_pFirstInterface; pIface; pIface = pIface->GetNextOnAdapter())
		{
			pIface->SendRipFullTableRequest();
		}
		m_uLastRipRouterRequestTime = uCurrentTime - RandomTimeShift(5000);
	}
}

/*void CAdapter::KeepAliveHost(CHost *pHost)
{
	pHost->KeepAlive();
}*/

void CAdapter::TracertHost(CHost *pHost)
{
	if (pHost->NeedTracert())
	{		
		SIpTraceRoute *pRoute = pHost->Tracert();
		if (pRoute)
		{
			/*CHost *pConnectedThroughHost = FindHost(uConnectedThroughIP);
			if (pHost)
			{ // it can be not created yet - tracert can be faster than spy thread
				m_pNetwork->HostTracerted(pHost, )
				pHost->ConnectedThrough(pConnectedThroughHost);
				CSubnet *pSubnet = pHost->GetSubnet();
				if (pSubnet)
					pSubnet->ConnectedThrough(pConnectedThroughHost);
			}*/
			// Update connection elasticities
			for (int nHop = 0; nHop < pRoute->nHops-1; nHop++)
			{
				IPADDRESS_TYPE uAddress1 = pRoute->HostAddresses[nHop];
				CHost *pHost1 = FindHost(uAddress1);
				if (!pHost1)
					continue;
				pHost1->SendNetBiosStatRequest();
				IPADDRESS_TYPE uAddress2 = pRoute->HostAddresses[nHop+1];
				CHost *pHost2 = FindHost(uAddress2);
				if (!pHost2)
					continue;
				pHost2->SendNetBiosStatRequest();
				pHost2->ConnectionToHostUsed(pHost, pHost1);
			}
			g_pWholeNetwork->HostTracerted(pHost, pRoute);
		}
	}
}

void CAdapter::TracertAllHosts()
{
	SOctetConverter converter; converter.addr = 0;
	CHost *pFirstHost = GetNextHost(converter.addr);
	if (!pFirstHost)
		return;
	CHost *pHost = pFirstHost;
	do
	{
		if (pHost)
			TracertHost(pHost);
		portableSwitchContext();
		pHost = GetNextHost(converter.addr);
	} while (pHost != pFirstHost);
}

void CAdapter::UpdateTrafficDatabase()
{
///	CStackCriticalSection section(&m_TrafficStoringCriticalSection, "Traffic");
	SOctetConverter converter; converter.addr = 0;
	CHost *pFirstHost = GetNextHost(converter.addr);
	if (!pFirstHost)
		return;
	CHost *pHost = pFirstHost;
	do
	{
		m_TrafficStoringCriticalSection.Enter();
		if (pHost)
			pHost->UpdateTrafficDatabase();
		m_TrafficStoringCriticalSection.Leave();
		portableSwitchContext();
		pHost = GetNextHost(converter.addr);
	} while (pHost != pFirstHost);
}

void CAdapter::OrganizeHostsBehindRouters()
{
	SOctetConverter converter; converter.addr = 0;
	CHost *pFirstHost = GetNextHost(converter.addr);
	if (!pFirstHost)
		return;
	CHost *pHost = pFirstHost;
	do
	{
		if (pHost)
			pHost->OrganizeHostsBehind();
		portableSwitchContext();
		pHost = GetNextHost(converter.addr);
	} while (pHost != pFirstHost);
}

CHost *CAdapter::AddNewInterface(IPADDRESS_TYPE uIpAddress, IPADDRESS_TYPE uSubnetMask)
{
	CSubnet *pSubnet = CreateSubnet(uIpAddress & uSubnetMask, uSubnetMask);
	g_pWholeNetwork->SetDistanceToSubnet(pSubnet, 1);
	CNetInterface *pNewInterface = new CNetInterface(this, uIpAddress, pSubnet);
	CHost *pHost = CreateHost(uIpAddress, pSubnet);

	pNewInterface->Chain(m_pFirstInterface);
	m_pFirstInterface = pNewInterface;

	pNewInterface->Run();
	pNewInterface->TracertHost(pHost);

	return pHost;
}

CHost *CAdapter::CreateHost(IPADDRESS_TYPE uIpAddress, CSubnet *pSubnet, long DatabaseID)
{
	// Try to find interface in the system except this adapter.
	// If found do nothing, i.e. this address is catched due to routing in multihomed systems and
	// there is no any host with this IP-address on this adapter.
	CNetInterface *pIface = g_pWholeNetwork->FindInterface(uIpAddress, this);
	if (pIface)
		return NULL;

	CHost *pHost = (CHost *)m_HostTable.FindObject(uIpAddress);
	if (pHost)
		return pHost; // exists already
	pHost = new CHost(this, uIpAddress, DatabaseID);
	g_pWholeNetwork->NewHostCreated(pHost);
	AddHostToSchedule(pHost);
	if (pSubnet)
		g_pWholeNetwork->AddHostToSubnet(pHost, pSubnet);
	g_pWholeNetwork->HostChanged(pHost);

	m_HostTable.AddObject(pHost, uIpAddress);
	return pHost;
}

CHost *CAdapter::CreateHost(SHostDesc *pDesc)
{
	// Try to find interface in the system except this adapter.
	// If found do nothing, i.e. this address is catched due to routing in multihomed systems and
	// there is no any host with this IP-address on this adapter.
	CNetInterface *pIface = g_pWholeNetwork->FindInterface(pDesc->uIpAddress, this);
	if (pIface)
		return NULL;

	CHost *pHost = (CHost *)m_HostTable.FindObject(pDesc->uIpAddress);
	if (pHost)
	{
		pHost->Init(pDesc);
	}
	else
	{
		pHost = new CHost(this, pDesc);
		m_HostTable.AddObject(pHost, pDesc->uIpAddress);
		g_pWholeNetwork->NewHostCreated(pHost);
	}
	AddHostToSchedule(pHost);
	
	IPADDRESS_TYPE uSubnetAddress = CalcSubnetAddress(pDesc->uIpAddress, pDesc->uSubnetMask);
	CSubnet *pSubnet = (CSubnet *)m_SubnetTable.FindObject(uSubnetAddress);
	if (pSubnet)
		g_pWholeNetwork->AddHostToSubnet(pHost, pSubnet);
	//m_pNetwork->HostChanged(pHost); //we just read the DB, no any update needed
	
	return pHost;
}

// Fill schedule about this host
void CAdapter::AddHostToSchedule(CHost *pHost)
{
	time_t CurrentTime;
	time(&CurrentTime);
	m_Schedule.NewItem(pHost, CNetObject::Scheduler_KeepAliveHost, pHost->GetLastKeepAliveTime());
	m_Schedule.NewItem(pHost, CNetObject::Scheduler_SendSnmpRequest, CurrentTime);
}

// Fill schedule about this subnet
void CAdapter::AddSubnetToSchedule(CSubnet *pSubnet)
{
	m_Schedule.NewItem(pSubnet, CNetObject::Scheduler_SubnetBackground, pSubnet->GetLastBackgroundTime());
}

CSubnet *CAdapter::CreateSubnet(IPADDRESS_TYPE uIpAddress, IPADDRESS_TYPE uSubnetMask, long DatabaseID)
{
	CSubnet *pSubnet = (CSubnet *)m_SubnetTable.FindObject(uIpAddress);
	if (pSubnet)
		return pSubnet; // exist already
	pSubnet = new CSubnet(this, uIpAddress, uSubnetMask, DatabaseID);
	m_SubnetTable.AddObject(pSubnet, uIpAddress);
	g_pWholeNetwork->NewSubnetCreated(pSubnet);
	AddSubnetToSchedule(pSubnet);
	return pSubnet;
}

CSubnet *CAdapter::CreatePseudoSubnet(IPADDRESS_TYPE uIpAddress, IPADDRESS_TYPE uSubnetMask, long DatabaseID)
{
	CSubnet *pSubnet = (CSubnet *)m_SubnetTable.FindObject(uIpAddress);
	if (pSubnet)
		return pSubnet; // exist already
	pSubnet = new CSubnet(this, uIpAddress, uSubnetMask, DatabaseID);
	pSubnet->BePseudo(LOGICAL_TRUE);
	m_SubnetTable.AddObject(pSubnet, uIpAddress);
	AddSubnetToSchedule(pSubnet);
	return pSubnet;
}

CSubnet *CAdapter::CreateSubnet(SSubnetDesc *pDesc)
{
	CSubnet *pSubnet = (CSubnet *)m_SubnetTable.FindObject(pDesc->uIpAddress);
	if (pSubnet)
	{
		pSubnet->Init(pDesc);
	}
	else
	{
		pSubnet = new CSubnet(this, pDesc);
		pSubnet->BePseudo(LOGICAL_TRUE);
		m_SubnetTable.AddObject(pSubnet, pDesc->uIpAddress);
	}
	AddSubnetToSchedule(pSubnet);
	return pSubnet;
}

// Returns LOGICAL_TRUE if this packet is fully processed and no further processing required
// Returns LOGICAL_FALSE if some lower-protocol data processing (UDP, TCP, ICMP etc) is required
LOGICAL CAdapter::IpPacketCaptured(SIpHeader *pIpHeader)
{
	// 1. Find host in table
	CHost *pSrcHost = (CHost *)m_HostTable.FindObject(pIpHeader->sourceIP);
	if (!pSrcHost)
		pSrcHost = CreateHost(pIpHeader->sourceIP);
	if (!pSrcHost)
		return LOGICAL_FALSE;	

	pSrcHost->IpPacketCaptured(pIpHeader);
	
	return LOGICAL_FALSE;	
}

void CAdapter::HostNameResolved(IPADDRESS_TYPE uIpAddress, char *pszName)
{
	CHost *pHost = (CHost *)m_HostTable.FindObject(uIpAddress);
	if (!pHost)
		pHost = CreateHost(uIpAddress);
	if (pHost)
	{
		if (pHost->SetOwnName(pszName))
			g_pWholeNetwork->HostChanged(pHost);
	}
}

CHost *CAdapter::GetNextHost(IPADDRESS_TYPE &uIpAddress)
{
	return (CHost *)m_HostTable.GetNextObject(uIpAddress);
}

CSubnet *CAdapter::GetNextSubnet(IPADDRESS_TYPE &uIpAddress)
{
	return (CSubnet *)m_SubnetTable.GetNextObject(uIpAddress);
}

CNetInterface *CAdapter::FindInterface(IPADDRESS_TYPE uIpAddress)
{
	for (CNetInterface *pIface = m_pFirstInterface; pIface; pIface = pIface->GetNextOnAdapter())
	{
		if (pIface->GetIpAddress() == uIpAddress)
			return pIface;
	}
	return NULL;
}

// ICMP
void CAdapter::IcmpPacketCaptured(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	if (1)
	{
		CHost *pSrcHost = (CHost *)m_HostTable.FindObject(pIpHeader->sourceIP);
		if (pSrcHost)
		{
			CHost *pDstHost = (CHost *)m_HostTable.FindObject(pIpHeader->destIP);
			m_TrafficStoringCriticalSection.Enter();
			pSrcHost->UpdateIcmpTrafficFrom(pDstHost, pIpHeader, pIcmpHeader);
			m_TrafficStoringCriticalSection.Leave();
		}
	}

	switch (pIcmpHeader->i_type)
	{
	case ICMP_TYPE_ECHO_REPLY:
		ProcessIcmpEchoReply(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_DEST_UNREACHABLE:
		ProcessIcmpDestUnreachable(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_SOURCE_QUENCH:
		ProcessIcmpSourceQuench(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_REDIRECT:
		ProcessIcmpRedirect(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_ECHO_REQUEST:
		ProcessIcmpEchoRequest(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_ROUTER_ADVERTISEMENT:
		ProcessIcmpRouterAdvertisement(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_ROUTER_SOLICITATION:
		ProcessIcmpRouterSolicitation(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_TIME_EXCEEDED:
		ProcessIcmpTimeExceeded(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_BAD_PARAMETER:
		ProcessIcmpBadParameter(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_TIMESTAMP_QUERY:
		ProcessIcmpTimeStampQuery(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_TIMESTAMP_REPLY:
		ProcessIcmpTimeStampReply(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_INFORMATION_REQUEST:
		ProcessIcmpInformationRequest(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_INFORMATION_REPLY:
		ProcessIcmpInformationReply(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_ADDRESS_MASK_REQUEST:
		ProcessIcmpAddressMaskRequest(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	case ICMP_TYPE_ADDRESS_MASK_REPLY:
		ProcessIcmpAddressMaskReply(pIpHeader, pIcmpHeader, pUserData, nUserDataLength);
		break;
	}
}

void CAdapter::ProcessIcmpEchoReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	CHost *pHost = (CHost *)m_HostTable.FindObject(pIpHeader->sourceIP);
	ASSERT(pHost);
	if (FindInterface(pIpHeader->destIP))
	{
		g_pWholeNetwork->MakeHostUnreach(pHost, LOGICAL_FALSE);
	}
}

void CAdapter::ProcessIcmpDestUnreachable(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	SIpHeader *pCustomIP = (SIpHeader *)((char *)pIcmpHeader+8);
#ifdef _DEBUG
	char szSource[16]; AddressToString(szSource, pIpHeader->sourceIP);
	TRACE("Destination unreachable report from %s:\n", szSource);
	AddressToString(szSource, pCustomIP->sourceIP);
	char szDest[16]; AddressToString(szDest, pCustomIP->destIP);
	TRACE("Original packet from %s to %s, proto %d\n", szSource, szDest, pCustomIP->proto);
	if (pCustomIP->proto == 17)
	{
		SUdpHeader *pUdpHeader = (SUdpHeader *)(pCustomIP+1);
		unsigned short nPort = ntohs(pUdpHeader->DstPortNo);
		TRACE("Port %d\n", nPort);
	}
#endif
	CHost *pDstHost = (CHost *)m_HostTable.FindObject(pCustomIP->destIP);
	if (!pDstHost)
		return;
	CHost *pSrcHost = (CHost *)m_HostTable.FindObject(pIpHeader->sourceIP);
	if (!pSrcHost)
		return;
	LOGICAL bDstChanged = LOGICAL_FALSE;
	LOGICAL bSrcChanged = LOGICAL_FALSE;

	if (pIcmpHeader->i_code == ICMP_CODE_PORT_UNREACHABLE)
	{
		if (pIpHeader->sourceIP != pCustomIP->destIP)
		{ // port unreachable message is received but destination and source address are different -> that is the same host
			LOGICAL bSomethingChanged = g_pWholeNetwork->AddHardwareConnection(pSrcHost, pDstHost,LOGICAL_TRUE);
			bDstChanged |= bSomethingChanged;
			bSrcChanged |= bSomethingChanged;
		}
	}
	else if (pIcmpHeader->i_code == ICMP_CODE_HOST_UNREACHABLE)
	{
		if (FindInterface(pCustomIP->destIP))
		{
			g_pWholeNetwork->MakeHostUnreach(pDstHost, LOGICAL_FALSE);
		}
		bSrcChanged |= pSrcHost->BeRouter();
	}
	if (bSrcChanged)
		g_pWholeNetwork->HostChanged(pSrcHost);
	if (bDstChanged)
		g_pWholeNetwork->HostChanged(pDstHost);
}

void CAdapter::ProcessIcmpSourceQuench(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpRedirect(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpEchoRequest(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpRouterAdvertisement(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpRouterSolicitation(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpTimeExceeded(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpBadParameter(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpTimeStampQuery(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpTimeStampReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpInformationRequest(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpInformationReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpAddressMaskRequest(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIcmpAddressMaskReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	CHost *pHost = (CHost *)m_HostTable.FindObject(pIpHeader->sourceIP);
	ASSERT(pHost);
	pHost->AddressMaskReplyReceived(pIpHeader, pIcmpHeader);

	if (FindInterface(pIpHeader->destIP))
	{
		g_pWholeNetwork->MakeHostUnreach(pHost, LOGICAL_FALSE);
	}
}

// IGMP
void CAdapter::IgmpPacketCaptured(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	if (1)
	{
		CHost *pSrcHost = (CHost *)m_HostTable.FindObject(pIpHeader->sourceIP);
		if (pSrcHost)
		{
			CHost *pDstHost = (CHost *)m_HostTable.FindObject(pIpHeader->destIP);
			m_TrafficStoringCriticalSection.Enter();
			pSrcHost->UpdateIgmpTrafficFrom(pDstHost, pIpHeader, pIgmpHeader);
			m_TrafficStoringCriticalSection.Leave();
		}
	}

	switch (pIgmpHeader->type)
	{
	case 0x11: // Membersip query
		ProcessIgmpMembershipQuery(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
		break;
	case 0x12: // Membersip report v1
		ProcessIgmpMembershipReportV1(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
		break;
	case 0x13: // DVMRP v1
		ProcessDVMRP(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
		break;
	case 0x16: // Membersip report v2
		ProcessIgmpMembershipReportV2(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
		break;
	case 0x17: // leave group v2
		ProcessIgmpLeaveGroup(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
		break;
	case 0x22: // Membersip report v3
		ProcessIgmpMembershipReportV3(pIpHeader, pIgmpHeader, pUserData, nUserDataLength);
		break;
	default:
		{
			char szTo[16], szFrom[16];
			TRACE("IGMP\tlen = %5d (from %s\t to %s)\n", ntohs(pIpHeader->total_len),
						AddressToString(szFrom, pIpHeader->sourceIP),
						AddressToString(szTo, pIpHeader->destIP));
			TRACE("Unknown IGMP datagram code: 0x%X\n", pIgmpHeader->type);
			break;
		}
	}
}

void CAdapter::ProcessIgmpMembershipQuery(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIgmpMembershipReportV1(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessDVMRP(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIgmpMembershipReportV2(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIgmpMembershipReportV3(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

void CAdapter::ProcessIgmpLeaveGroup(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
}

// UDP
void CAdapter::UdpPacketCaptured(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	unsigned short SrcPortNo = ntohs(pUdpHeader->SrcPortNo);
	unsigned short DstPortNo = ntohs(pUdpHeader->DstPortNo);

	if (1)
	{
		CHost *pSrcHost = (CHost *)m_HostTable.FindObject(pIpHeader->sourceIP);
		if (pSrcHost)
		{
			CHost *pDstHost = (CHost *)m_HostTable.FindObject(pIpHeader->destIP);
			m_TrafficStoringCriticalSection.Enter();
			pSrcHost->UpdateUdpTrafficFrom(pDstHost, pIpHeader, pUdpHeader);
			m_TrafficStoringCriticalSection.Leave();
		}
	}

	if (SrcPortNo == 137 || DstPortNo == 137)
		ProcessUdp137(pIpHeader, pUdpHeader, pUserData, nUserDataLength); // NetBIOS
	else if (SrcPortNo == 138 || DstPortNo == 138)
		ProcessUdp138(pUdpHeader, pUserData, nUserDataLength); // NetBIOS
	else if (SrcPortNo == 520 || DstPortNo == 520)
		ProcessRIP(pIpHeader, pUdpHeader, pUserData, nUserDataLength); // RIP
	else
	{
		char szTo[16], szFrom[16];
		TRACE("UDP:%5d/%5d len = %5d (from %s\t to %s)\n", SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
			AddressToString(szFrom, pIpHeader->sourceIP),
			AddressToString(szTo, pIpHeader->destIP));
	}
}

void CAdapter::ProcessUdp137(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	SNameServiceHeader *pNsHeader = (SNameServiceHeader *)pUserData;
	unsigned char *pCurrent = pUserData + sizeof(SNameServiceHeader);
	if (pNsHeader->IsTypeAnswer())
	{ // response
		if (pNsHeader->IsOperationQuery())
		{ // query
			ProcessUdp137QueryAnswer(pIpHeader, pNsHeader, pCurrent);
		}
	}
	else
	{ // query
		if (pNsHeader->IsOperationQuery())
		{ // query
			ProcessUdp137Query(pIpHeader, pNsHeader, pCurrent);
		}
		else if (pNsHeader->IsOperationRegistration())
		{ // registration
			ProcessUdp137Registration(pIpHeader, pNsHeader, pCurrent);
		}
	}
}

void CAdapter::ProcessUdp137QueryAnswer(SIpHeader *pIpHeader, SNameServiceHeader *pNsHeader, unsigned char *pCurrent)
{
	LOGICAL bChanged = LOGICAL_FALSE;
	CHost *pSrc = FindHost(pIpHeader->sourceIP);
	ASSERT(pSrc);
	if (pSrc)
		bChanged = pSrc->NetBiosDgmReceived(pNsHeader, pCurrent);
	if (bChanged)
		g_pWholeNetwork->HostChanged(pSrc);
}

void CAdapter::ProcessUdp137Query(SIpHeader *pIpHeader, SNameServiceHeader *pNsHeader, unsigned char *pCurrent)
{
	char szName[255];
	szName[0] = 0;
	for (int iQuestion = 0; iQuestion < pNsHeader->GetQuestionCount(); iQuestion++)
	{
		pCurrent += RFC883_UnpackName(pCurrent, szName, sizeof(szName));

		// Type
		//unsigned short *pType = (unsigned short *)pCurrent;
		//unsigned short Type = ntohs(*pType);
		pCurrent += 2;

		// Class
		//unsigned short *pClass = (unsigned short *)pCurrent;
		//unsigned short Class = ntohs(*pClass);
		pCurrent += 2;

		//TRACE("Query: %s\n", szName);
	}
}

void CAdapter::ProcessUdp137Registration(SIpHeader *pIpHeader, SNameServiceHeader *pNsHeader, unsigned char *pCurrent)
{
	char szName[255];
	szName[0] = 0;
	for (int iQuestion = 0; iQuestion < pNsHeader->GetQuestionCount(); iQuestion++)
	{
		pCurrent += RFC883_UnpackName(pCurrent, szName, sizeof(szName));

		// Type
		//unsigned short *pType = (unsigned short *)pCurrent;
		//unsigned short Type = ntohs(*pType);
		pCurrent += 2;

		// Class
		//unsigned short *pClass = (unsigned short *)pCurrent;
		//unsigned short Class = ntohs(*pClass);
		pCurrent += 2;

		//TRACE("Registration query: %s\n", szName);
	}
	for (int iAdditionalRec = 0; iAdditionalRec < pNsHeader->GetAdditionalRecordCount(); iAdditionalRec++)
	{
		// Name ref (0xC0, 0x0C)
		pCurrent += 2;

		// Type
		unsigned short *pType = (unsigned short *)pCurrent;
		unsigned short Type = ntohs(*pType);
		pCurrent += 2;

		// Class
		unsigned short *pClass = (unsigned short *)pCurrent;
		unsigned short Class = ntohs(*pClass);
		pCurrent += 2;

		// TTL
		unsigned long *pTTL = (unsigned long *)pCurrent;
		unsigned long TTL = ntohl(*pTTL);
		pCurrent += 4;

		// Node type and something else - 4 octets
		pCurrent += 4;

		// IP-address
		unsigned long *pIP = (unsigned long *)pCurrent;
		unsigned long IP = *pIP;
		pCurrent += 4;

		char szIP[16];
		AddressToString(szIP, IP);
		TRACE("Registration: %s -> %s\n", szName, szIP);
	}
}

void CAdapter::ProcessUdp138(SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	SNetbiosDatagramHeader *pDgmHeader = (SNetbiosDatagramHeader *)pUserData;
	unsigned char *pCurrent = pUserData + sizeof(SNetbiosDatagramHeader);
	if (pDgmHeader->IsDirectUniqueDgm() ||
		pDgmHeader->IsDirectGroupDgm()  ||
		pDgmHeader->IsBroadcastDgm())
	{ // ordinary datagram
		ProcessUdp138OrdinaryDgm(pDgmHeader, pCurrent);
	}
	else if (pDgmHeader->IsErrorDgm())
	{ // error report
		ProcessUdp138ErrorDgm(pDgmHeader, pCurrent);
	}
	else if (pDgmHeader->IsDgmQueryRequest())
	{
		ProcessUdp138QueryRequest(pDgmHeader, pCurrent);
	}
	else if (pDgmHeader->IsPositiveResponse())
	{
		ProcessUdp138PositiveResponse(pDgmHeader, pCurrent);
	}
	else if (pDgmHeader->IsNegativeResponse())
	{
		ProcessUdp138NegativeResponse(pDgmHeader, pCurrent);
	}
}

void CAdapter::ProcessUdp138OrdinaryDgm(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent)
{
	char szName[255];
	szName[0] = 0;
	pCurrent += RFC883_UnpackName(pCurrent, szName, sizeof(szName));
	
	HostNameResolved(pDgmHeader->GetIP(), szName);

	//debug
#ifdef _DEBUG
	char szIP[16];
	AddressToString(szIP, pDgmHeader->GetIP());
	TRACE("Resolved: %s -> %s\n", szName, szIP);
#endif
}

void CAdapter::ProcessUdp138ErrorDgm(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent)
{
}

void CAdapter::ProcessUdp138QueryRequest(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent)
{
}

void CAdapter::ProcessUdp138PositiveResponse(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent)
{
}

void CAdapter::ProcessUdp138NegativeResponse(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent)
{
}

// RIP
void CAdapter::ProcessRIP(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	SRipHeader *pRipHeader = (SRipHeader *)pUserData;
	nUserDataLength -= sizeof(SRipHeader);
	if (pRipHeader->IsQuery())
		return; // do nothing as it is a query

	// Sender is definitely router
	CHost *pHost = (CHost *)m_HostTable.FindObject(pIpHeader->sourceIP);
	ASSERT(pHost);
	g_pWholeNetwork->MakeHostRouter(pHost, LOGICAL_TRUE);
	
	SRipEntry RouteRecord;
	SRipEntry *pEntry = (SRipEntry *)(pRipHeader+1); //entries follow header
	while (nUserDataLength > 0)
	{
		RouteRecord.Metric = ntohl(pEntry->Metric);
		if (pRipHeader->IsVersion2())
		{ // RIP-2 specific
#ifdef _DEBUG
			char szIp[16], szMask[16], szHop[16];
			char szBuffer[512];
			sprintf(szBuffer, "RIP-2: IP = %s SubnetMask = %s NextHop = %s Metric = %d\n",
				AddressToString(szIp, pEntry->IpAddress), AddressToString(szMask, pEntry->SubnetMask),
				AddressToString(szHop, pEntry->NextHop), RouteRecord.Metric);
			TRACE(szBuffer);
#endif
			if (pEntry->IpAddress && pEntry->SubnetMask)
			{
				CSubnet *pSubnet = (CSubnet *)m_SubnetTable.FindObject(pEntry->IpAddress);
				if (!pSubnet)
				{
					pSubnet = CreatePseudoSubnet(pEntry->IpAddress, pEntry->SubnetMask);
					IPADDRESS_TYPE uBroadcastAddress = pSubnet->GetBroadcastAddress();
					CNetInterface *pIface = GetBestInterface(pEntry->IpAddress);
					if (pIface)
						pIface->SendEchoRequest(uBroadcastAddress);
				}
			}			
		}
		else
		{ // RIP-1 specific
#ifdef _DEBUG
			char szIp[16], szMask[16], szHop[16];
			char szBuffer[512];
			sprintf(szBuffer, "RIP-1: IP = %s SubnetMask = %s NextHop = %s Metric = %d\n",
				AddressToString(szIp, pEntry->IpAddress), AddressToString(szMask, pEntry->SubnetMask),
				AddressToString(szHop, pEntry->NextHop), RouteRecord.Metric);
			TRACE(szBuffer);
#endif
		}
		pEntry++;
		nUserDataLength -= sizeof(SRipEntry);
	}
}

// TCP
void CAdapter::TcpPacketCaptured(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
{
	unsigned short SrcPortNo = ntohs(pTcpHeader->SrcPortNo);
	unsigned short DstPortNo = ntohs(pTcpHeader->DstPortNo);

	if (1)
	{
		CHost *pSrcHost = (CHost *)m_HostTable.FindObject(pIpHeader->sourceIP);
		if (pSrcHost)
		{
			CHost *pDstHost = (CHost *)m_HostTable.FindObject(pIpHeader->destIP);
			m_TrafficStoringCriticalSection.Enter();
			pSrcHost->UpdateTcpTrafficFrom(pDstHost, pIpHeader, pTcpHeader);
			m_TrafficStoringCriticalSection.Leave();
		}
	}
}


