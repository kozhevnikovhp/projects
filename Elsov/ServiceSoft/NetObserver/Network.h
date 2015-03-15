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

#ifndef __NETWORK_H_INCLUDED__
#define __NETWORK_H_INCLUDED__

#include "Portable.h"
#include "IcmpSockets.h"
#include "DatabaseAPI.h"
#include "NetBiosSocket.h"
#include <vector>

//Forward declarations
class IGuiAPI;
class CNetObject;
class CHost;
class CSubnet;
class CAdapter;
class CNetInterface;
class CNetwork;
class CPingGroup;
class CAppearanceGroup;
class CSnmpGroup;
class CWholeNetPlaneRenderer;
class CScheduleItem;

extern CNetwork *g_pWholeNetwork;
extern CPortableLocale *g_pLocale;

//Misc
IPADDRESS_TYPE CalcBroadcastAddress(IPADDRESS_TYPE uAddress, IPADDRESS_TYPE uSubnetMask);
IPADDRESS_TYPE CalcSubnetAddress(IPADDRESS_TYPE uAddress, IPADDRESS_TYPE uSubnetMask);

typedef enum
{
	STATUS_OK,
	STATUS_WARNING,
	STATUS_ALARM,
	STATUS_UNKNOWN
} NetObjectStatusEnum;

enum {
	DEFAULT_GROUP_INDEX,
	PING_GROUP_INDEX,
	APPEARANCE_GROUP_INDEX,
	SNMP_GROUP_INDEX,
	STOP_GROUP_INDEX
};

// NetObject flags
const unsigned long	IGNORED_FLAG	= 1 << 0;
const unsigned long HIDDEN_FLAG		= 1 << 1;
const unsigned long ROUTER_FLAG		= 1 << 2;
const unsigned long UNREACH_FLAG	= 1 << 3;
const unsigned long PSEUDO_FLAG		= 1 << 4;
const unsigned long SELECTED_FLAG	= 1 << 5;
const unsigned long HANDMADE_FLAG	= 1 << 6;

const unsigned long	HAS_ADDRESS_MASK_REPLY_FLAG	= 1 << 0;

// Microsoft-specific NetBIOS flags (from 16th character of NetBIOS name)
const unsigned long WORKSTATION_SERVICE_FLAG	= 1 << 0;
const unsigned long MESSENGER_SERVICE_FLAG		= 1 << 1;
const unsigned long MASTER_BROWSER_FLAG			= 1 << 2;
const unsigned long IIS_SERVICES_FLAG			= 1 << 3; // IIS (Internet Information Service)
const unsigned long RAS_SERVER_SERVICE_FLAG		= 1 << 4; // RAS (Remote Access Service)
const unsigned long RAS_CLIENT_SERVICE_FLAG		= 1 << 5; // RAS (Remote Access Service)
const unsigned long NET_DDE_SERVICE_FLAG		= 1 << 6; // NetDDE
const unsigned long FILE_SERVER_SERVICE_FLAG	= 1 << 7; // 

// Energy constants for plane view
const double NameSortPenalty = 0.05;
const double fWanElasticity = 1.0;
const double fLanElasticity = 0.1;
const double fHardwareElasticity = 10.0;


class CListOfTrafficRecords : public std::vector<STrafficRecord>
{
public:
	void Record(CHost *pSrcHost, CHost *pDstHost, unsigned short proto, unsigned short port, int octets);
	void UpdateDatabase();
};

class CPeriodic
{
//Constuctors/destructors
public:
	CPeriodic();
	virtual ~CPeriodic();

// Public methods
public:
	void Set(time_t LastTime, time_t DeltaTime) { m_LastTime = LastTime; m_NextTime = m_LastTime + DeltaTime; }
	time_t GetLastTime() { return m_LastTime; }
	time_t GetNextTime() { return m_NextTime; }
	time_t GetTimeTillNext();
	LOGICAL IsExpired();
	LOGICAL IsLater(CPeriodic &Another) { return m_LastTime > Another.m_LastTime; }

