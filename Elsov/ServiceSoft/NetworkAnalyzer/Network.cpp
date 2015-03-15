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

*/

#include "stdafx.h"
#include "Network.h"
#include "GuiAPI.h"
#include "Adapter.h"
#include "NetInterface.h"
#include "IpHelper.h"


void CListOfTrafficRecords::Record(CHost *pSrcHost, CHost *pDstHost, unsigned short proto, unsigned short port, int octets)
{
	for (iterator it = begin(); it != end(); it++)
	{
		STrafficRecord *pRecord = it;
		if (pRecord->pDstHost == pDstHost &&
			pRecord->port == port)
		{
			pRecord->packets++;
			pRecord->octets += octets;
			return;
		}
	}
	STrafficRecord NewRecord;
	NewRecord.pSrcHost = pSrcHost;
	NewRecord.pDstHost = pDstHost;
	time(&NewRecord.FromTime);
	NewRecord.port = port;
	NewRecord.packets = 1;
	NewRecord.octets = octets;
	NewRecord.proto = proto;
	if (port == 0)
		port=port;//debug
	push_back(NewRecord);
}

void CListOfTrafficRecords::UpdateDatabase(IDatabaseAPI *pDatabase)
{
	time_t current_time;
	time(&current_time);
	for (iterator it = begin(); it != end(); it++)
	{
		STrafficRecord *pRecord = it;
		time_t delta = current_time-pRecord->FromTime;
		if (delta >= 60)
		{
			pRecord->ToTime = current_time;
			// store in db
			if (pRecord->octets != 0)
				pDatabase->StoreTrafficRecord(pRecord);
			// prepare record for next
			pRecord->FromTime = pRecord->ToTime;
			pRecord->octets = 0;
			pRecord->packets = 0;
		}
	}
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

/////////////////////////////////////////////////////////////////////
// CNetwork class implementation

//Constuctors/destructors
CNetwork::CNetwork(IGuiAPI *pGui,IDatabaseAPI *pDatabase)
{
	m_pGui = pGui;
	CNetObject::m_pGui = pGui;

	m_pDatabase = &m_StubDB;
	if (pDatabase)
		m_pDatabase = pDatabase;

	m_pAdapters = NULL;
	Run();
}

//virtual
CNetwork::~CNetwork()
{
	TRACE("CNetwork object destructed\n");
	if (m_pAdapters)
		delete[] m_pAdapters;
}

void CNetwork::Run()
{
	// List of network interfaces
/*	CIpSocket s;
	char szName[255];
	gethostname(szName, sizeof(szName));
	struct hostent *pHostent = gethostbyname(szName);
	for (char **p = pHostent->h_addr_list; *p != 0; p++)
	{
		unsigned long IpAddress = *(unsigned long *)*p;
		CAdapter *pNewAdapter = new CAdapter(this, IpAddress);
		m_Adapters.push_back(pNewAdapter);
		pNewAdapter->Run();
	}*/
	CIpHelper helper;
	int iAdapter;
	m_nAdapters = helper.GetAdaptersCount();
	m_pAdapters = new CAdapter[m_nAdapters];
	unsigned long nInterfaces = helper.GetIpAddressesCount();
	CListOfHosts InterfaceHosts;
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
		CHost *pHost = m_pAdapters[iAdapter].AddNewInterface(this, IpAddress, SubnetMask);
		pHost->SetDistance(1);
		for (CListOfHosts::iterator it = InterfaceHosts.begin(); it != InterfaceHosts.end(); it++)
		{
			CHost *pExistingHost = *it;
			pExistingHost->AddHardwareConnection(pHost);
			pHost->AddHardwareConnection(pExistingHost);
		}
		InterfaceHosts.AddIfNotExists(pHost);
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
	m_pGui->NewHostCreated(pNewHost);
	m_pDatabase->NewHostCreated(pNewHost);
}

void CNetwork::HostChanged(CHost *pHost)
{
	if (!pHost)
		return;
	m_pGui->HostChanged(pHost);
	m_pDatabase->HostChanged(pHost);
}

void CNetwork::NewSubnetCreated(CSubnet *pNewSubnet)
{
	if (!pNewSubnet)
		return;
	m_pGui->NewSubnetCreated(pNewSubnet);
	m_pDatabase->NewSubnetCreated(pNewSubnet);
}

void CNetwork::SubnetChanged(CSubnet *pSubnet)
{
	if (!pSubnet)
		return;
	m_pGui->SubnetChanged(pSubnet);
	m_pDatabase->SubnetChanged(pSubnet);
}

void CNetwork::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (!pSubnet || !pHost)
		return;
	m_pGui->HostAddedToSubnet(pHost, pSubnet);
}

