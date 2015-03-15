/* Network.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	15 Mar 2006 - initial creation
	30 Mar 2006 - skeleton for all possible ICMP packets
	03 Jul 2006 - RIP packets processing and subnets
	05 Jul 2006 - interface list from IpHelper, subnet masks added to interfaces
	10 Aug 2006 - ARP-cache
	21 Aug 2006 - tracert and keepalive
	22 Nov 2006 - storing network objects and their properties in database
	28 Nov 2006 - loading network objects and their properties from database
	29 Nov 2006 - storing/loading flags of network objects in/from database
	20 May 2007 - traffic table
	15 Nov 2007 - global IGuiAPI pointer instead of member variables
	16 Nov 2007 - global IDatabaseAPI pointer instead of member variables
	19 Nov 2007 - global CNetwork pointer instead of member variables
	28 Nov 2007 - own host's MAC-address
	13 Mar 2008 - User name; NetBIOS stat processing, services
	14 Mar 2008 - groups table reading
	20 Mar 2008 - grouping (hosts by groups) loading/storing
	21 Mar 2008 - default groups are dynamically allocated as ordinary ones
	15 Apr 2008 - host merging (due to DHCP, for example)
	15 Apr 2008 - organize hosts behind routers - some real functionality
	24 Apr 2008 - list of WAN connections instead of list of hosts, connection visibility etc
	13 May 2008 - dynamically recalculated WAN connection elasticity (based on tracert counts)
	19 May 2008 - the only CConnection object for each host pair, managed by CNetwork object and referenced by peers
	20 May 2008 - CPlaneHost and CHost objects are merged to CHost
	26 May 2008 - dynamic elasticity of CConnection objects; Subnet creation behind routers; Host neighbours are stored with it
	08 Jul 2008 - color scheme for hosts (through appearance group)
	22 Jul 2008 - subnet color scheme
	09 Sep 2008 - events from scheduler
	10 Sep 2008 - keep-alive stuff updated
	11 Sep 2008 - subnet background stuff updated
*/

#include "stdafx.h"
#include "Network.h"
#include "GuiAPI.h"
#include "Adapter.h"
#include "NetInterface.h"
#include "WholeNetRenderer.h"
#include "Groups.h"
#include "IpHelper.h"
#include <math.h>

CNetwork *g_pWholeNetwork = NULL;
CPortableLocale *g_pLocale = NULL;
CPortableLocale g_Locale;


///////////////////////////////////////////////////////////////////////////////
// CListOfTrafficRecords

void CListOfTrafficRecords::Record(CHost *pSrcHost, CHost *pDstHost, unsigned short proto, unsigned short port, int octets)
{
	STrafficRecord *pFreeRecord = NULL;
	for (iterator it = begin(); it != end(); it++)
	{
		STrafficRecord *pRecord = it;
		if (!pRecord->bInUse)
		{
			if (!pFreeRecord)
				pFreeRecord = pRecord;
		}
		else if (pRecord->pDstHost == pDstHost &&
			pRecord->port == port)
		{
			pRecord->packets++;
			pRecord->octets += octets;
			return;
		}
	}
	STrafficRecord NewRecord;
	STrafficRecord *pRecordToWrite = pFreeRecord;
	if (!pRecordToWrite)
		pRecordToWrite = &NewRecord;
	pRecordToWrite->pSrcHost = pSrcHost;
	pRecordToWrite->pDstHost = pDstHost;

	time(&pRecordToWrite->FromTime);
	// align time interval on 5 seconds
	pRecordToWrite->FromTime /= 5;
	pRecordToWrite->FromTime *= 5;
	
	pRecordToWrite->port = port;
	pRecordToWrite->packets = 1;
	pRecordToWrite->octets = octets;
	pRecordToWrite->proto = proto;
	pRecordToWrite->bInUse = LOGICAL_TRUE;
	if (!pFreeRecord)
		push_back(NewRecord);
}