	void RightNow(time_t DeltaTime);

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	time_t m_LastTime, m_NextTime;
};

// Misc
long RandomTimeShift(long Range);

class CListOfNetObjects : public std::vector<CNetObject *>
{
public:
	LOGICAL AddIfNotExists(CNetObject *pObject);
	void Remove(CNetObject *pObject);
};

class CConnection
{
	friend class CHost;
//Constuctors/destructors
public:
	CConnection(CHost *pHost1, CHost *pHost2);
	virtual ~CConnection();

// Public methods
public:
	CHost *GetPeer(CHost *pThisHost);
	CHost *GetPeer1() { return m_pHost1; }
	CHost *GetPeer2() { return m_pHost2; }
	double GetElasticity() { return m_fElasticity; }
	void Used(CHost *pTarget);
	void Show() { m_bVisible = LOGICAL_TRUE; }
	void Hide() { m_bVisible = LOGICAL_FALSE; }
	LOGICAL IsVisible() { return m_bVisible; }

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	CHost *m_pHost1, *m_pHost2;
	LOGICAL m_bVisible;
	double m_fElasticity;
	CListOfNetObjects m_Hosts;
};

class CListOfConnections : public std::vector<CConnection *>
{
public:
	CListOfConnections();
protected:

// Protected members
};

typedef struct
{
	double ScaleX, ScaleY;
	double ShiftX, ShiftY;
	LOGICAL bInitialized;
} STextDisplayContext;

// CNetwork class - whole net
// 
class CNetwork
{
	friend class CAdapter;
	friend class CNetInterface;
	friend class CHost;
	friend class CSubnet;
//Constuctors/destructors
public:
	CNetwork();
	virtual ~CNetwork();

// Public methods
public:
	void TakeExclusiveRights(char *pszText = NULL);
	void ReleaseExclusiveRights(char *pszText = NULL);
	void IgnoreHost(CHost *pHost);
	void HideHost(CHost *pHost);
	void ShowIgnoredObjects(LOGICAL bShow);
	void ShowHiddenObjects(LOGICAL bShow);
	void MakeHostUnreach(CHost *pHost, LOGICAL bUnreach);
	void MakeHostRouter(CHost *pHost, LOGICAL bRouter);
	CListOfConnections *GetWanConnections() { return &m_WanConnections; }
	CListOfConnections *GetHardwareConnections() { return &m_HardwareConnections; }
	void Run();

	void NewHostCreated(CHost *pNewHost);
	void HostChanged(CHost *pHost);
	void NewSubnetCreated(CSubnet *pNewSubnet);
	void SubnetChanged(CSubnet *pSubnet);
	LOGICAL AddHostToSubnet(CHost *pHost, CSubnet *pSubnet);
	void RemoveHostFromSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostTracerted(CHost *pHost, SIpTraceRoute *pRoute);
	LOGICAL AddWanConnection(CHost *pHost1, CHost *pHost2);
	LOGICAL AddHardwareConnection(CHost *pHost1, CHost *pHost2, LOGICAL bRecurse);
	
	void NewGroupCreated(CGroup *pGroup);
	void GroupChanged(CGroup *pGroup);
	void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup);

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

	LOGICAL SetDistanceToHost(CHost *pHost, long Distance);
	LOGICAL SetDistanceToSubnet(CSubnet *pSubnet, long Distance);

	void ReadDatabase();

	CNetInterface *FindInterface(IPADDRESS_TYPE uIpAddress, CAdapter *pAdapterToExclude = NULL);
	CAdapter *FindAdapter(const char *pszMacAddress) const;
// Protected overridables
protected:

// Protected members
protected:
	CAdapter *m_pAdapters;
	int m_nAdapters;
	CListOfConnections m_WanConnections;
	CListOfConnections m_HardwareConnections;

	CPortableCriticalSection m_CriticalSection;
};