void CNetwork::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (!pSubnet || !pHost)
		return;
	m_pGui->HostRemovedFromSubnet(pHost, pSubnet);
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
	if (!m_pDatabase)
		return; // DB is not opened - skip any DB stuff

	// Subnets
	SSubnetDesc *pSubnets = NULL;
	int nSubnets = 0;
	int bSuccess = m_pDatabase->QuerySubnets(pSubnets, nSubnets);
	for (int i = 0; i < nSubnets; i++)
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
	bSuccess = m_pDatabase->QueryHosts(pHosts, nHosts);
	for (i = 0; i < nHosts; i++)
	{
		SHostDesc *pHostDesc = pHosts + i;
		if (pHostDesc->uIpAddress == 0)
			continue;
		TRACE("Host [%s (%s)] from database\n", pHostDesc->szName, pHostDesc->szGivenName);
		CAdapter *pAdapter = FindAdapter(pHostDesc->szIfaceMacAddress);
		if (!pAdapter)
			continue;
		pAdapter->CreateHost(pHostDesc);
	}
}

/////////////////////////////////////////////////////////////////////
// CNetObject class implementation

//static
IGuiAPI *CNetObject::m_pGui = NULL;

//Constuctors/destructors
CNetObject::CNetObject(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, long DatabaseID)
{
	m_uIpAddress = IpAddress;
	m_szName[0] = 0;
	m_szGivenName[0] = 0;
	AddressToString(m_szAddress, m_uIpAddress);

	m_szSubnetMask[0] = 0;
	m_uSubnetMask = 0;
	m_pAdapter = pAdapter;
	m_pConnectedThrough = NULL;  // unknown yet
	m_DatabaseID = DatabaseID;
	m_Flags = 0;
	m_nDistanceInHops = DISTANCE_UNKNOWN;
}

//virtual
CNetObject::~CNetObject()
{
}

