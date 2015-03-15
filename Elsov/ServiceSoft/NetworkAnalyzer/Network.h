/* Network.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	15 Mar 2006 - initial creation
	30 Mar 2006 - skeleton for all possible ICMP packets
	03 Jul 2006 - RIP packets processing and subnets
	05 Jul 2006 - interface list from IpHelper, subnet masks added to interfaces
	10 Aug 2006 - ARP-cash
	21 Aug 2006 - tracert and keepalive
	22 Nov 2006 - storing network objects and their properties in database
	28 Nov 2006 - loading network objects and their properties from database
	29 Nov 2006 - storing/loading flags of network objects in/from database
	20 May 2007 - traffic table

*/

#ifndef __NETWORK_H_INCLUDED__
#define __NETWORK_H_INCLUDED__

#include "Portable.h"
#include "IcmpSockets.h"
#include "DatabaseAPI.h"
#include <vector>

//Misc
IPADDRESS_TYPE CalcBroadcastAddress(IPADDRESS_TYPE uAddress, IPADDRESS_TYPE uSubnetMask);

//Forward declarations
class IGuiAPI;
class CHost;
class CSubnet;
class CAdapter;
class CNetInterface;

typedef enum
{
	STATUS_OK,
	STATUS_WARNING,
	STATUS_ALARM,
	STATUS_UNKNOWN
} NetObjectStatusEnum;

// NetObject flags
const unsigned long	IGNORED_FLAG	= 1 << 0;
const unsigned long HIDDEN_FLAG		= 1 << 1;
const unsigned long ROUTER_FLAG		= 1 << 2;
const unsigned long UNREACH_FLAG	= 1 << 3;
const unsigned long PSEUDO_FLAG		= 1 << 4;
const unsigned long SELECTED_FLAG	= 1 << 5;


class CListOfTrafficRecords : public std::vector<STrafficRecord>
{
public:
	void Record(CHost *pSrcHost, CHost *pDstHost, unsigned short proto, unsigned short port, int octets);
	void UpdateDatabase(IDatabaseAPI *pDatabase);
};

// Misc
long RandomTimeShift(long Range);

// CDatabaseStub class - empty stub for IDatabase interface
// Used when no any real database chosen.
class CDatabaseStub : public IDatabaseAPI
{
// Construction/destruction
public:

// Public interface
public:
	virtual LOGICAL Create() { return LOGICAL_TRUE; }
	virtual void StoreTrafficRecord(STrafficRecord *pRecord) { }
	// Queries
	virtual LOGICAL QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets) { pSubnetDescs = NULL; nSubnets = 0; return LOGICAL_TRUE; }
	virtual LOGICAL QueryHosts(SHostDesc *&pHostDescs, int &nHosts) { pHostDescs = NULL; nHosts = 0; return LOGICAL_TRUE; }
	virtual SSubnetDesc *FindSubnet(CSubnet *pSubnet) { return NULL; }
	virtual SHostDesc *FindHost(CHost *pHost) { return NULL; }

	// Events
	virtual void NewHostCreated(CHost *pHost) { }
	virtual void HostChanged(CHost *pHost) { }
	virtual void NewSubnetCreated(CSubnet *pSubnet) { }
	virtual void SubnetChanged(CSubnet *pSubnet) { }
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet) { }
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet) { }

// Protected overridables
protected:
	// Create tables
	virtual LOGICAL CreateTables() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateObjectsTable() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateObjectPropertiesTable() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateEventLogTable()  { return LOGICAL_TRUE; }
	virtual LOGICAL CreateStringTable() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateTrafficTable() { return LOGICAL_TRUE; }

};

// CNetwork class - whole net
// 
class CNetwork
{
	friend class CAdapter;
//Constuctors/destructors
public:
	CNetwork(IGuiAPI *pGui,IDatabaseAPI *pDatabase);
	virtual ~CNetwork();

// Public methods
public:
	IGuiAPI *GetGUI() { return m_pGui; }
	IDatabaseAPI *GetDatabase() { return m_pDatabase; }

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	void Run();
	void NewHostCreated(CHost *pNewHost);
	void HostChanged(CHost *pHost);
	void NewSubnetCreated(CSubnet *pNewSubnet);
	void SubnetChanged(CSubnet *pSubnet);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);

	void ReadDatabase();

	CNetInterface *FindInterface(IPADDRESS_TYPE uIpAddress, CAdapter *pAdapterToExclude = NULL);
	CAdapter *FindAdapter(const char *pszMacAddress) const;