class CNetObject
{
	friend class CNetwork;
//Constuctors/destructors
public:
	CNetObject(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, long DatabaseID);
	CNetObject(IPADDRESS_TYPE IpAddress);
	virtual ~CNetObject();

// Public methods
public:
	IPADDRESS_TYPE GetIpAddress() { return m_uIpAddress; }
	IPADDRESS_TYPE GetSubnetMask() { return m_uSubnetMask; }
	IPADDRESS_TYPE GetBroadcastAddress() { return CalcBroadcastAddress(m_uIpAddress, m_uSubnetMask); }
	char *GetIpAddressString() { return m_szAddress; }
	char *GetSubnetMaskString() { return m_szSubnetMask; }
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
	LOGICAL SetGivenName(const char *pszName);
	LOGICAL IsHidden();
	LOGICAL BeHidden(LOGICAL bHidden);
	LOGICAL IsIgnored();
	LOGICAL BeIgnored(LOGICAL bIgnored);
	LOGICAL IsHandMade() const;
	LOGICAL BeHandMade(); // returns TRUE if GUI update needed (FALSE otherwise, for example, if it is marked as router's interface already so no any GUI update is needed).
	LOGICAL IsVisible();
	LOGICAL IsTooFar();
	
	// Schedule methods
	static LOGICAL Scheduler_KeepAliveHost(CScheduleItem *pItem);
	static LOGICAL Scheduler_SendSnmpRequest(CScheduleItem *pItem);
	static LOGICAL Scheduler_SubnetBackground(CScheduleItem *pItem);


// Public overridables
public:
	virtual void GetFullName(char *pszFullName) {}
	virtual char *GetName() { return m_szGivenName; }
	virtual NetObjectStatusEnum GetGeneralStatusCode() { return STATUS_UNKNOWN; }
	virtual LOGICAL GuiContextMenu(int ScreenX, int ScreenY) { return LOGICAL_FALSE; }

// Public members
public:

// Protected methods
protected:
	void Init(IPADDRESS_TYPE IpAddress);
	LOGICAL SetIpAddress(IPADDRESS_TYPE IpAddress);
	LOGICAL SetSubnetMask(IPADDRESS_TYPE uSubnetMask);
	LOGICAL SetFlags(long flags);
	LOGICAL SetDistance(long distance);
	void Tracerted(SIpTraceRoute *pRoute);

// Protected overridables
protected:
	virtual void OnConnectedThroughFound() {}
	// Schedule actions. All methods return TRUE if action must be repeated next time
	virtual LOGICAL KeepAliveHost(CScheduleItem *pItem) { return LOGICAL_FALSE; }
	virtual LOGICAL SendSnmpRequest(CScheduleItem *pItem) { return LOGICAL_FALSE; }
	virtual LOGICAL SubnetBackground(CScheduleItem *pItem) { return LOGICAL_FALSE; }


// Protected members
protected:
	unsigned long m_uIpAddress;
	char m_szAddress[16];
	IPADDRESS_TYPE m_uSubnetMask;
	char m_szSubnetMask[16];
	char m_szGivenName[128];
	long m_nDistanceInHops;
	CAdapter *m_pAdapter;
	long m_DatabaseID;			// ID in database
	long m_Flags;
	SIpTraceRoute m_Route;
};


class CRouter
{
	friend class CHost;
//Constuctors/destructors
public:
	CRouter(CHost *pHost);
	virtual ~CRouter();

// Public methods
public:

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	LOGICAL OrganizeHostsBehind();
	LOGICAL AddSubnet();
	LOGICAL RemoveSubnet();
	LOGICAL ChangeSubnetMask();
	LOGICAL HostsToSubnets();
	LOGICAL TryToAddHostToSubnet(CHost *pHost, CSubnet *pSubnet);
	double Prepare();
	double GetSubnetExistanceEnergy(CSubnet *pSubnet);
	double GetSubnetExistanceEnergy(IPADDRESS_TYPE uMask);
	double GetSubnetExistanceEnergy(int nMaskLength);

// Protected overridables
protected:

// Protected members
protected:
	CHost *m_pHost;
	int m_nCurrentAction;
	CListOfNetObjects m_Subnets;
	CListOfNetObjects m_HostsOutsideSubnet;
	IPADDRESS_TYPE *m_pAddresses; // aux array
};