LOGICAL CNetObject::SetName(const char *pszName)
{
	if (!strcmp(m_szName, pszName))
		return LOGICAL_FALSE;
	strcpy(m_szName, pszName);
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::SetGivenName(const char *pszName)
{
	if (!strcmp(m_szGivenName, pszName))
		return LOGICAL_FALSE;
	strcpy(m_szGivenName, pszName);
	return LOGICAL_TRUE;
}

LOGICAL CNetObject::SetSubnetMask(IPADDRESS_TYPE uSubnetMask)
{
	if (uSubnetMask == m_uSubnetMask)
		return LOGICAL_FALSE;
	m_uSubnetMask = uSubnetMask;
	AddressToString(m_szSubnetMask, uSubnetMask);
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
	OnDistanceChanged();
	return bChanged;
}

void CNetObject::ConnectedThrough(CHost *pConnectedThrough)
{
	if (pConnectedThrough)
	{
		m_pConnectedThrough = pConnectedThrough;
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


void CListOfHosts::AddIfNotExists(CHost *pHost)
{
	for (iterator it = begin(); it != end(); it++)
	{
		CHost *pExistingHost = *it;
		if (pHost == pExistingHost)
			return;  // exists already
	}
	push_back(pHost);
}

/////////////////////////////////////////////////////////////////////
// CHost class implementation

//Constuctors/destructors
CHost::CHost(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, long DatabaseID)
	: CNetObject(pAdapter, IpAddress, DatabaseID)
{
	m_pSubnet = NULL; // unknown yet
	m_bKeepAliveEnabled = LOGICAL_TRUE; //enabled
	m_uKeepAlivePeriod = 60*1000; // 60 sec = 1 min
	m_uLastPacketTime = portableGetCurrentTimeMsec();
	m_uLastKeepAliveTime = 0;
	m_uLastTracertTime = 0;
	m_pPreferredIface = NULL;
//	TRACE("Host %s created\n", m_szAddress);
}

//virtual
CHost::~CHost()
{
//	TRACE("Host %s destroyed\n", m_szAddress);
}

// Returns TRUE if GUI update needed (FALSE otherwise, for example, if it is marked as router's interface already so no any GUI update is needed).
LOGICAL CHost::BeRouter()
{
	if (IsRouter() == LOGICAL_TRUE)
		return LOGICAL_FALSE; // flag is set already, no GUI update needed
	m_Flags |= ROUTER_FLAG;
	TRACE("Host %s is router\n", GetIpAddressString());
	m_pAdapter->HostChanged(this);
	return LOGICAL_TRUE;
}

LOGICAL CHost::IsRouter()
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
	m_pAdapter->HostChanged(this);
	return LOGICAL_TRUE;
}

LOGICAL CHost::IsUnreach()
{
	if (m_Flags & UNREACH_FLAG)
		return LOGICAL_TRUE;
	return LOGICAL_FALSE;
}

void CHost::IpPacketCaptured(SIpHeader *pIpHeader)
{
	m_uLastPacketTime = portableGetCurrentTimeMsec();
	if (!m_pPreferredIface)
	{
		m_pPreferredIface = m_pAdapter->FindInterface(pIpHeader->destIP);
		if (m_pPreferredIface)
			SendSubnetMaskRequest();
	}
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

void CHost::UpdateTrafficDatabase(IDatabaseAPI *pDatabase)
{
	m_UdpTrafficRecords.UpdateDatabase(pDatabase);
	m_TcpTrafficRecords.UpdateDatabase(pDatabase);
	m_IcmpTrafficRecords.UpdateDatabase(pDatabase);
	m_IgmpTrafficRecords.UpdateDatabase(pDatabase);
}

LOGICAL CHost::IsLastPacketTooOld()
{
	return ((portableGetCurrentTimeMsec() - m_uLastPacketTime) > m_uKeepAlivePeriod);
}

LOGICAL CHost::KeepAlive()
{
	if (IsKeepAliveEnabled())
	{
		unsigned long uCurrentTime = portableGetCurrentTimeMsec();
		// We will send keep-alive echo requests is the last received packet is too old
		// OR after doubled keep-alive period time is gone (just to update accessibility status
		// of the host because we can have packets from unreachable hosts)
		if (/*IsLastPacketTooOld() || */	(uCurrentTime - m_uLastKeepAliveTime) > m_uKeepAlivePeriod)
		{
			TRACE("Host %s/%s - keep alive\n", GetName(), GetIpAddressString());
			SendEchoRequest();
			m_uLastKeepAliveTime = uCurrentTime - RandomTimeShift(5000);
		}
		return LOGICAL_TRUE;
	}
	return LOGICAL_FALSE;
}

LOGICAL CHost::NeedTracert()
{
	if (m_pSubnet || m_pConnectedThrough)
		return LOGICAL_FALSE;
	if (IsLastPacketTooOld())
		return LOGICAL_FALSE; // tracert if alive only
	unsigned long uCurrentTime = portableGetCurrentTimeMsec();
	return (uCurrentTime - m_uLastTracertTime > 180*1000);
}

IPADDRESS_TYPE CHost::Tracert()
{
	SendSubnetMaskRequest();
	TRACE("Host %s/%s - tracert\n", GetName(), GetIpAddressString());
	unsigned long uCurrentTime = portableGetCurrentTimeMsec();
	IPADDRESS_TYPE uConnectedThroughIP = m_pAdapter->GetBestInterface(GetIpAddress())->TracertHost(this);
	m_uLastTracertTime = uCurrentTime - RandomTimeShift(1000);
	return uConnectedThroughIP;
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

LOGICAL CHost::AddWanConnection(CHost *pHost)
{
	m_WanConnections.AddIfNotExists(pHost);
	LOGICAL bUpdated = BeRouter();
	return bUpdated;
}

LOGICAL CHost::AddHardwareConnection(CHost *pHost)
{
	m_HardwareConnections.AddIfNotExists(pHost);
	return LOGICAL_TRUE;
}

//virtual
void CHost::OnConnectedThroughFound()
{
	if (m_pSubnet)
	{

	}
	else
	{
		CSubnet *pSubnet = m_pConnectedThrough->m_pSubnet;
		if (m_pConnectedThrough->m_pSubnet)
		{
			IPADDRESS_TYPE ThisSubnetAddress = m_uIpAddress & pSubnet->GetSubnetMask();
			IPADDRESS_TYPE ThatSubnetAddress = pSubnet->GetIpAddress() & pSubnet->GetSubnetMask();
			if (ThisSubnetAddress == ThatSubnetAddress)
			{
				pSubnet->AddHost(this);
			}
		}
	}
}

//virtual
void CHost::OnDistanceChanged()
{
	if (m_pSubnet)
	{
		if (m_pSubnet->GetDistance() != GetDistance())
		m_pSubnet->SetDistance(GetDistance());
	}
	m_pAdapter->HostChanged(this);
}

LOGICAL CHost::Compare(SHostDesc *pDesc)
{
	if (m_uIpAddress != pDesc->uIpAddress)
		return LOGICAL_FALSE;
	if (strcmp(m_pAdapter->GetMacAddress(), pDesc->szIfaceMacAddress))
		return LOGICAL_FALSE;

	return LOGICAL_TRUE;
}

LOGICAL CHost::EqualSubnets(CHost *pHost)
{
	if (!GetSubnet() || !pHost)
		return LOGICAL_FALSE;
	return (GetSubnet() == pHost->GetSubnet());
}

//virtual
void CHost::GetFullName(char *pszFullName)
{
	char *pszName = m_szGivenName;
	if (pszName[0] == 0)
		pszName = m_szName;
	sprintf(pszFullName, "Host %s %s", pszName, GetIpAddressString());
}

//virtual
NetObjectStatusEnum CHost::GetGeneralStatusCode()
{
	if (!IsKeepAliveEnabled() || IsUnreach() || (m_uLastPacketTime >= m_uLastKeepAliveTime))
		return STATUS_OK;

	unsigned long uKeepAliveTimeElapsed = portableGetCurrentTimeMsec() - m_uLastPacketTime;
	if (uKeepAliveTimeElapsed > 3*m_uKeepAlivePeriod)
		return STATUS_ALARM;
	else if (uKeepAliveTimeElapsed > 2*m_uKeepAlivePeriod)
		return STATUS_WARNING;

	return STATUS_OK;
}

//virtual
LOGICAL CHost::GuiContextMenu(int ScreenX, int ScreenY)
{
	return m_pGui->HostContextMenu(ScreenX, ScreenY, this);
}

////////////////////////////////////////////////////////////////////
// CSubnet class implementation

//Constuctors/destructors
CSubnet::CSubnet(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, IPADDRESS_TYPE SubnetMask, long DatabaseID)
	: CNetObject(pAdapter, IpAddress, DatabaseID)
{
	SetSubnetMask(SubnetMask);
	// Calculate how many hosts can be in this subnet
	m_nMaxHosts = 1;
	IPADDRESS_TYPE unity = 1;
	while ((m_nMaxHosts & ntohl(SubnetMask)) == 0)
	{
		m_nMaxHosts <<= 1;
	}
	m_ppHosts = (CHost **)calloc(m_nMaxHosts, sizeof(CHost *));

	m_uLastBroadcastPingTime = portableGetCurrentTimeMsec();
	m_uLastRouterSolicitationTime = m_uLastBroadcastPingTime;
	m_uLastHostDiscoveringTime = m_uLastBroadcastPingTime;
	m_iDiscoveredHost = 0;
}

//virtual
CSubnet::~CSubnet()
{
	free(m_ppHosts);
}

void CSubnet::AddHost(CHost *pHost)
{
	if (pHost->GetSubnet() == this)
		return; // in this subnet already
	if (!IsMyAddress(pHost->GetIpAddress()))
		return; // cannot be here
	int nShift = ntohl(pHost->GetIpAddress() & (~GetSubnetMask()));
	ASSERT(m_ppHosts[nShift] == NULL || m_ppHosts[nShift] == pHost);
	m_ppHosts[nShift] = pHost;
	pHost->m_pSubnet = this;
	pHost->SetSubnetMask(m_uSubnetMask);
	if (IsPseudo())
	{
		BePseudo(LOGICAL_FALSE);
		m_pAdapter->NewSubnetCreated(this);
	}
	if (pHost->GetDistance() < GetDistance())
		SetDistance(pHost->GetDistance()); // host is "closer" so update all subnet
	else
		pHost->SetDistance(GetDistance()); // subnet is "closer" so update the host

	m_pAdapter->HostAddedToSubnet(pHost, this);
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

//virtual
void CSubnet::OnConnectedThroughFound()
{
	// refresh all children
	for (unsigned int i = 0; i < m_nMaxHosts; i++)
	{
		CHost *pHost = m_ppHosts[i];
		if (pHost)
		{
			pHost->ConnectedThrough(m_pConnectedThrough);
		}
	}
}

//virtual
void CSubnet::OnDistanceChanged()
{
	for (int i = 0; i < m_nMaxHosts; i++)
	{
		CHost *pHost = m_ppHosts[i];
		if (!pHost)
			continue;
		if (m_nDistanceInHops == pHost->GetDistance())
			continue;
		pHost->SetDistance(m_nDistanceInHops);
	}
	m_pAdapter->SubnetChanged(this);
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
	return m_pGui->SubnetContextMenu(ScreenX, ScreenY, this);
}