// Protected overridables
protected:

// Protected members
protected:
	CAdapter *m_pAdapters;
	int m_nAdapters;
	CDatabaseStub m_StubDB;
	IGuiAPI *m_pGui;
	IDatabaseAPI *m_pDatabase;
};


class CNetObject
{
	friend class CNetwork;
	friend class CAdapter;
	friend class CHost;
	friend class CSubnet;
//Constuctors/destructors
public:
	CNetObject(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, long DatabaseID);
	virtual ~CNetObject();

// Public methods
public:
	IPADDRESS_TYPE GetIpAddress() { return m_uIpAddress; }
	IPADDRESS_TYPE GetSubnetMask() { return m_uSubnetMask; }
	IPADDRESS_TYPE GetBroadcastAddress() { return CalcBroadcastAddress(m_uIpAddress, m_uSubnetMask); }
	char *GetIpAddressString() { return m_szAddress; }
	char *GetSubnetMaskString() { return m_szSubnetMask; }
	char *GetName() { return m_szName; }
	char *GetGivenName() { return m_szGivenName; }
	long GetDatabaseID() { return m_DatabaseID; }
	void SetDatabaseID(long ID) { m_DatabaseID = ID; }
	char *GetIfaceMacAddress();
	long GetFlags() { return m_Flags; }
	int GetDistance() { return m_nDistanceInHops; }
	int CompareIpAddress(CNetObject *pObject);
	LOGICAL BePseudo(LOGICAL bPseudo);
	LOGICAL IsPseudo();
	LOGICAL BeSelected(LOGICAL bSelected);
	LOGICAL IsSelected();
	LOGICAL SetName(const char *pszName);
	LOGICAL SetGivenName(const char *pszName);

// Public overridables
public:
	virtual void GetFullName(char *pszFullName) {}
	virtual NetObjectStatusEnum GetGeneralStatusCode() { return STATUS_UNKNOWN; }
	virtual LOGICAL GuiContextMenu(int ScreenX, int ScreenY) { return LOGICAL_FALSE; }

// Public members
public:

// Protected methods
protected:
	LOGICAL SetSubnetMask(IPADDRESS_TYPE uSubnetMask);
	LOGICAL SetFlags(long flags);
	LOGICAL SetDistance(long distance);
	void ConnectedThrough(CHost *pConnectedThrough);

// Protected overridables
protected:
	virtual void OnConnectedThroughFound() {}
	virtual void OnDistanceChanged() {}

// Protected members
protected:
	unsigned long m_uIpAddress;
	char m_szAddress[16];
	IPADDRESS_TYPE m_uSubnetMask;
	char m_szSubnetMask[16];
	CHost *m_pConnectedThrough; // last host in trace
	char m_szName[128];
	char m_szGivenName[128];
	CAdapter *m_pAdapter;
	long m_DatabaseID;			// ID in database
	long m_Flags;
	long m_nDistanceInHops;
	static IGuiAPI *m_pGui;
};


class CListOfHosts : public std::vector<CHost *>
{
public:
	void AddIfNotExists(CHost *pHost);
};

class CHost : public CNetObject
{
	friend class CSubnet;
	friend class CAdapter;
	friend class CNetInterface;
	friend class CNetwork;
//Constuctors/destructors
public:
	CHost(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, long DatabaseID);
	virtual ~CHost();

// Public methods
public:
	LOGICAL BeRouter(); // returns TRUE if GUI update needed (FALSE otherwise, for example, if it is marked as router's interface already so no any GUI update is needed).
	LOGICAL IsRouter();
	LOGICAL BeUnreach(LOGICAL bUnreach = TRUE); // returns TRUE if GUI update needed (FALSE otherwise, for example, if it is marked as router's interface already so no any GUI update is needed).
	LOGICAL IsUnreach();
	LOGICAL Compare(SHostDesc *pDesc);
	LOGICAL EqualSubnets(CHost *pHost);
	int GetWanConnectionCount() { return m_WanConnections.size(); }
	CHost *GetWanConnection(int i) { return m_WanConnections[i]; }
	int GetHardwareConnectionCount() { return m_HardwareConnections.size(); }
	CHost *GetHardwareConnection(int i) { return m_HardwareConnections[i]; }
	