class CHost : public CNetObject
{
	friend class CSubnet;
	friend class CNetwork;
	friend class CRouter;
//Constuctors/destructors
public:
	CHost(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, long DatabaseID);
	CHost(CAdapter *pAdapter, SHostDesc *pDesc);
	virtual ~CHost();

// Public methods
public:
	void Init();
	void Init(SHostDesc *pDesc);
	void InitDefaultGroups();
	void InitPeriodics();
	LOGICAL IsRouter() const;
	LOGICAL BeRouter(); // returns TRUE if GUI update needed (FALSE otherwise, for example, if it is marked as router's interface already so no any GUI update is needed).
	LOGICAL IsUnreach() const;
	LOGICAL BeUnreach(LOGICAL bUnreach = LOGICAL_TRUE); // returns TRUE if GUI update needed (FALSE otherwise, for example, if it is marked as router's interface already so no any GUI update is needed).
	LOGICAL HasAddressMaskReply() { return ((m_uAuxFlags | HAS_ADDRESS_MASK_REPLY_FLAG) != 0); }
	LOGICAL AnotherHostConnectedThrough(CHost *pHost);
	LOGICAL OrganizeHostsBehind(); // modifies if necessary hosts by subnets
	LOGICAL Compare(SHostDesc *pDesc) const;
	LOGICAL EqualSubnets(CHost *pHost) const;

	CConnection *FindWanConnection(CHost *pHost);
	void AddWanConnection(CConnection *pConnection);

	CConnection *FindHardwareConnection(CHost *pHost);
	void AddHardwareConnection(CConnection *pConnection, LOGICAL bRecurse);

	char *GetOwnName() { return m_szOwnName; }
	LOGICAL SetOwnName(const char *pszName);
	char *GetNetBiosDomainName() { return m_szNetBiosDomainName; }
	LOGICAL SetNetBiosDomainName(const char *pszName);
	char *GetUserName() { return m_szUserName; }
	LOGICAL SetUserName(const char *pszName);
	unsigned long GetServicesFlags() { return m_uServicesFlags; }
	LOGICAL SetServicesFlags(unsigned long uServicesFlags);
	char *GetMacAddress() { return m_szMacAddress; }
	LOGICAL SetMacAddress(char *pszMacAddress);
	LOGICAL SetMacAddress(unsigned char *pMacAddress, int MacAddressLength);

	// Comparison
	LOGICAL EqualMacAddresses(CHost *pHost);
	LOGICAL EqualOwnNames(CHost *pHost);
	LOGICAL EqualServices(CHost *pHost);

	// Merge
	LOGICAL CanMerge(CHost *pHost);
	LOGICAL Merge(CHost *pHost);

	// Host groups
	CGroup *GetGroup(long type) { return m_Groups[type]; }
	LOGICAL SetGroup(long GroupID, LOGICAL bNeedUpdate = LOGICAL_TRUE);
	LOGICAL SetGroup(CGroup *pGroup, LOGICAL bNeedUpdate = LOGICAL_TRUE);
	CPingGroup *GetPingGroup() { return (CPingGroup *)GetGroup(PING_GROUP_INDEX); }
	CAppearanceGroup *GetAppearanceGroup() { return (CAppearanceGroup *)GetGroup(APPEARANCE_GROUP_INDEX); }
	CSnmpGroup *GetSnmpGroup() { return (CSnmpGroup *)GetGroup(SNMP_GROUP_INDEX); }

	LOGICAL AddressMaskReplyReceived(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader);
	LOGICAL NetBiosDgmReceived(SNameServiceHeader *pNsHeader, unsigned char *pCurrent);
	