void CListOfTrafficRecords::UpdateDatabase()
{
	time_t current_time;
	time(&current_time);
	for (iterator it = begin(); it != end(); it++)
	{
		STrafficRecord *pRecord = it;
		if (pRecord->pSrcHost->GetDatabaseID() == NET_OBJECT_ID_UNKNOWN)
			continue;
		time_t delta = current_time-pRecord->FromTime;
		if (delta >= g_pGui->GetTrafficStoreInterval())
		{
			pRecord->ToTime = current_time;
			// store in db
			if (pRecord->octets != 0)
				g_pDB->StoreTrafficRecord(pRecord);
			// prepare record for next
			pRecord->FromTime = pRecord->ToTime = 0;
			pRecord->octets = 0;
			pRecord->packets = 0;
			pRecord->bInUse = LOGICAL_FALSE;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// CPeriodic

CPeriodic::CPeriodic()
{
	m_LastTime = m_NextTime = 0;
}

//virtual
CPeriodic::~CPeriodic()
{
}

time_t CPeriodic::GetTimeTillNext()
{
	time_t current_time; time(&current_time);
	return (GetNextTime() - current_time);
}

LOGICAL CPeriodic::IsExpired()
{
	time_t current_time; time(&current_time);
	if (current_time >= GetNextTime())
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

void CPeriodic::RightNow(time_t DeltaTime)
{
	time(&m_LastTime);
	m_NextTime = m_LastTime + DeltaTime;
}

// Misc
long RandomTimeShift(long Range)
{
	long r = rand();
	r %= Range;
	return r;
}

IPADDRESS_TYPE CalcBroadcastAddress(IPADDRESS_TYPE uAddress, IPADDRESS_TYPE uSubnetMask)
{
	return (uAddress | ~uSubnetMask);
}

IPADDRESS_TYPE CalcSubnetAddress(IPADDRESS_TYPE uAddress, IPADDRESS_TYPE uSubnetMask)
{
	return (uAddress  & uSubnetMask);
}

/////////////////////////////////////////////////////////////////////
// CNetwork class implementation

//Constuctors/destructors
CNetwork::CNetwork()
{
	m_pAdapters = NULL;
	g_pLocale = &g_Locale;
	g_pPingDefaultGroup = CreatePingGroup(LOGICAL_TRUE, LOGICAL_FALSE);
	g_pAppearanceDefaultGroup = CreateAppearanceGroup(LOGICAL_TRUE, LOGICAL_FALSE);
	g_pSnmpDefaultGroup = CreateSnmpGroup(LOGICAL_TRUE, LOGICAL_FALSE);
}

//virtual
CNetwork::~CNetwork()
{
	TRACE("CNetwork object destructed\n");
	for (CListOfConnections::iterator it = m_WanConnections.begin(); it != m_WanConnections.end(); it++)
	{
		CConnection *pConnection = *it;
		delete pConnection;
	}
	for (it = m_HardwareConnections.begin(); it != m_HardwareConnections.end(); it++)
	{
		CConnection *pConnection = *it;
		delete pConnection;
	}
	if (m_pAdapters)
		delete[] m_pAdapters;
}

void CNetwork::TakeExclusiveRights(char *pszText)
{
	m_CriticalSection.Enter();
}

void CNetwork::ReleaseExclusiveRights(char *pszText)
{
	m_CriticalSection.Leave();
}

void CNetwork::IgnoreHost(CHost *pHost)
{
	LOGICAL bCurrent = pHost->IsIgnored();
	bCurrent = !bCurrent;
	LOGICAL bChanged = pHost->BeIgnored(bCurrent);
	g_pGui->HostIgnored(pHost);
}

void CNetwork::HideHost(CHost *pHost)
{
	LOGICAL bCurrent = pHost->IsHidden();
	bCurrent = !bCurrent;
	LOGICAL bChanged = pHost->BeHidden(bCurrent);
	g_pGui->HostHidden(pHost);
}

void CNetwork::ShowIgnoredObjects(LOGICAL bShow)
{
	m_CriticalSection.Enter();
	g_pGui->IgnoredObjectsVisibilityChanged();
	m_CriticalSection.Leave();
}

void CNetwork::ShowHiddenObjects(LOGICAL bShow)
{
	m_CriticalSection.Enter();
	g_pGui->HiddenObjectsVisibilityChanged();
	m_CriticalSection.Leave();
}

void CNetwork::MakeHostUnreach(CHost *pHost, LOGICAL bUnreach)
{
	LOGICAL bUpdated = LOGICAL_FALSE;
	if (pHost->BeUnreach(bUnreach) == LOGICAL_TRUE)
		bUpdated = LOGICAL_TRUE;
	if (bUpdated == LOGICAL_TRUE)
		HostChanged(pHost);
}

void CNetwork::MakeHostRouter(CHost *pHost, LOGICAL bRouter)
{
	LOGICAL bUpdated = LOGICAL_FALSE;
	if (pHost->BeRouter() == LOGICAL_TRUE)
		bUpdated = LOGICAL_TRUE;
	if (bUpdated == LOGICAL_TRUE)
		HostChanged(pHost);
}

void CNetwork::Run()
{
	CIpHelper helper;
	int iAdapter;
	m_nAdapters = helper.GetAdaptersCount();
	m_pAdapters = new CAdapter[m_nAdapters];
	unsigned long nInterfaces = helper.GetIpAddressesCount();
	CHost *pPrevHost = NULL;
	for (unsigned long i = 0; i < nInterfaces; i++)
	{
		if (helper.IsIpAddressLocal(i))
			continue;
		unsigned long IpAddress = helper.GetIpAddress(i);
		unsigned long SubnetMask = helper.GetIpSubnetMask(i);
		if (IpAddress == 0 || SubnetMask == 0)
			continue; // skip unconnected
		iAdapter = helper.GetIfaceIpAddressIndex(i);
		CString AdapterDesc = helper.GetIfaceDesc(iAdapter); //debug
		char szMacAddress[32];
		helper.GetIfacePhysAddressStr(iAdapter, szMacAddress);
		m_pAdapters[iAdapter].Init(szMacAddress);
		CHost *pHost = m_pAdapters[iAdapter].AddNewInterface(IpAddress, SubnetMask);
		pHost->SetDistance(1);
		if (pPrevHost)
			AddHardwareConnection(pHost, pPrevHost, LOGICAL_TRUE);
		pPrevHost = pHost;
	}
	ReadDatabase();
	for (iAdapter = 0; iAdapter < m_nAdapters; iAdapter++)
	{
		m_pAdapters[iAdapter].Run();
	}

	// ARP-cache
	SArpCacheRecord ArpRecord;
	int nArpCashRecords = helper.GetArpCacheRecordCount();
	for (int iArp = 0; iArp < nArpCashRecords; iArp++)
	{
		if (helper.GetArpCacheRecord(iArp, ArpRecord))
		{
			ASSERT(ArpRecord.nAdapter < m_nAdapters);
			CAdapter *pAdapter = &m_pAdapters[ArpRecord.nAdapter];
			CIcmpSocket *pPinger = pAdapter->GetBestPinger(ArpRecord.IpAddress);
#ifdef _DEBUG
			char szIP[16]; AddressToString(szIP, ArpRecord.IpAddress);
			TRACE("ARP-cash: %s\n", szIP);
#endif //_DEBUG
			pPinger->SendEchoRequest(ArpRecord.IpAddress);
		}
	}
	helper.FlashArpCacheTable(); // not needed more but next time will be required again
}

void CNetwork::NewHostCreated(CHost *pNewHost)
{
	if (!pNewHost)
		return;
	m_CriticalSection.Enter();
	pNewHost->InitDefaultGroups();
	pNewHost->InitPeriodics();
	g_pGui->NewHostCreated(pNewHost);
	g_pDB->NewHostCreated(pNewHost);
	m_CriticalSection.Leave();
}

void CNetwork::HostChanged(CHost *pHost)
{
	if (!pHost)
		return;
	m_CriticalSection.Enter();
	g_pGui->HostChanged(pHost);
	g_pDB->HostChanged(pHost);
	m_CriticalSection.Leave();
}

void CNetwork::NewSubnetCreated(CSubnet *pNewSubnet)
{
	if (!pNewSubnet)
		return;
	m_CriticalSection.Enter();
	g_pGui->NewSubnetCreated(pNewSubnet);
	g_pDB->NewSubnetCreated(pNewSubnet);
	m_CriticalSection.Leave();
}

void CNetwork::SubnetChanged(CSubnet *pSubnet)
{
	if (!pSubnet)
		return;
	m_CriticalSection.Enter();
	g_pGui->SubnetChanged(pSubnet);
	g_pDB->SubnetChanged(pSubnet);
	m_CriticalSection.Leave();
}

LOGICAL CNetwork::AddHostToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (!pSubnet || !pHost)
		return LOGICAL_FALSE;
	if (pHost->GetSubnet() != NULL && pHost->GetSubnet() != pSubnet)
	{
		RemoveHostFromSubnet(pHost, pSubnet);
	}
	m_CriticalSection.Enter();
	pSubnet->AddHost(pHost);
	g_pGui->HostAddedToSubnet(pHost, pSubnet);
	g_pDB->HostAddedToSubnet(pHost, pSubnet);
	m_CriticalSection.Leave();
	return LOGICAL_TRUE;
}

void CNetwork::RemoveHostFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (!pSubnet || !pHost)
		return;
	m_CriticalSection.Enter();
	pSubnet->RemoveHost(pHost);
	g_pGui->HostRemovedFromSubnet(pHost, pSubnet);
	g_pDB->HostRemovedFromSubnet(pHost, pSubnet);
	m_CriticalSection.Leave();
}

void CNetwork::HostTracerted(CHost *pHost, SIpTraceRoute *pRoute)
{
	m_CriticalSection.Enter();
	pHost->Tracerted(pRoute);
	CSubnet *pSubnet = pHost->GetSubnet();
	if (pSubnet)
		pSubnet->Tracerted(pRoute);
	m_CriticalSection.Leave();
	HostChanged(pHost);
}

LOGICAL CNetwork::AddWanConnection(CHost *pHost1, CHost *pHost2)
{
	// May be exists already
	CConnection *pConnection = pHost1->FindWanConnection(pHost2);
	if (pConnection)
		return LOGICAL_FALSE; // Exists already, nothing to do
	pConnection = new CConnection(pHost1, pHost2);
	m_WanConnections.push_back(pConnection);
	pHost1->AddWanConnection(pConnection);
	pHost2->AddWanConnection(pConnection);
	return LOGICAL_TRUE;
}

LOGICAL CNetwork::AddHardwareConnection(CHost *pHost1, CHost *pHost2, LOGICAL bRecurse)
{
	// May be exists already
	CConnection *pConnection = pHost1->FindHardwareConnection(pHost2);
	if (pConnection)
		return LOGICAL_FALSE; // Exists already, nothing to do
	pConnection = new CConnection(pHost1, pHost2);
	pHost1->AddHardwareConnection(pConnection, bRecurse);
	pHost2->AddHardwareConnection(pConnection, bRecurse);
	m_HardwareConnections.push_back(pConnection);
	return LOGICAL_TRUE;
}

void CNetwork::NewGroupCreated(CGroup *pGroup)
{
	g_pGui->NewGroupCreated(pGroup);
	g_pDB->NewGroupCreated(pGroup);
}

void CNetwork::GroupChanged(CGroup *pGroup)
{
	g_pGui->GroupChanged(pGroup);
	g_pDB->GroupChanged(pGroup);	
}

void CNetwork::HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup)
{
	m_CriticalSection.Enter();
	g_pGui->HostMovedBetweenGroups(pHost, pOldGroup, pNewGroup);
	g_pDB->HostMovedBetweenGroups(pHost, pOldGroup, pNewGroup);
	m_CriticalSection.Leave();
}

LOGICAL CNetwork::SetDistanceToHost(CHost *pHost, long Distance)
{
	m_CriticalSection.Enter();
	LOGICAL bChanged = pHost->SetDistance(Distance);
	m_CriticalSection.Leave();
	return bChanged;
}

LOGICAL CNetwork::SetDistanceToSubnet(CSubnet *pSubnet, long Distance)
{
	m_CriticalSection.Enter();
	LOGICAL bChanged = pSubnet->SetDistance(Distance);
	m_CriticalSection.Leave();
	return bChanged;
}

CNetInterface *CNetwork::FindInterface(IPADDRESS_TYPE uIpAddress, CAdapter *pAdapterToExclude/* = NULL*/)
{
	for (int iAdapter = 0; iAdapter < m_nAdapters; iAdapter++)
	{
		CAdapter *pAdapter = m_pAdapters+iAdapter;
		if (pAdapter == pAdapterToExclude)
			continue;
		CNetInterface *pIface = pAdapter->FindInterface(uIpAddress);
		if (pIface)
			return pIface;
	}
	return NULL;
}

CAdapter *CNetwork::FindAdapter(const char *pszMacAddress) const
{
	for (int iAdapter = 0; iAdapter < m_nAdapters; iAdapter++)
	{
		CAdapter *pAdapter = m_pAdapters+iAdapter;
		if (!pAdapter)
			continue;
		if (!strcmp(pAdapter->GetMacAddress(), pszMacAddress))
			return pAdapter;
	}
	return NULL;
}

void CNetwork::ReadDatabase()
{
	if (!g_pDB)
		return; // DB is not opened - skip any DB stuff

	// Groups
	SGroupDesc *pGroups = NULL;
	int nGroups = 0;
	LOGICAL bSuccess = g_pDB->QueryGroups(pGroups, nGroups);
	for (int i = 0; i < nGroups; i++)
	{
		SGroupDesc *pGroupDesc = pGroups + i;
		TRACE("Group [%s] from database\n", pGroupDesc->szName);
		switch (pGroupDesc->Type)
		{
		case PING_GROUP_INDEX:
			g_pPingDefaultGroup->NewGroup(pGroupDesc);
			break;
		case APPEARANCE_GROUP_INDEX:
			g_pAppearanceDefaultGroup->NewGroup(pGroupDesc);
			break;
		case SNMP_GROUP_INDEX:
			g_pAppearanceDefaultGroup->NewGroup(pGroupDesc);
			break;
		}
	}
	g_pWholeNetwork->GroupChanged(g_pPingDefaultGroup);

	// Subnets
	SSubnetDesc *pSubnets = NULL;
	int nSubnets = 0;
	bSuccess = g_pDB->QuerySubnets(pSubnets, nSubnets);
	for (i = 0; i < nSubnets; i++)
	{
		SSubnetDesc *pSubnetDesc = pSubnets + i;
		if (pSubnetDesc->uIpAddress == 0)
			continue;
		CAdapter *pAdapter = FindAdapter(pSubnetDesc->szIfaceMacAddress);
		if (!pAdapter)
			continue;
		pAdapter->CreateSubnet(pSubnetDesc);
	}

	// Hosts
	SHostDesc *pHosts = NULL;
	int nHosts = 0;
	bSuccess = g_pDB->QueryHosts(pHosts, nHosts);
	for (i = 0; i < nHosts; i++)
	{
		SHostDesc *pHostDesc = pHosts + i;
		if (pHostDesc->uIpAddress == 0)
			continue;
		TRACE("Host [%s (%s)] from database\n", pHostDesc->szName, pHostDesc->szGivenName);
		CAdapter *pAdapter = FindAdapter(pHostDesc->szIfaceMacAddress);
		if (!pAdapter)
			continue;
		// check if the same host is created already but has another IP-address (because of DHCP, for instance)
		LOGICAL bMergedWithAnother = LOGICAL_FALSE;
		for (int j = 0; j < i; j++)
		{
			SHostDesc *pExistingDesc = pHosts + j;
			if (pExistingDesc->bSkipped)
				continue;
			if (pHostDesc->IsMergeable(pExistingDesc))
			{
				pExistingDesc->Merge(pHostDesc);
				g_pDB->MergeHosts(pExistingDesc, pHostDesc);
				pExistingDesc->pCreatedHost->Init(pExistingDesc);
				g_pDB->HostChanged(pExistingDesc->pCreatedHost);
				g_pDB->DeleteHost(pHostDesc->ID);
				pHostDesc->bSkipped = LOGICAL_TRUE;
				bMergedWithAnother = LOGICAL_TRUE;
			}
		}
		if (!bMergedWithAnother)
			pAdapter->CreateHost(pHostDesc);
	}
}


/////////////////////////////////////////////////////////////////////
// CNetObject schedule methods

// static
LOGICAL CNetObject::Scheduler_KeepAliveHost(CScheduleItem *pItem)
{
	return pItem->GetObject()->KeepAliveHost(pItem);
}

// static
LOGICAL CNetObject::Scheduler_SendSnmpRequest(CScheduleItem *pItem)
{
	return pItem->GetObject()->SendSnmpRequest(pItem);
}

// static
LOGICAL CNetObject::Scheduler_SubnetBackground(CScheduleItem *pItem)
{
	return pItem->GetObject()->SubnetBackground(pItem);
}

/////////////////////////////////////////////////////////////////////
// CNetObject class implementation

//Constuctors/destructors
CNetObject::CNetObject(IPADDRESS_TYPE IpAddress)
{
	Init(IpAddress);
}

CNetObject::CNetObject(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, long DatabaseID)
{
	Init(IpAddress);
	m_pAdapter = pAdapter;
	m_DatabaseID = DatabaseID;
}

void CNetObject::Init(IPADDRESS_TYPE IpAddress)
{
	m_szGivenName[0] = 0;
	m_uIpAddress = 0;
	SetIpAddress(IpAddress);

	m_szSubnetMask[0] = 0;
	m_uSubnetMask = 0;
	m_pAdapter = NULL;
	m_DatabaseID = NET_OBJECT_ID_UNKNOWN;
	m_Flags = 0;
	m_nDistanceInHops = DISTANCE_UNKNOWN;
}

//virtual
CNetObject::~CNetObject()
{
}

LOGICAL CNetObject::SetGivenName(const char *pszName)
{
	if (!strcmp(m_szGivenName, pszName))
		return LOGICAL_FALSE;
	strcpy(m_szGivenName, pszName);
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::SetIpAddress(IPADDRESS_TYPE uIpAddress)
{
	if (uIpAddress == m_uIpAddress)
		return LOGICAL_FALSE;
	m_uIpAddress = uIpAddress;
	AddressToString(m_szAddress, m_uIpAddress);
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::SetSubnetMask(IPADDRESS_TYPE uSubnetMask)
{
	if (uSubnetMask == m_uSubnetMask)
		return LOGICAL_FALSE;
	m_uSubnetMask = uSubnetMask;
	AddressToString(m_szSubnetMask, m_uSubnetMask);
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::SetFlags(long flags)
{
	if (m_Flags == flags)
		return LOGICAL_FALSE;
	m_Flags = flags;
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::SetDistance(long distance)
{
	LOGICAL bChanged = LOGICAL_FALSE;
	if (m_nDistanceInHops != distance)
		bChanged = LOGICAL_TRUE;
	m_nDistanceInHops = distance;
	return bChanged;
}

void CNetObject::Tracerted(SIpTraceRoute *pRoute)
{
	if (pRoute)
	{
		memcpy(&m_Route, pRoute, sizeof(m_Route));
		OnConnectedThroughFound();
	}
}

char *CNetObject::GetIfaceMacAddress()
{
	if (!m_pAdapter)
		return "";
	return m_pAdapter->GetMacAddress();
}

int CNetObject::CompareIpAddress(CNetObject *pObject)
{
	SOctetConverter This, That;
	This.addr = GetIpAddress();
	That.addr = pObject->GetIpAddress();

	if (This.bytes.b1 < That.bytes.b1)
		return -1;
	else if (This.bytes.b1 > That.bytes.b1)
		return +1;
	if (This.bytes.b2 < That.bytes.b2)
		return -1;
	else if (This.bytes.b2 > That.bytes.b2)
		return +1;
	if (This.bytes.b3 < That.bytes.b3)
		return -1;
	else if (This.bytes.b3 > That.bytes.b3)
		return +1;
	if (This.bytes.b4 < That.bytes.b4)
		return -1;
	else if (This.bytes.b4 > That.bytes.b4)
		return +1;

	return 0; //equal
}

LOGICAL CNetObject::BePseudo(LOGICAL bPseudo)
{
	if (IsPseudo() == bPseudo)
		return LOGICAL_FALSE; // flag is set already, no GUI update needed
	if (bPseudo)
		m_Flags |= PSEUDO_FLAG;
	else
		m_Flags &= ~PSEUDO_FLAG;
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::IsPseudo()
{
	if (m_Flags & PSEUDO_FLAG)
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

LOGICAL CNetObject::BeSelected(LOGICAL bSelected)
{
	if (IsSelected() == bSelected)
		return LOGICAL_FALSE; // flag is set already, no GUI update needed
	if (bSelected)
		m_Flags |= SELECTED_FLAG;
	else
		m_Flags &= ~SELECTED_FLAG;
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::IsSelected()
{
	if (m_Flags & SELECTED_FLAG)
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}


LOGICAL CNetObject::IsHidden()
{
	if (m_Flags & HIDDEN_FLAG)
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

LOGICAL CNetObject::BeHidden(LOGICAL bHidden)
{
	if (IsHidden() == bHidden)
		return LOGICAL_FALSE; // flag is set already, no GUI update needed
	if (bHidden)
		m_Flags |= HIDDEN_FLAG;
	else
		m_Flags &= ~HIDDEN_FLAG;
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::IsIgnored()
{
	if (m_Flags & IGNORED_FLAG)
		return LOGICAL_TRUE;
	if (IsTooFar())
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

LOGICAL CNetObject::BeIgnored(LOGICAL bIgnored)
{
	if (IsIgnored() == bIgnored)
		return LOGICAL_FALSE; // flag is set already, no GUI update needed
	if (bIgnored)
		m_Flags |= IGNORED_FLAG;
	else
		m_Flags &= ~IGNORED_FLAG;
	return LOGICAL_TRUE;
}

// Returns TRUE if GUI update needed (FALSE otherwise, for example, if it is marked as router's interface already so no any GUI update is needed).
LOGICAL CNetObject::BeHandMade()
{
	if (IsHandMade() == LOGICAL_TRUE)
		return LOGICAL_FALSE; // flag is set already, no GUI update needed
	m_Flags |= HANDMADE_FLAG;
	TRACE("NetObject %s is hand-made\n", GetIpAddressString());
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::IsHandMade() const
{
	if (m_Flags & HANDMADE_FLAG)
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

LOGICAL CNetObject::IsVisible()
{
	if (IsTooFar())
		return LOGICAL_FALSE;
	if (IsHidden() && !g_pGui->AreHiddenObjectsVisible())
		return LOGICAL_FALSE;
	if (IsIgnored() && !g_pGui->AreIgnoredObjectsVisible())
		return LOGICAL_FALSE;
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::IsTooFar()
{
	if (GetDistance() > g_pGui->GetMaxDistanceInHops())
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}


/////////////////////////////////////////////////////////////////////
// CListOfNetObjects class implementation

LOGICAL CListOfNetObjects::AddIfNotExists(CNetObject *pObject)
{
	for (iterator it = begin(); it != end(); it++)
	{
		CNetObject *pExisting = *it;
		if (pObject == pExisting)
			return LOGICAL_FALSE;  // exists already
	}
	push_back(pObject);
	return LOGICAL_TRUE;
}

void CListOfNetObjects::Remove(CNetObject *pObject)
{
	for (iterator it = begin(); it != end(); it++)
	{
		CNetObject *pExisting = *it;
		if (pObject == pExisting)
		{
			erase(it);
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////
// CListOfConnections class implementation

CListOfConnections::CListOfConnections()
{
}

/////////////////////////////////////////////////////////////////////
// CConnection class implementation

CConnection::CConnection(CHost *pHost1, CHost *pHost2)
{
	m_pHost1 = pHost1;
	m_pHost2 = pHost2;
	m_bVisible = LOGICAL_TRUE;
	m_fElasticity = 1.;
}

//virtual
CConnection::~CConnection()
{
}

CHost *CConnection::GetPeer(CHost *pThisHost)
{
	if (m_pHost1 ==pThisHost)
		return m_pHost2;

	return m_pHost1;
}

void CConnection::Used(CHost *pTarget)
{
	m_Hosts.AddIfNotExists(pTarget);
	m_fElasticity = m_Hosts.size();
}

/////////////////////////////////////////////////////////////////////
// CRouter class implementation

const double fSubnetExistance = 1;
const double fHostOutsideSubnet = 1;

//Constuctors/destructors
CRouter::CRouter(CHost *pHost)
{
	m_pHost = pHost;
	m_nCurrentAction = 0;
	m_pAddresses = NULL;
}

//virtual
CRouter::~CRouter()
{
	if (m_pAddresses)
		free(m_pAddresses);
}

LOGICAL CRouter::OrganizeHostsBehind()
{
	++m_nCurrentAction;
	m_nCurrentAction = m_nCurrentAction % 4;
	LOGICAL bSomethingChanged = LOGICAL_FALSE;
	
	switch (m_nCurrentAction)
	{
	case 0:
		bSomethingChanged = AddSubnet();
		break;
	case 1:
		bSomethingChanged = RemoveSubnet();
		break;
	case 2:
		bSomethingChanged = ChangeSubnetMask();
		break;
	case 3:
		bSomethingChanged = HostsToSubnets();
		break;
	default:
		break;
	}
	return bSomethingChanged;
}

int CompareIpAddressProc(const void *elem1, const void *elem2)
{
	IPADDRESS_TYPE *p1 = (IPADDRESS_TYPE *)elem1;
	IPADDRESS_TYPE *p2 = (IPADDRESS_TYPE *)elem2;
	return (*p1 - *p2);
}

LOGICAL CRouter::AddSubnet()
{
	Prepare();
	int nHosts = m_HostsOutsideSubnet.size();
	if (nHosts <= 1)
		return LOGICAL_FALSE;

	m_pAddresses = (IPADDRESS_TYPE *)realloc(m_pAddresses, nHosts*sizeof(IPADDRESS_TYPE));
	double MinEnergy = VERY_BIG;
	IPADDRESS_TYPE uBestAddress = 0;
	IPADDRESS_TYPE uBestMask = 0;
	LOGICAL bBestFound = LOGICAL_FALSE;
	for (int nMaskLen = 24-3; nMaskLen <= 24+3; nMaskLen++)
	{
		int nAddresses = 0;
		IPADDRESS_TYPE uMask = GetSubnetMaskByLength(nMaskLen);
		for (CListOfNetObjects::iterator it = m_HostsOutsideSubnet.begin(); it != m_HostsOutsideSubnet.end(); it++)
		{
			CHost *pHost = (CHost *)*it;
			IPADDRESS_TYPE uIpAddress = pHost->GetIpAddress() & uMask;
			m_pAddresses[nAddresses++] = uIpAddress;
		}
		::qsort(m_pAddresses, nAddresses, sizeof(IPADDRESS_TYPE), CompareIpAddressProc);
		int nMaxSeriesCount = 0, nSeriesCount = 1;
		IPADDRESS_TYPE uAddress;
		for (int i = 1; i < nAddresses; i++)
		{
			if (m_pAddresses[i] == m_pAddresses[i-1])
			{
				++nSeriesCount;
				if (nSeriesCount > nMaxSeriesCount)
				{
					nMaxSeriesCount = nSeriesCount;
					uAddress = m_pAddresses[i-1];
				}
			}
			else
			{
				nSeriesCount = 1;
			}
		}

		if (nMaxSeriesCount > 2)
		{
			double E = GetSubnetExistanceEnergy(nMaskLen);
			E -= nMaxSeriesCount*fHostOutsideSubnet;
			if (E < MinEnergy)
			{
				uBestAddress = uAddress;
				uBestMask = uMask;
				MinEnergy = E;
				bBestFound = LOGICAL_TRUE;
			}
		}
	}

	if (bBestFound)
	{
		CSubnet *pSubnet = m_pHost->m_pAdapter->CreateSubnet(uBestAddress, uBestMask);
		for (CListOfNetObjects::iterator it = m_HostsOutsideSubnet.begin(); it != m_HostsOutsideSubnet.end(); it++)
		{
			CHost *pHost = (CHost *)*it;
			if (pSubnet->IsMyAddress(pHost->GetIpAddress()))
				g_pWholeNetwork->AddHostToSubnet(pHost, pSubnet);
		}
		return LOGICAL_TRUE;
	}

	return LOGICAL_FALSE;
}

LOGICAL CRouter::RemoveSubnet()
{
	return LOGICAL_FALSE;
}

LOGICAL CRouter::ChangeSubnetMask()
{
	Prepare();
	LOGICAL bSomethingChanged = LOGICAL_FALSE;
	for (CListOfNetObjects::iterator it = m_Subnets.begin(); it != m_Subnets.end(); it++)
	{
		CSubnet *pSubnet = (CSubnet *)*it;
		if (pSubnet->IsHandMade())
			continue;
		// Try to make subnet mask longer
		if (pSubnet->CanShrink())
		{
			bSomethingChanged |= pSubnet->Shrink();
			g_pWholeNetwork->SubnetChanged(pSubnet);
		}
	}

	return bSomethingChanged;
}

LOGICAL CRouter::TryToAddHostToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (pHost->GetSubnet())
		return LOGICAL_FALSE;
	if (pSubnet->IsMyAddress(pHost->GetIpAddress()))
		return g_pWholeNetwork->AddHostToSubnet(pHost, pSubnet);
	return LOGICAL_FALSE;
}

LOGICAL CRouter::HostsToSubnets()
{
	LOGICAL bSomethingChanged = LOGICAL_FALSE;
	for (CListOfNetObjects::iterator SubnetIt = m_Subnets.begin(); SubnetIt != m_Subnets.end(); SubnetIt++)
	{
		CSubnet *pSubnet = (CSubnet *)*SubnetIt;
		CHost *pConnectedHost;
		CConnection *pConnection;
		for (CListOfConnections::iterator WanConnectionIt = m_pHost->m_WanConnections.begin(); WanConnectionIt != m_pHost->m_WanConnections.end(); WanConnectionIt++)
		{
			pConnection = *WanConnectionIt;
			pConnectedHost = pConnection->GetPeer(m_pHost);
			bSomethingChanged = TryToAddHostToSubnet(pConnectedHost, pSubnet);
		}
		for (CListOfConnections::iterator HWConnectionIt = m_pHost->m_HardwareConnections.begin(); HWConnectionIt != m_pHost->m_HardwareConnections.end(); HWConnectionIt++)
		{
			pConnection = *HWConnectionIt;
			pConnectedHost = pConnection->GetPeer(m_pHost);
			bSomethingChanged = TryToAddHostToSubnet(pConnectedHost, pSubnet);
			// Try to remove (hide) wan connections because of hardware connection to whole subnet
			if (pConnectedHost->GetSubnet() == pSubnet)
			{
				for (CListOfConnections::iterator WanConnectionIt2 = m_pHost->m_WanConnections.begin(); WanConnectionIt2 != m_pHost->m_WanConnections.end(); WanConnectionIt2++)
				{
					CConnection *pConnection2 = *WanConnectionIt2;
					CHost *pLanHost = pConnection2->GetPeer(m_pHost);
					if (pLanHost->GetSubnet() == pSubnet)
					{
						pConnection2->Hide();
						bSomethingChanged = LOGICAL_TRUE;
					}
				}
			}
		}
	}

	return bSomethingChanged;
}

double CRouter::Prepare()
{
	double E = 0;
	m_Subnets.resize(0);
	m_HostsOutsideSubnet.resize(0);
	for (CListOfConnections::iterator it = m_pHost->m_WanConnections.begin(); it != m_pHost->m_WanConnections.end(); it++)
	{
		CConnection *pConnection = *it;
		CHost *pConnectedHost = pConnection->GetPeer(m_pHost);
		CSubnet *pSubnet = pConnectedHost->GetSubnet();
		if (pSubnet)
		{
			m_Subnets.AddIfNotExists(pSubnet);
			E += GetSubnetExistanceEnergy(pSubnet);
		}
		else
		{
			m_HostsOutsideSubnet.push_back(pConnectedHost);
			E += fHostOutsideSubnet;
		}
	}
	return E;
}

double CRouter::GetSubnetExistanceEnergy(CSubnet *pSubnet)
{
	return GetSubnetExistanceEnergy(pSubnet->GetSubnetMask());
}

double CRouter::GetSubnetExistanceEnergy(IPADDRESS_TYPE uMask)
{
	return GetSubnetExistanceEnergy(Count1s(uMask));
}

double CRouter::GetSubnetExistanceEnergy(int nMaskLength)
{
	double E = fSubnetExistance;
	if (nMaskLength > 24)
		E += exp(nMaskLength-24.)*fSubnetExistance/2.;
	else if (nMaskLength < 24)
		E += exp(24. - nMaskLength)*fSubnetExistance;
	return E;
}

/////////////////////////////////////////////////////////////////////
// CHost class implementation

//Constuctors/destructors
CHost::CHost(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, long DatabaseID)
	: CNetObject(pAdapter, IpAddress, DatabaseID)
{
	m_bInitialized = LOGICAL_FALSE;
	Init();
}

CHost::CHost(CAdapter *pAdapter, SHostDesc *pDesc)
	: CNetObject(pAdapter, pDesc->uIpAddress, pDesc->ID)
{
	m_bInitialized = LOGICAL_FALSE;
	Init(pDesc);
}

void CHost::Init()
{
	if (m_bInitialized)
		return; // done already
	m_bInitialized = LOGICAL_TRUE;
	m_szOwnName[0] = 0;
	m_szNetBiosDomainName[0] = 0;
	m_szUserName[0] = 0;
	m_pSubnet = NULL; // unknown yet
	m_uLastTracertTime = 0;
	m_uServicesFlags = 0;
	m_bServicesFlagValid = FALSE;
	m_uAuxFlags = 0;
	m_pPreferredIface = NULL;
	m_szMacAddress[0] = 0;
	m_bMacAddressValid = LOGICAL_FALSE;
	m_pRouter = 0;

	m_pWorld = NULL;
	m_iCol = 0;
	m_iRow = 0;
	m_bPlaced = LOGICAL_FALSE;
	m_pNBuddy = m_pSBuddy = m_pWBuddy = m_pEBuddy = NULL;
	m_NameDisplayContext.bInitialized = m_IpAddressDisplayContext.bInitialized = LOGICAL_FALSE;
}

void CHost::Init(SHostDesc *pDesc)
{
	Init();
	SetDatabaseID(pDesc->ID);
	SetOwnName(pDesc->szName);
	SetNetBiosDomainName(pDesc->szNetBiosDomainName);
	SetGivenName(pDesc->szGivenName);
	SetFlags(pDesc->flags);
	SetDistance(pDesc->nDistanceInHops);
	SetMacAddress(pDesc->szOwnMacAddress);
	pDesc->pCreatedHost = this;
	g_pDB->LoadHostGrouping(this);
}

void CHost::InitDefaultGroups()
{
	memset(m_Groups, 0, sizeof(m_Groups));
	SetGroup(g_pPingDefaultGroup, LOGICAL_FALSE);
	SetGroup(g_pAppearanceDefaultGroup, LOGICAL_FALSE);
	SetGroup(g_pSnmpDefaultGroup, LOGICAL_FALSE);
}

void CHost::InitPeriodics()
{
	time_t CurrentTime; time(&CurrentTime);
	
	time_t t = CurrentTime;
	t -= RandomTimeShift(10);
	m_KeepAlivePeriodic.Set(t, GetKeepAlivePeriod());
	
	t = CurrentTime;
	t -= RandomTimeShift(10);
	m_NetbiosStatRequestPeriodic.Set(t, GetNetbiosStatReqPeriod());

	m_LastPacket.Set(0, 0);
}

//virtual
CHost::~CHost()
{
	if (m_pRouter)
		delete m_pRouter;
//	TRACE("Host %s destroyed\n", m_szAddress);
}

LOGICAL CHost::SetOwnName(const char *pszName)
{
	if (!strcmp(m_szOwnName, pszName))
		return LOGICAL_FALSE;
	strcpy(m_szOwnName, pszName);
	return LOGICAL_TRUE;
}

LOGICAL CHost::SetNetBiosDomainName(const char *pszName)
{
	if (!strcmp(m_szNetBiosDomainName, pszName))
		return LOGICAL_FALSE;
	strcpy(m_szNetBiosDomainName, pszName);
	return LOGICAL_TRUE;
}

LOGICAL CHost::SetUserName(const char *pszName)
{
	if (!strcmp(m_szUserName, pszName))
		return LOGICAL_FALSE;
	strcpy(m_szUserName, pszName);
	return LOGICAL_TRUE;
}

LOGICAL CHost::SetServicesFlags(unsigned long uServicesFlags)
{
	if (m_uServicesFlags == uServicesFlags)
		return LOGICAL_FALSE;
	m_uServicesFlags = uServicesFlags;
	m_bServicesFlagValid = LOGICAL_TRUE;
	return LOGICAL_TRUE;
}

LOGICAL CHost::SetMacAddress(char *pszMacAddress)
{
	if (!strcmp(m_szMacAddress, pszMacAddress))
		return LOGICAL_FALSE;
	strcpy(m_szMacAddress, pszMacAddress);
	m_bMacAddressValid = LOGICAL_TRUE;
	return LOGICAL_TRUE;
}

LOGICAL CHost::SetMacAddress(unsigned char *pMacAddress, int MacAddressLength)
{
	char szNewMac[32], szAdd[8];
	sprintf(szNewMac, "%02X", pMacAddress[0]);
	for (unsigned int i = 1; i < MacAddressLength; i++)
	{
		sprintf(szAdd, "-%02X", pMacAddress[i]);
		strcat(szNewMac, szAdd);
	}
	return SetMacAddress(szNewMac);
}

LOGICAL CHost::EqualMacAddresses(CHost *pHost)
{
	if (m_bMacAddressValid |= pHost->m_bMacAddressValid)
		return LOGICAL_FALSE;
	return (strcmp(GetMacAddress(), pHost->GetMacAddress()) == 0);
}

LOGICAL CHost::EqualOwnNames(CHost *pHost)
{
	if (!strlen(GetOwnName()) || !strlen(pHost->GetOwnName()))
		return LOGICAL_FALSE;
	return (strcmp(GetOwnName(), pHost->GetOwnName()) == 0);
}

LOGICAL CHost::EqualServices(CHost *pHost)
{
	if (m_bServicesFlagValid |= pHost->m_bServicesFlagValid)
		return LOGICAL_FALSE;
	return (GetServicesFlags() == pHost->GetServicesFlags());
}

LOGICAL CHost::CanMerge(CHost *pHost)
{
	return (EqualMacAddresses(pHost) && EqualOwnNames(pHost));
}

LOGICAL CHost::Merge(CHost *pHost)
{
	return LOGICAL_FALSE;
}

LOGICAL CHost::SetGroup(long GroupID, LOGICAL bNeedUpdate /* = LOGICAL_TRUE */)
{
	CGroup *pGroup = g_AllGroups.Find(GroupID);
	ASSERT(pGroup);
	if (!pGroup)
		return LOGICAL_FALSE;
	return SetGroup(pGroup, bNeedUpdate);
}

LOGICAL CHost::SetGroup(CGroup *pGroup, LOGICAL bNeedUpdate /* = LOGICAL_TRUE */)
{
	CGroup *pOldGroup = GetGroup(pGroup->GetType()); // group of the same type
	if (pGroup == pOldGroup)
		return LOGICAL_FALSE; // nothing to do

	if (pOldGroup)
		pOldGroup->ExcludeHost(this);
	pGroup->IncludeHost(this);
	m_Groups[pGroup->GetType()] = pGroup;

	if (bNeedUpdate)
		g_pWholeNetwork->HostMovedBetweenGroups(this, pOldGroup, pGroup);
	return LOGICAL_TRUE;
}

// Returns TRUE if GUI update needed (FALSE otherwise, for example, if it is marked as router's interface already so no any GUI update is needed).
LOGICAL CHost::BeRouter()
{
	if (IsRouter() == LOGICAL_TRUE)
		return LOGICAL_FALSE; // flag is set already, no GUI update needed
	m_Flags |= ROUTER_FLAG;
	TRACE("Host %s is router\n", GetIpAddressString());
	return LOGICAL_TRUE;
}

LOGICAL CHost::IsRouter() const
{
	if (m_Flags & ROUTER_FLAG)
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

// Returns TRUE if GUI update needed (FALSE otherwise, for example, if it is marked as router's interface already so no any GUI update is needed).
LOGICAL CHost::BeUnreach(LOGICAL bUnreach/*= TRUE*/)
{
	if (IsUnreach() == bUnreach)
		return LOGICAL_FALSE; // flag is set already, no GUI update needed
	if (bUnreach)
	{
		m_Flags |= UNREACH_FLAG;
		TRACE("Host %s is unreachable\n", GetIpAddressString());
	}
	else
	{
		m_Flags &= ~UNREACH_FLAG;
		TRACE("Host %s is reachable\n", GetIpAddressString());
	}
	if (!bUnreach)
		SendSubnetMaskRequest(); // update subnet mask as this host became reachable
	return LOGICAL_TRUE;
}

LOGICAL CHost::IsUnreach() const
{
	if (m_Flags & UNREACH_FLAG)
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

LOGICAL CHost::AnotherHostConnectedThrough(CHost *pHost)
{
	return BeRouter();
}

// modifies if necessary hosts by subnets
LOGICAL CHost::OrganizeHostsBehind()
{
	if (!IsRouter())
		return LOGICAL_FALSE;
	
	LOGICAL bUpdated = LOGICAL_FALSE;
	g_pWholeNetwork->TakeExclusiveRights();
	if (m_WanConnections.size() > 5)
	{
		if (!m_pRouter)
			m_pRouter = new CRouter(this);
		bUpdated = m_pRouter->OrganizeHostsBehind();
	}
	g_pWholeNetwork->ReleaseExclusiveRights();
	return bUpdated;
}

void CHost::IpPacketCaptured(SIpHeader *pIpHeader)
{
	m_LastPacket.RightNow(0);
	if (!m_pPreferredIface)
	{
		m_pPreferredIface = m_pAdapter->FindInterface(pIpHeader->destIP);
		if (m_pPreferredIface)
			SendSubnetMaskRequest();
	}
}

//inline
LOGICAL CHost::IsKeepAliveEnabled()
{
	return GetPingGroup()->IsKeepAliveEnabled();
}

//inline
time_t CHost::GetKeepAlivePeriod()
{
	return GetPingGroup()->GetKeepAlivePeriod();
}

void CHost::UpdateUdpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, SUdpHeader *pUdpHeader)
{
	m_UdpTrafficRecords.Record(this, pDestHost, IPPROTO_UDP, ntohs(pUdpHeader->DstPortNo), ntohs(pIpHeader->total_len));
}

void CHost::UpdateTcpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, STcpHeader *pTcpHeader)
{
	m_TcpTrafficRecords.Record(this, pDestHost, IPPROTO_TCP, ntohs(pTcpHeader->DstPortNo), ntohs(pIpHeader->total_len));
}

void CHost::UpdateIcmpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader)
{
	m_IcmpTrafficRecords.Record(this, pDestHost, IPPROTO_ICMP, pIcmpHeader->i_type, ntohs(pIpHeader->total_len));
}

void CHost::UpdateIgmpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader)
{
	m_IgmpTrafficRecords.Record(this, pDestHost, IPPROTO_IGMP, pIgmpHeader->type, ntohs(pIpHeader->total_len));
}

void CHost::UpdateTrafficDatabase()
{
	m_UdpTrafficRecords.UpdateDatabase();
	m_TcpTrafficRecords.UpdateDatabase();
	m_IcmpTrafficRecords.UpdateDatabase();
	m_IgmpTrafficRecords.UpdateDatabase();
}

LOGICAL CHost::AddressMaskReplyReceived(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader)
{
	m_uAuxFlags |= HAS_ADDRESS_MASK_REPLY_FLAG;
	unsigned long SubnetMask = pIcmpHeader->timestamp;
#ifdef _DEBUG
	char szMask[16], szFrom[16];
	AddressToString(szFrom, pIpHeader->sourceIP);
	AddressToString(szMask, SubnetMask);
	TRACE("AddressMask reply: %s, %s\n", szFrom, szMask);
#endif

	int nMaskLen = Count1s(SubnetMask);
	if (nMaskLen < 24)
		return LOGICAL_FALSE; // Masks like 255.0.0.0 received by mistake, do nothing

	unsigned long SubnetAddress = pIpHeader->sourceIP & SubnetMask;
	CSubnet *pSubnet = m_pAdapter->FindSubnet(SubnetAddress);
	if (!pSubnet)
		pSubnet = m_pAdapter->CreateSubnet(SubnetAddress, SubnetMask);

	g_pWholeNetwork->AddHostToSubnet(this, pSubnet);

	return LOGICAL_TRUE;
}

static void append_ascii_symbol(char *pszString, unsigned char cSymbol, int &nSymbols)
{
	unsigned int iSymbol = cSymbol;
	if (cSymbol != 0 && cSymbol != 0x20 && isascii(iSymbol))
	{
		pszString[nSymbols++] = cSymbol;
		pszString[nSymbols] = 0; // terminating zero
	}
}

LOGICAL CHost::NetBiosDgmReceived(SNameServiceHeader *pNsHeader, unsigned char *pCurrent)
{
	const unsigned short CONFLICT_FLAG = 1 << 11;
	const unsigned short GROUP_FLAG = 1 << 15;

	LOGICAL bChanged = LOGICAL_FALSE;
	
	char szName[255];
	szName[0] = 0;
	for (int iAnswer = 0; iAnswer < pNsHeader->GetAnswerCount(); iAnswer++)
	{
		pCurrent += RFC883_UnpackName(pCurrent, szName, sizeof(szName));
		
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

		if (Type == 0x20) // NB
		{
			// IP-address
			unsigned long *pIP = (unsigned long *)pCurrent;
			unsigned long IP = *pIP;
			pCurrent += 4;
			CHost *pHost = NULL;
			if (IP == m_uIpAddress)
				bChanged |= SetOwnName(szName);
			else
			{
				pHost = m_pAdapter->FindHost(IP);
				if (pHost && pHost->SetOwnName(szName))
					g_pWholeNetwork->HostChanged(pHost);
			}
		}
		else if (Type == 0x21) // NBSTAT
		{
			unsigned short *pRdLength = (unsigned short *)pCurrent;
			unsigned short RdLength = ntohs(*pRdLength);
			pCurrent += 2;
			unsigned char *pNumNames = pCurrent;
			unsigned int NumNames = *pNumNames;
			pCurrent++;
			unsigned long uServicesFlags = 0;
			for (int i = 0; i < NumNames; i++)
			{
				int nSymbols = 0;
				for (int j = 0; j < 15; j++)
				{
					append_ascii_symbol(szName, *pCurrent, nSymbols);
					pCurrent++;
				}
				// Store suffix
				unsigned char cSuffix = *pCurrent;
				pCurrent++;

				unsigned short *pNameFlags = (unsigned short *)pCurrent;
				unsigned short NameFlags = ntohs(*pNameFlags);
				pCurrent += 2;

				LOGICAL bGroupFlag = ((NameFlags & GROUP_FLAG) != 0);
				LOGICAL bConflict = ((NameFlags & CONFLICT_FLAG) != 0);
				// Analyze suffix
				switch (cSuffix)
				{
				case 0x00:
					if (bGroupFlag)
						bChanged |= SetNetBiosDomainName(szName);
					else
					{
						if (!strncmp(szName, "IS~", 3))
							uServicesFlags |= IIS_SERVICES_FLAG;
						else
						{
							bChanged |= SetOwnName(szName);
							uServicesFlags |= WORKSTATION_SERVICE_FLAG;
						}
					}
					break;
				case 0x01:
					if (bGroupFlag)
						uServicesFlags |= MASTER_BROWSER_FLAG; // __MSBROWSE__ as name, right?
					else
						uServicesFlags |= MESSENGER_SERVICE_FLAG;
					break;
				case 0x03:
					uServicesFlags |= MESSENGER_SERVICE_FLAG;
					bChanged |= SetUserName(szName);
					break;
				case 0x06:
					break;
				case 0x1B: break;
				case 0x1C: break;
				case 0x1D: break;
				case 0x1E: break;
				case 0x1F: break;
				case 0x20: break;
				case 0x21: break;
				case 0x22: break;
				case 0x23: break;
				case 0x24: break;
				case 0x30: break;
				case 0x31: break;
				case 0x33: break;
				case 0x43: break;
				case 0x44: break;
				case 0x45: break;
				case 0x46: break;
				case 0x4C: break;
				case 0x52: break;
				case 0x87: break;
				case 0x6A: break;
				case 0xBE: break;
				case 0xBF: break;
				case 0x2B: break;
				case 0x2F: break;
				default:
					append_ascii_symbol(szName, cSuffix, nSymbols); //ordinary, non-windows dgm
					bChanged |= SetOwnName(szName);
					break;
				}

			}
			bChanged |= SetMacAddress(pCurrent, 6);
			bChanged |= SetServicesFlags(uServicesFlags);
		}
	}
	return bChanged;
}

LOGICAL CHost::KeepAlive()
{
	if (IsKeepAliveEnabled() && !IsTooFar())
	{
		TRACE("Host %s/%s - keep alive\n", GetName(), GetIpAddressString());
		SendEchoRequest();
		m_KeepAlivePeriodic.RightNow(GetKeepAlivePeriod());
		return LOGICAL_TRUE;
	}
	return LOGICAL_FALSE;
}

void Concatenate(char *pszResult, char *pszAdd, int &nRemainingSymbols)
{
	int len = min(strlen(pszAdd), nRemainingSymbols);
	strncat(pszResult, pszAdd, len);
	nRemainingSymbols -= len;
}

void Concatenate(char *pszResult, time_t _time, int &nRemainingSymbols)
{
	char szAdd[128];
	strftime(szAdd, sizeof(szAdd), g_pLocale->GetShortDateAndTimeFormatStr(), localtime(&_time));
	Concatenate(pszResult, szAdd, nRemainingSymbols);
}

void CHost::GetKeepAliveStatusStr(char *szStatus, int MaxLen)
{
	char szDummy[32];
	
	szStatus[0] = 0;
	int nRemainingSymbols = MaxLen-1;

	char *pszNewStr = "Latest packet received at :";
	Concatenate(szStatus, pszNewStr, nRemainingSymbols);
	if (!nRemainingSymbols)
		return; // no empty space
	time_t last = m_LastPacket.GetLastTime();
	Concatenate(szStatus, last, nRemainingSymbols);
	Concatenate(szStatus, "\r\n", nRemainingSymbols);
	
	pszNewStr = "Latest keep-alive ping at : ";
	Concatenate(szStatus, pszNewStr, nRemainingSymbols);
	if (!nRemainingSymbols)
		return; // no empty space
	last = m_KeepAlivePeriodic.GetLastTime();
	Concatenate(szStatus, last, nRemainingSymbols);
	Concatenate(szStatus, "\r\n", nRemainingSymbols);

	pszNewStr = "Next keep-alive ping at : ";
	Concatenate(szStatus, pszNewStr, nRemainingSymbols);
	if (!nRemainingSymbols)
		return; // no empty space

	time_t next = m_KeepAlivePeriodic.GetNextTime();
	Concatenate(szStatus, next, nRemainingSymbols);
	if (!nRemainingSymbols)
		return; // no empty space
	sprintf(szDummy, " (in %d seconds)\r\n", m_KeepAlivePeriodic.GetTimeTillNext());
	Concatenate(szStatus, szDummy, nRemainingSymbols);
	if (!nRemainingSymbols)
		return; // no empty space
}

void CHost::ConnectionToHostUsed(CHost *pTarget, CHost *pRouter)
{
	CConnection *pConnection = FindWanConnection(pRouter);
	if (pConnection)
		pConnection->Used(pTarget);
}

LOGICAL CHost::NeedTracert()
{
//	if (m_pSubnet || m_Route.bReached)
//		return LOGICAL_FALSE;
	if (m_LastPacket.GetLastTime() == 0)
		return LOGICAL_TRUE; // not initialized yet
	unsigned long uCurrentTime = portableGetCurrentTimeMsec();
	return (uCurrentTime - m_uLastTracertTime > 180*1000);
}

SIpTraceRoute *CHost::Tracert()
{
	SendSubnetMaskRequest();
	SendNetBiosStatRequest();
	TRACE("Host %s/%s - tracert\n", GetName(), GetIpAddressString());
	unsigned long uCurrentTime = portableGetCurrentTimeMsec();
	SIpTraceRoute *pRoute = m_pAdapter->GetBestInterface(GetIpAddress())->TracertHost(this);
	m_uLastTracertTime = uCurrentTime - RandomTimeShift(1000);
	return pRoute;
}

LOGICAL CHost::SendEchoRequest()
{
	if (m_pPreferredIface)
	{
		CIcmpSocket *pPinger = m_pPreferredIface->GetPinger();
		if (pPinger)
		{
			if (pPinger->SendEchoRequest(GetIpAddress()))
			{
				return LOGICAL_TRUE;
			}
			else
			{
				if (pPinger->IsEC_Unreach())
					BeUnreach();
			}
		}
	}
	else
	{
		CIcmpSocket *pPinger = m_pAdapter->GetBestPinger(GetIpAddress());
		if (pPinger)
		{
			if (pPinger->SendEchoRequest(GetIpAddress()))
			{
				return LOGICAL_TRUE;
			}
			else
			{
				if (pPinger->IsEC_Unreach())
					BeUnreach();
			}
		}
	}
	return LOGICAL_FALSE;
}

LOGICAL CHost::SendSubnetMaskRequest()
{
	if (m_pPreferredIface)
	{
		CIcmpSocket *pPinger = m_pPreferredIface->GetPinger();
		if (pPinger)
		{
			if (pPinger->SendAddressMaskRequest(GetIpAddress()))
			{
				return LOGICAL_TRUE;
			}
			else
			{
				if (pPinger->IsEC_Unreach())
					BeUnreach();
			}
		}
	}
	return LOGICAL_FALSE;
}

LOGICAL CHost::SendNetBiosStatRequest()
{
	if (m_NetbiosStatRequestPeriodic.IsExpired())
	{
		if (m_pPreferredIface)
		{
			m_NetbiosStatRequestPeriodic.RightNow(GetNetbiosStatReqPeriod());
			return m_pPreferredIface->SendNetBiosStatRequest(GetIpAddress());
		}
	}
	return LOGICAL_FALSE;
}

void CHost::AddWanConnection(CConnection *pConnection)
{
	m_WanConnections.push_back(pConnection);
}

void CHost::AddHardwareConnection(CConnection *pConnection, LOGICAL bRecurse)
{
	if (bRecurse)
	{
		CHost *pPeer3 = pConnection->GetPeer(this);
		ASSERT(pPeer3);
		for (CListOfConnections::iterator it = m_HardwareConnections.begin(); it != m_HardwareConnections.end(); it++)
		{
			CConnection *pExistingConnection = *it;
			CHost *pPeer2 = pExistingConnection->GetPeer(this);
			if (pPeer3 == pPeer2)
				continue;
			g_pWholeNetwork->AddHardwareConnection(pPeer2, pPeer3, LOGICAL_FALSE);
		}
	}
	m_HardwareConnections.push_back(pConnection);
}

//virtual
void CHost::OnConnectedThroughFound()
{
	if (m_pSubnet)
	{

	}
	else
	{
		CHost *pConnectedThrough = m_pAdapter->FindHost(m_Route.HostAddresses[m_Route.nHops-1]);
		if (pConnectedThrough)
		{
			CSubnet *pSubnet = pConnectedThrough->m_pSubnet;
			if (pSubnet)
			{
				IPADDRESS_TYPE ThisSubnetAddress = m_uIpAddress & pSubnet->GetSubnetMask();
				IPADDRESS_TYPE ThatSubnetAddress = pSubnet->GetIpAddress() & pSubnet->GetSubnetMask();
				if (ThisSubnetAddress == ThatSubnetAddress)
				{
					g_pWholeNetwork->AddHostToSubnet(this, pSubnet);
				}
			}
		}
	}
}

LOGICAL CHost::Compare(SHostDesc *pDesc) const
{
	if (m_uIpAddress != pDesc->uIpAddress)
		return LOGICAL_FALSE;
	if (strcmp(m_pAdapter->GetMacAddress(), pDesc->szIfaceMacAddress))
		return LOGICAL_FALSE;

	return LOGICAL_TRUE;
}

LOGICAL CHost::EqualSubnets(CHost *pHost) const
{
	if (!GetSubnet() || !pHost)
		return LOGICAL_FALSE;
	return (GetSubnet() == pHost->GetSubnet());
}

double CHost::EnergyAt(int iCol, int iRow)
{
	double m = 1;
	if (GetSubnet())
		m *= 2; // host belonging to any subnet are heavier
	if (IsRouter())
		m *= 2; // router must tend to world center
	double E = m*m_pWorld->GravityAt(iCol, iRow);

	E += m_pWorld->ChemicalInteraction(this, m_pNBuddy);

	E += m_pWorld->ChemicalInteraction(this, m_pSBuddy);

	E += m_pWorld->ChemicalInteraction(this, m_pWBuddy);

	E += m_pWorld->ChemicalInteraction(this, m_pEBuddy);

	// Wan connections
	for (CListOfConnections::iterator WANiterator = m_WanConnections.begin(); WANiterator != m_WanConnections.end(); WANiterator++)
	{
		CConnection *pConnection = *WANiterator;
		if (!pConnection->IsVisible())
			continue; // hidden connection
		CHost *pConnectedHost = pConnection->GetPeer(this);
		if (!pConnectedHost->IsVisible())
			continue;
		int dx = iCol-pConnectedHost->GetColumn();
		int dy = iRow-pConnectedHost->GetRow();
		double distance2 = dx*dx+dy*dy;
		double elasticity = pConnection->GetElasticity();
		E += fWanElasticity * elasticity * distance2;
	}
	
	// Hardware connections
	for (CListOfConnections::iterator HWiterator = m_HardwareConnections.begin(); HWiterator != m_HardwareConnections.end(); HWiterator++)
	{
		CConnection *pConnection = *HWiterator;
		if (!pConnection->IsVisible())
			continue; // hidden connection
		CHost *pConnectedHost = pConnection->GetPeer(this);
		if (!pConnectedHost->IsVisible())
			continue;

		int dx = iCol-pConnectedHost->GetColumn();
		int dy = iRow-pConnectedHost->GetRow();
		double distance2 = dx*dx+dy*dy;
		E += fHardwareElasticity * distance2;
	}
	
	// Attraction to subnet
	CSubnet *pSubnet = GetSubnet();
	if (pSubnet)
		E += pSubnet->HostAttraction(this, iCol, iRow);
	
	return E;
}

CConnection *CHost::FindWanConnection(CHost *pHost)
{
	for (CListOfConnections::iterator it = m_WanConnections.begin(); it != m_WanConnections.end(); it++)
	{
		CConnection *pConnection = *it;
		if (pConnection->GetPeer(this) == pHost)
			return pConnection;
	}
	return NULL;
}

CConnection *CHost::FindHardwareConnection(CHost *pHost)
{
	for (CListOfConnections::iterator it = m_HardwareConnections.begin(); it != m_HardwareConnections.end(); it++)
	{
		CConnection *pConnection = *it;
		if (pConnection->GetPeer(this) == pHost)
			return pConnection;
	}
	return NULL;
}

//virtual
char *CHost::GetName()
{
	char *pszName = GetGivenName();
	if (pszName[0] == 0)
		pszName = GetOwnName();
	return pszName;
}

//virtual
void CHost::GetFullName(char *pszFullName)
{
	sprintf(pszFullName, "(%d, %d) Host %s %s", GetColumn(), GetRow(), GetName(), GetIpAddressString());
}

//virtual
NetObjectStatusEnum CHost::GetGeneralStatusCode()
{
	if (!IsKeepAliveEnabled() || IsUnreach() || (m_LastPacket.IsLater(m_KeepAlivePeriodic)))
		return STATUS_OK;

	time_t CurrentTime; time(&CurrentTime);
	if ((CurrentTime - m_LastPacket.GetLastTime()) > 3*GetKeepAlivePeriod())
		return STATUS_ALARM;
	else if ((CurrentTime - m_LastPacket.GetLastTime()) > 2*GetKeepAlivePeriod())
		return STATUS_WARNING;

	return STATUS_OK;
}

//virtual
LOGICAL CHost::GuiContextMenu(int ScreenX, int ScreenY)
{
	return g_pGui->HostContextMenu(ScreenX, ScreenY, this);
}

//virtual
LOGICAL CHost::KeepAliveHost(CScheduleItem *pItem)
{
	KeepAlive();
	pItem->MoveTargetTime(GetKeepAlivePeriod());
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CHost::SendSnmpRequest(CScheduleItem *pItem)
{
	TRACE("SnmpReq to host %s\n", GetIpAddressString());
	pItem->MoveTargetTime(60);
	return LOGICAL_TRUE;
}

LOGICAL CHost::HasBuddy()
{
	return (m_pNBuddy != NULL || m_pSBuddy != NULL || m_pEBuddy != NULL || m_pWBuddy != NULL);
}

CNetObject *CHost::GetNetObject(double x, double y)
{
	if (x < (m_iCol - 0.5)) return NULL;
	if (x > (m_iCol + 0.5)) return NULL;
	if (y < (m_iRow - 0.5)) return NULL;
	if (y > (m_iRow + 0.5)) return NULL;

	if (x > (m_iCol - fHostSize/2) && x < (m_iCol + fHostSize/2) &&
		y > (m_iRow - fHostSize/2) && y < (m_iRow + fHostSize/2))
		return this;

	return GetSubnet();
}

unsigned char *CHost::GetOrdinaryColor()
{
	return GetAppearanceGroup()->GetOrdinaryColor()->GetRGBAptr();
}

unsigned char *CHost::GetOrdinarySelectedColor()
{
	return GetAppearanceGroup()->GetOrdinarySelectedColor()->GetRGBAptr();
}

unsigned char *CHost::GetWarningColor()
{
	return GetAppearanceGroup()->GetWarningColor()->GetRGBAptr();
}

unsigned char *CHost::GetWarningSelectedColor()
{
	return GetAppearanceGroup()->GetWarningSelectedColor()->GetRGBAptr();
}

unsigned char *CHost::GetAlarmColor()
{
	return GetAppearanceGroup()->GetAlarmColor()->GetRGBAptr();
}

unsigned char *CHost::GetAlarmSelectedColor()
{
	return GetAppearanceGroup()->GetAlarmSelectedColor()->GetRGBAptr();
}

unsigned char *CHost::GetTextColor()
{
	return GetAppearanceGroup()->GetTextColor()->GetRGBAptr();
}

unsigned char *CHost::GetTextSelectedColor()
{
	return GetAppearanceGroup()->GetTextSelectedColor()->GetRGBAptr();
}

void CHost::Render(CWholeNetPlaneRenderer *pRenderer)
{
	if (!IsPlaced())
		return;

	pRenderer->StoreModelViewMatrix();
	CSubnet *pSubnet = GetSubnet();
	NetObjectStatusEnum status = GetGeneralStatusCode();

	if (IsIgnored() || IsHidden())
		glEnable(GL_BLEND);
	glTranslated(m_iCol, m_iRow, 0);
	glBegin(GL_QUADS);
		//Subnet rectangle
		if (pSubnet)
		{
			if (pSubnet->IsSelected())
				glColor4ubv(pSubnet->GetSelectedColor()->GetRGBAptr());
			else
				glColor4ubv(pSubnet->GetColor()->GetRGBAptr());

			glVertex2d(-fSubnetSize/2, -fSubnetSize/2);
			glVertex2d(-fSubnetSize/2, +fSubnetSize/2);
			glVertex2d(+fSubnetSize/2, +fSubnetSize/2);
			glVertex2d(+fSubnetSize/2, -fSubnetSize/2);

			LOGICAL bNorth, bSouth, bWest, bEast;
			// North
			CHost *pBuddy;
			if (m_pNBuddy && m_pNBuddy->GetSubnet() == pSubnet)
			{
				glVertex2d(-fSubnetSize/2, -fSubnetSize/2);
				glVertex2d(-fSubnetSize/2, +0.5);
				glVertex2d(+fSubnetSize/2, +0.5);
				glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				bNorth = LOGICAL_TRUE;
			}
			else
				bNorth = LOGICAL_FALSE;
			// South
			if (m_pSBuddy && m_pSBuddy->GetSubnet() == pSubnet)
			{
				glVertex2d(-fSubnetSize/2, -fSubnetSize/2);
				glVertex2d(-fSubnetSize/2, -0.5);
				glVertex2d(+fSubnetSize/2, -0.5);
				glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				bSouth = LOGICAL_TRUE;
			}
			else
				bSouth = LOGICAL_FALSE;
			// West
			if (m_pWBuddy && m_pWBuddy->GetSubnet() == pSubnet)
			{
				glVertex2d(-0.5, -fSubnetSize/2);
				glVertex2d(-0.5, +fSubnetSize/2);
				glVertex2d(+fSubnetSize/2, +fSubnetSize/2);
				glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				bWest = LOGICAL_TRUE;
			}
			else
				bWest = LOGICAL_FALSE;
			// East
			if (m_pEBuddy && m_pEBuddy->GetSubnet() == pSubnet)
			{
				glVertex2d(+0.5, -fSubnetSize/2);
				glVertex2d(+0.5, +fSubnetSize/2);
				glVertex2d(+fSubnetSize/2, +fSubnetSize/2);
				glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				bEast = LOGICAL_TRUE;
			}
			else
				bEast = LOGICAL_FALSE;
			// North-West
			if (bNorth && bWest)
			{
				pBuddy = m_pWorld->GetHostAt(m_iCol-1, m_iRow+1);
				if (pBuddy && pBuddy->GetSubnet() == pSubnet)
				{
					glVertex2d(-0.5, +fSubnetSize/2);
					glVertex2d(-0.5, +0.5);
					glVertex2d(-fSubnetSize/2, +0.5);
					glVertex2d(-fSubnetSize/2, +fSubnetSize/2);
				}
			}
			// North-East
			if (bNorth && bEast)
			{
				pBuddy = m_pWorld->GetHostAt(m_iCol+1, m_iRow+1);
				if (pBuddy && pBuddy->GetSubnet() == pSubnet)
				{
					glVertex2d(+0.5, +fSubnetSize/2);
					glVertex2d(+0.5, +0.5);
					glVertex2d(+fSubnetSize/2, +0.5);
					glVertex2d(+fSubnetSize/2, +fSubnetSize/2);
				}
			}
			// South-East
			if (bSouth && bEast)
			{
				pBuddy = m_pWorld->GetHostAt(m_iCol+1, m_iRow-1);
				if (pBuddy && pBuddy->GetSubnet() == pSubnet)
				{
					glVertex2d(+0.5, -fSubnetSize/2);
					glVertex2d(+0.5, -0.5);
					glVertex2d(+fSubnetSize/2, -0.5);
					glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				}
			}
			// South-West
			if (bSouth && bWest)
			{
				pBuddy = m_pWorld->GetHostAt(m_iCol-1, m_iRow-1);
				if (pBuddy && pBuddy->GetSubnet() == pSubnet)
				{
					glVertex2d(-0.5, -fSubnetSize/2);
					glVertex2d(-0.5, -0.5);
					glVertex2d(-fSubnetSize/2, -0.5);
					glVertex2d(-fSubnetSize/2, -fSubnetSize/2);
				}
			}
		}
		
		switch (status)
		{
		case STATUS_WARNING:
			if (IsSelected())
				glColor4ubv(GetWarningSelectedColor());
			else
				glColor4ubv(GetWarningColor());
			break;
		case STATUS_ALARM:
			if (IsSelected())
				glColor4ubv(GetAlarmSelectedColor());
			else
				glColor4ubv(GetAlarmColor());
			break;
		case STATUS_UNKNOWN:
		case STATUS_OK:
			if (IsSelected())
				glColor4ubv(GetOrdinarySelectedColor());
			else
				glColor4ubv(GetOrdinaryColor());
			break;
		default:
			break;
		}

		glVertex2d(-fHostSize/2, -fHostSize/2);
		glVertex2d(-fHostSize/2, +fHostSize/2);
		glVertex2d(+fHostSize/2, +fHostSize/2);
		glVertex2d(+fHostSize/2, -fHostSize/2);
	glEnd();

	if (IsSelected())
		glColor4ubv(GetTextSelectedColor());
	else
		glColor4ubv(GetTextColor());
	pRenderer->WriteHostIPAddress(GetIpAddressString(), m_IpAddressDisplayContext);
	if (GetGivenName()[0])
		pRenderer->WriteHostName(GetGivenName(), m_NameDisplayContext);
	else
		pRenderer->WriteHostName(GetName(), m_NameDisplayContext);
	

	glDisable(GL_BLEND);
	pRenderer->RestoreModelViewMatrix();
}
	
void CHost::SetLocation(int iCol, int iRow)
{
	ASSERT(IsVisible());
	m_iCol = iCol;
	m_iRow = iRow;
	m_bPlaced = LOGICAL_TRUE;
	// Northern buddy
	CHost *pBuddy = m_pWorld->GetHostAt(m_iCol, m_iRow+1);
	if (pBuddy)
	{
		pBuddy->m_pSBuddy = this;
		this->m_pNBuddy = pBuddy;
	}
	// Southern buddy
	pBuddy = m_pWorld->GetHostAt(m_iCol, m_iRow-1);
	if (pBuddy)
	{
		pBuddy->m_pNBuddy = this;
		this->m_pSBuddy = pBuddy;
	}
	// Western buddy
	pBuddy = m_pWorld->GetHostAt(m_iCol-1, m_iRow);
	if (pBuddy)
	{
		pBuddy->m_pEBuddy = this;
		this->m_pWBuddy = pBuddy;
	}
	// Eastern buddy
	pBuddy = m_pWorld->GetHostAt(m_iCol+1, m_iRow);
	if (pBuddy)
	{
		pBuddy->m_pWBuddy = this;
		this->m_pEBuddy = pBuddy;
	}
///	TRACE("Host %s placed at (%d, %d)\n", GetName(), m_iCol, m_iRow);
}

void CHost::Unplace()
{
	m_bPlaced = LOGICAL_FALSE;
	
	if (m_pNBuddy)
		m_pNBuddy->m_pSBuddy = NULL;
	if (m_pSBuddy)
		m_pSBuddy->m_pNBuddy = NULL;
	if (m_pWBuddy)
		m_pWBuddy->m_pEBuddy = NULL;
	if (m_pEBuddy)
		m_pEBuddy->m_pWBuddy = NULL;
	m_pNBuddy = m_pSBuddy = m_pWBuddy = m_pEBuddy = NULL;

//	TRACE("Host %s is unplaced at (%d, %d)\n", GetName(), m_iCol, m_iRow);
}

LOGICAL CHost::IsPlaneEqual(CHost *pHost)
{
	if (!EqualSubnets(pHost))
		return LOGICAL_FALSE;
	if (m_WanConnections.size() || pHost->m_WanConnections.size())
		return LOGICAL_FALSE;
	if (m_HardwareConnections.size() || pHost->m_HardwareConnections.size())
		return LOGICAL_FALSE;

	return LOGICAL_TRUE;
}


////////////////////////////////////////////////////////////////////
// CSubnet class implementation

//Constuctors/destructors
CSubnet::CSubnet(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, IPADDRESS_TYPE SubnetMask, long DatabaseID)
	: CNetObject(pAdapter, IpAddress, DatabaseID)
{
	Init(SubnetMask);
}

CSubnet::CSubnet(CAdapter *pAdapter, SSubnetDesc *pDesc)
	: CNetObject(pAdapter, pDesc->uIpAddress, pDesc->ID)
{
	Init(pDesc);
}

CSubnet::CSubnet(IPADDRESS_TYPE IpAddress, IPADDRESS_TYPE SubnetMask)
	: CNetObject(IpAddress)
{
	Init(SubnetMask);
}

CSubnet::CSubnet(IPADDRESS_TYPE IpAddress, int nMaskLength)
	: CNetObject(IpAddress)
{
	IPADDRESS_TYPE SubnetMask = GetSubnetMaskByLength(nMaskLength);
	Init(SubnetMask);
}

//virtual
CSubnet::~CSubnet()
{
	free(m_ppHosts);
}

void CSubnet::Init(IPADDRESS_TYPE uSubnetMask)
{
	SetSubnetMask(uSubnetMask);
	// Calculate how many hosts can be in this subnet
	m_nMaxHosts = CalcMaxPossibleHosts(uSubnetMask);
	m_ppHosts = (CHost **)calloc(m_nMaxHosts, sizeof(CHost *));

	m_uLastBroadcastPingTime = portableGetCurrentTimeMsec();
	m_uLastRouterSolicitationTime = m_uLastBroadcastPingTime;
	m_uLastHostDiscoveringTime = m_uLastBroadcastPingTime;
	m_iDiscoveredHost = 0;

	m_Color.Init(0,0,128); // blue
	m_TextColor.Init(0,128,128);//blue
	InitSelectedColors();

	m_BackgroundPeriodic.RightNow(5);
}

void CSubnet::Init(SSubnetDesc *pDesc)
{
	Init(pDesc->uSubnetMask);
	SetGivenName(pDesc->szGivenName);
	SetFlags(pDesc->flags);
	SetDistance(pDesc->nDistanceInHops);
	SetColors(pDesc->Color, pDesc->TextColor);
}

void CSubnet::AddHost(CHost *pHost)
{
	if (pHost->GetSubnet() == this)
		return; // in this subnet already
	if (!IsMyAddress(pHost->GetIpAddress()))
		return; // cannot be here
	PlaceHostInArray(pHost, m_ppHosts);
	pHost->m_pSubnet = this;
	pHost->SetSubnetMask(m_uSubnetMask);
	if (IsPseudo())
	{
		BePseudo(LOGICAL_FALSE);
		g_pWholeNetwork->NewSubnetCreated(this);
	}
	if (pHost->GetDistance() < GetDistance())
		g_pWholeNetwork->SetDistanceToSubnet(this, pHost->GetDistance()); // host is "closer" so update all subnet
	else
		g_pWholeNetwork->SetDistanceToHost(pHost, GetDistance()); // subnet is "closer" so update the host
}

void CSubnet::RemoveHost(CHost *pHost)
{
	if (pHost->GetSubnet() != this)
		return; // outside this subnet already
	UnplaceHostInArray(pHost, m_ppHosts);
	pHost->m_pSubnet = NULL;
	pHost->SetSubnetMask(0);
}

void CSubnet::PlaceHostInArray(CHost *pHost, CHost **ppHosts)
{
	if (!pHost)
		return;
	int nShift = ntohl(pHost->GetIpAddress() & (~GetSubnetMask()));
	ASSERT(ppHosts[nShift] == NULL || ppHosts[nShift] == pHost);
	ppHosts[nShift] = pHost;
}

void CSubnet::UnplaceHostInArray(CHost *pHost, CHost **ppHosts)
{
	if (!pHost)
		return;
	int nShift = ntohl(pHost->GetIpAddress() & (~GetSubnetMask()));
	ASSERT(ppHosts[nShift] == pHost);
	ppHosts[nShift] = NULL;
}

LOGICAL CSubnet::BroadcastPing(CIcmpSocket *pSocket)
{
	unsigned long uCurrentTime = portableGetCurrentTimeMsec();
	if (uCurrentTime - m_uLastBroadcastPingTime >= 60*1000)
	{
		TRACE("Subnet %s - broadcast ping...\n", GetIpAddressString());
		pSocket->SendEchoRequest(GetBroadcastAddress());
		m_uLastBroadcastPingTime = uCurrentTime - RandomTimeShift(1000);
		return LOGICAL_TRUE;
	}
	return LOGICAL_FALSE;
}

LOGICAL CSubnet::SendRouterSolicitation(CIcmpSocket *pSocket)
{
	unsigned long uCurrentTime = portableGetCurrentTimeMsec();
	if (uCurrentTime - m_uLastRouterSolicitationTime >= 60*1000)
	{
		TRACE("Subnet %s - router solicitation...\n", GetIpAddressString());
		pSocket->SendRouterSolicitation(INADDR_ANY_ROUTER);
		m_uLastRouterSolicitationTime = uCurrentTime - RandomTimeShift(5000);
	}
	return LOGICAL_FALSE;
}

LOGICAL CSubnet::DiscoverNextHost(CIcmpSocket *pSocket)
{
	unsigned long uCurrentTime = portableGetCurrentTimeMsec();
	if (uCurrentTime - m_uLastHostDiscoveringTime >= 5*1000)
	{
		if (m_iDiscoveredHost == 0)
			m_iDiscoveredHost = 1;
		CHost *pHost = m_ppHosts[m_iDiscoveredHost];
		if (pHost)
		{ // host exists
			//TRACE("Subnet %s - host %s exists\n", GetIpAddressString(), pHost->GetIpAddressString());
		}
		else
		{ // no host - discover
			IPADDRESS_TYPE IpAddress = m_uIpAddress | htonl(m_iDiscoveredHost);
#ifdef _DEBUG
			char szAddress[16]; AddressToString(szAddress, IpAddress);
			TRACE("Subnet %s - discovering %s...\n", GetIpAddressString(), szAddress);
#endif
			pSocket->SendEchoRequest(IpAddress);
		}
		m_uLastHostDiscoveringTime = uCurrentTime - RandomTimeShift(500);
		++m_iDiscoveredHost;
		m_iDiscoveredHost = m_iDiscoveredHost % (m_nMaxHosts-1);
		return LOGICAL_TRUE;
	}
	return LOGICAL_FALSE;
}

LOGICAL CSubnet::Compare(SSubnetDesc *pDesc)
{
	if (m_uIpAddress != pDesc->uIpAddress)
		return LOGICAL_FALSE;
	if (strcmp(m_pAdapter->GetMacAddress(), pDesc->szIfaceMacAddress))
		return LOGICAL_FALSE;

	return LOGICAL_TRUE;
}

LOGICAL CSubnet::IsMyAddress(IPADDRESS_TYPE uIpAddress)
{
	return ((uIpAddress & m_uSubnetMask) == (m_uIpAddress & m_uSubnetMask));
}

int CSubnet::GetMaskLength()
{
	return Count1s(GetSubnetMask());
}

LOGICAL CSubnet::CanShrink()
{
	if (GetMaskLength() >= 24)
		return LOGICAL_FALSE;
	
	CHost *pFirstHost = NULL;
	for (int i = 0; i < m_nMaxHosts; i++)
	{
		CHost *pHost = m_ppHosts[i];
		if (pHost)
		{
			pFirstHost = pHost;
			break;
		}
	}
	if (!pFirstHost)
		return LOGICAL_FALSE;
	int nMaskLen = GetMaskLength() + 1;
	IPADDRESS_TYPE ShrinkedMask = GetSubnetMaskByLength(nMaskLen);
	CSubnet ShrinkedSubnet(pFirstHost->GetIpAddress() & ShrinkedMask, ShrinkedMask);
	for (i = 0; i < m_nMaxHosts; i++)
	{
		CHost *pHost = m_ppHosts[i];
		if (pHost)
		{
			if (!ShrinkedSubnet.IsMyAddress(pHost->GetIpAddress()))
				return LOGICAL_FALSE;
		}
	}

	return LOGICAL_TRUE;
}

LOGICAL CSubnet::Shrink()
{
	m_uSubnetMask = GetSubnetMaskByLength(GetMaskLength()+1);
	int nMaxHostsNew = CalcMaxPossibleHosts(m_uSubnetMask);
	CHost **ppNewHosts = (CHost **)calloc(nMaxHostsNew, sizeof(CHost *));
	CHost *pAnyHost = NULL;
	for (int i = 0; i < m_nMaxHosts; i++)
	{
		CHost *pHost = m_ppHosts[i];
		if (pHost)
		{
			PlaceHostInArray(pHost, ppNewHosts);
			pHost->SetSubnetMask(GetSubnetMask());
			pAnyHost = pHost;
		}
	}
	ASSERT(pAnyHost);
	free(m_ppHosts);
	m_ppHosts = ppNewHosts;
	m_nMaxHosts = nMaxHostsNew;
	m_uIpAddress = pAnyHost->GetIpAddress() & GetSubnetMask();
	return LOGICAL_TRUE;
}

double CSubnet::HostAttraction(CHost *pHost, int iCol, int iRow)
{
	ASSERT(pHost->GetSubnet() == this);
	double E = 0;
	for (int i = 0; i < m_nMaxHosts; i++)
	{
		CHost *pSubnetHost = m_ppHosts[i];
		if (pSubnetHost != NULL && pSubnetHost != pHost)
		{
			ASSERT(pSubnetHost->GetSubnet() == this);
			E += fLanElasticity*(abs(pSubnetHost->GetColumn() - iCol)+abs(pSubnetHost->GetRow() - iRow));
		}
	}
	return E;
}

void CSubnet::InitSelectedColors()
{
	m_SelectedColor.InitAndHighlite(m_Color, 1.5);
	m_TextSelectedColor.InitAndHighlite(m_TextColor, 1.5);
}

void CSubnet::SetColors(SRGBA &Color, SRGBA &TextColor)
{
	m_Color.Init(Color);
	m_TextColor.Init(TextColor);
	InitSelectedColors();
}

char *CSubnet::GetColorSchemeDbString()
{
	sprintf(m_szColorScheme, "%lu;%lu", m_Color.GetRGBA(), m_TextColor.GetRGBA());
	return m_szColorScheme;
}

//virtual
void CSubnet::OnConnectedThroughFound()
{
	// refresh all children
	for (unsigned int i = 0; i < m_nMaxHosts; i++)
	{
		CHost *pHost = m_ppHosts[i];
		if (pHost)
			pHost->Tracerted(&m_Route);
	}
}

//virtual
LOGICAL CSubnet::SubnetBackground(CScheduleItem *pItem)
{
	TRACE("Subnet %s background\n", GetIpAddressString());
	CIcmpSocket *pPinger = m_pAdapter->GetBestPinger(GetIpAddress());
	//BroadcastPing(pPinger);
	//SendRouterSolicitation(pPinger);
	DiscoverNextHost(pPinger);
	pItem->MoveTargetTime(5);
	return LOGICAL_TRUE;
}

void CSubnet::DistanceChanged()
{
	for (int i = 0; i < m_nMaxHosts; i++)
	{
		CHost *pHost = m_ppHosts[i];
		if (!pHost)
			continue;
		if (m_nDistanceInHops == pHost->GetDistance())
			continue;
		g_pWholeNetwork->SetDistanceToHost(pHost, m_nDistanceInHops);
	}
}

//virtual
void CSubnet::GetFullName(char *pszFullName)
{
	sprintf(pszFullName, "Subnet %s %s/%s", GetGivenName(), GetIpAddressString(), GetSubnetMaskString());
}

//virtual
NetObjectStatusEnum CSubnet::GetGeneralStatusCode()
{
	return STATUS_OK;
}

//virtual
LOGICAL CSubnet::GuiContextMenu(int ScreenX, int ScreenY)
{
	return g_pGui->SubnetContextMenu(ScreenX, ScreenY, this);
}