	// Keep-alive stuff
	void EnableKeepAlive(int bEnable) { m_bKeepAliveEnabled = bEnable; }
	LOGICAL IsKeepAliveEnabled() { return m_bKeepAliveEnabled; }
	unsigned long GetKeepAlivePeriod() { return m_uKeepAlivePeriod; }
	void SetKeeplivePeriod(unsigned long uPeriod) { m_uKeepAlivePeriod = uPeriod; }

	CSubnet *GetSubnet() { return m_pSubnet; }

// Public overridables
public:
	virtual void GetFullName(char *pszFullName);
	virtual NetObjectStatusEnum GetGeneralStatusCode();
	virtual LOGICAL GuiContextMenu(int ScreenX, int ScreenY);

// Public members
public:

// Protected methods
protected:
	void IpPacketCaptured(SIpHeader *pIpHeader);
	void UpdateUdpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, SUdpHeader *pUdpHeader);
	void UpdateTcpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, STcpHeader *pUdpHeader);
	void UpdateIcmpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader);
	void UpdateIgmpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader);
	void UpdateTrafficDatabase(IDatabaseAPI *pDatabase);
	LOGICAL IsLastPacketTooOld();
	LOGICAL KeepAlive();
	LOGICAL NeedTracert();
	IPADDRESS_TYPE Tracert();
	LOGICAL SendEchoRequest();
	LOGICAL SendSubnetMaskRequest();
	LOGICAL AddWanConnection(CHost *pHost);
	LOGICAL AddHardwareConnection(CHost *pHost);

// Protected overridables
protected:
	virtual void OnConnectedThroughFound();
	virtual void OnDistanceChanged();

// Protected members
protected:
	CSubnet *m_pSubnet;
	LOGICAL m_bKeepAliveEnabled;
	unsigned long m_uLastPacketTime; // time of receiving the latest packet from this host
	unsigned long m_uLastKeepAliveTime; // time of sending the latest keep-alive request
	unsigned long m_uKeepAlivePeriod; // keep-alive period
	unsigned long m_uLastTracertTime;
	CListOfTrafficRecords m_UdpTrafficRecords;
	CListOfTrafficRecords m_TcpTrafficRecords;
	CListOfTrafficRecords m_IcmpTrafficRecords;
	CListOfTrafficRecords m_IgmpTrafficRecords;
	CListOfHosts m_WanConnections;
	CListOfHosts m_HardwareConnections;
	CNetInterface *m_pPreferredIface;
};


class CSubnet : public CNetObject
{
//Constuctors/destructors
public:
	CSubnet(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, IPADDRESS_TYPE SubnetMask, long DatabaseID);
	virtual ~CSubnet();

// Public methods
public:
	void AddHost(CHost *pHost);
	LOGICAL BroadcastPing(CIcmpSocket *pSocket);
	LOGICAL SendRouterSolicitation(CIcmpSocket *pSocket);
	LOGICAL DiscoverNextHost(CIcmpSocket *pSocket);
	LOGICAL Compare(SSubnetDesc *pDesc);
	LOGICAL IsMyAddress(IPADDRESS_TYPE uIpAddress);

// Public overridables
public:
	virtual void GetFullName(char *pszFullName);
	virtual NetObjectStatusEnum GetGeneralStatusCode();
	virtual LOGICAL GuiContextMenu(int ScreenX, int ScreenY);

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:
	virtual void OnConnectedThroughFound();
	virtual void OnDistanceChanged();

// Protected members
protected:
	CHost **m_ppHosts;
	unsigned long m_nMaxHosts;
	unsigned long m_uLastBroadcastPingTime;
	unsigned long m_uLastRouterSolicitationTime;
	unsigned long m_uLastHostDiscoveringTime;
	unsigned long m_iDiscoveredHost;
};

#endif //__NETWORK_H_INCLUDED__