	LOGICAL KeepAlive();
	LOGICAL NeedTracert();
	SIpTraceRoute *Tracert();
	LOGICAL SendEchoRequest();
	LOGICAL SendSubnetMaskRequest();
	LOGICAL SendNetBiosStatRequest();
	void UpdateTrafficDatabase();
	void UpdateUdpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, SUdpHeader *pUdpHeader);
	void UpdateTcpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, STcpHeader *pUdpHeader);
	void UpdateIcmpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader);
	void UpdateIgmpTrafficFrom(CHost *pDestHost, SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader);
	void IpPacketCaptured(SIpHeader *pIpHeader);
	void GetKeepAliveStatusStr(char *szStatus, int MaxLen);

	void ConnectionToHostUsed(CHost *pTarget, CHost *pRouter);

	// Colors
	unsigned char *GetOrdinaryColor();
	unsigned char *GetOrdinarySelectedColor();
	unsigned char *GetWarningColor();
	unsigned char *GetWarningSelectedColor();
	unsigned char *GetAlarmColor();
	unsigned char *GetAlarmSelectedColor();
	unsigned char *GetTextColor();
	unsigned char *GetTextSelectedColor();

	// Keep-alive stuff
	inline LOGICAL IsKeepAliveEnabled();
	inline time_t GetKeepAlivePeriod();
	inline time_t GetLastKeepAliveTime() { return m_KeepAlivePeriodic.GetLastTime(); }

	// Netbios
	inline time_t GetNetbiosStatReqPeriod() { return 60; }

	CSubnet *GetSubnet() const { return m_pSubnet; }

	// PlaneHost interface
	void SetPlaneWorld(CWholeNetPlaneRenderer *pWorld) { m_pWorld = pWorld; }
	LOGICAL HasBuddy();
	int GetColumn() { return m_iCol; }
	int GetRow() { return m_iRow; }
	void ContextChanged() { m_IpAddressDisplayContext.bInitialized = m_NameDisplayContext.bInitialized = LOGICAL_FALSE; }
	void Render(CWholeNetPlaneRenderer *pRenderer);
	CNetObject *GetNetObject(double x, double y);
	void SetLocation(int iCol, int iRow);
	void Unplace();
	LOGICAL IsPlaced() { return m_bPlaced; }
	LOGICAL IsPlaneEqual(CHost *pHost);
	double EnergyAt(int iCol, int iRow);

// Public overridables
public:
	virtual void GetFullName(char *pszFullName);
	virtual char *GetName();
	virtual NetObjectStatusEnum GetGeneralStatusCode();
	virtual LOGICAL GuiContextMenu(int ScreenX, int ScreenY);

// Protected methods
protected:

// Protected overridables
protected:
	virtual void OnConnectedThroughFound();

	// Schedule actions. All methods return TRUE if action must be repeated next time
	virtual LOGICAL KeepAliveHost(CScheduleItem *pItem);
	virtual LOGICAL SendSnmpRequest(CScheduleItem *pItem);

// Protected members
protected:
	LOGICAL m_bInitialized;
	CNetInterface *m_pPreferredIface;
	CSubnet *m_pSubnet;
	char m_szOwnName[128];
	char m_szNetBiosDomainName[128];
	char m_szUserName[128];
	char m_szMacAddress[32];
	LOGICAL m_bMacAddressValid;
	CPeriodic m_LastPacket; // time of receiving the latest packet from this host
	CPeriodic m_KeepAlivePeriodic;
	CPeriodic m_NetbiosStatRequestPeriodic;
	unsigned long m_uLastTracertTime;
	unsigned long m_uServicesFlags;
	LOGICAL m_bServicesFlagValid;
	unsigned long m_uAuxFlags; // non-persistent flags
	CRouter *m_pRouter;

	// Traffic
	CListOfTrafficRecords m_UdpTrafficRecords;
	CListOfTrafficRecords m_TcpTrafficRecords;
	CListOfTrafficRecords m_IcmpTrafficRecords;
	CListOfTrafficRecords m_IgmpTrafficRecords;

	// Connections
	CListOfConnections m_WanConnections;
	CListOfConnections m_HardwareConnections;
	
	// Groups
	CGroup *m_Groups[STOP_GROUP_INDEX];
	
	int m_iCol, m_iRow;
	LOGICAL m_bPlaced;
	CHost *m_pNBuddy, *m_pSBuddy, *m_pWBuddy, *m_pEBuddy; // neighbours on N, S, W, E sides respectively
	CWholeNetPlaneRenderer *m_pWorld;
	STextDisplayContext m_IpAddressDisplayContext, m_NameDisplayContext;

};

class CSubnet : public CNetObject
{
	friend class CNetwork;
//Constuctors/destructors
public:
	CSubnet(CAdapter *pAdapter, IPADDRESS_TYPE IpAddress, IPADDRESS_TYPE SubnetMask, long DatabaseID);
	CSubnet(CAdapter *pAdapter, SSubnetDesc *pDesc);
	CSubnet(IPADDRESS_TYPE IpAddress, IPADDRESS_TYPE SubnetMask);
	CSubnet(IPADDRESS_TYPE IpAddress, int nMaskLength);
	virtual ~CSubnet();

// Public methods
public:
	void Init(IPADDRESS_TYPE uSubnetMask);
	void Init(SSubnetDesc *pDesc);
	LOGICAL BroadcastPing(CIcmpSocket *pSocket);
	LOGICAL SendRouterSolicitation(CIcmpSocket *pSocket);
	LOGICAL DiscoverNextHost(CIcmpSocket *pSocket);
	LOGICAL Compare(SSubnetDesc *pDesc);
	LOGICAL IsMyAddress(IPADDRESS_TYPE uIpAddress);
	int GetMaskLength();
	LOGICAL CanShrink();
	LOGICAL Shrink();
	double HostAttraction(CHost *pHost, int iCol, int iRow);
	void InitSelectedColors();
	void SetColors(SRGBA &Color, SRGBA &TextColor);
	inline time_t GetLastBackgroundTime() { return m_BackgroundPeriodic.GetLastTime(); }

	// Colors
	SRGBA *GetColor() { return &m_Color; }
	SRGBA *GetSelectedColor() { return &m_SelectedColor; };
	SRGBA *GetTextColor() { return &m_TextColor; }
	SRGBA *GetTextSelectedColor() { return &m_TextSelectedColor; }
	char *GetColorSchemeDbString();

	// Public overridables
public:
	virtual void GetFullName(char *pszFullName);
	virtual NetObjectStatusEnum GetGeneralStatusCode();
	virtual LOGICAL GuiContextMenu(int ScreenX, int ScreenY);

// Public members
public:

// Protected methods
protected:
	void AddHost(CHost *pHost);
	void RemoveHost(CHost *pHost);
	void DistanceChanged();
	void UnplaceHostInArray(CHost *pHost, CHost **ppHosts);
	void PlaceHostInArray(CHost *pHost, CHost **ppHosts);

// Protected overridables
protected:
	virtual void OnConnectedThroughFound();

	// Schedule actions. All methods return TRUE if action must be repeated next time
	virtual LOGICAL SubnetBackground(CScheduleItem *pItem);

// Protected members
protected:
	CHost **m_ppHosts;
	unsigned long m_nMaxHosts;
	unsigned long m_uLastBroadcastPingTime;
	unsigned long m_uLastRouterSolicitationTime;
	unsigned long m_uLastHostDiscoveringTime;
	CPeriodic m_BackgroundPeriodic;
	unsigned long m_iDiscoveredHost;
	SRGBA m_Color, m_SelectedColor;
	SRGBA m_TextColor, m_TextSelectedColor;
	char m_szColorScheme[32];
};

#endif //__NETWORK_H_INCLUDED__