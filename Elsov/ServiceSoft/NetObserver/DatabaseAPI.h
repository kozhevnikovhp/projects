/* DatabaseAPI.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	25 Oct 2006 - initial creation
	03 Jul 2007 - traffic records queries
	28 Nov 2007 - own host's MAC-address
	14 Mar 2008 - groups table
	20 Mar 2008 - grouping (hosts by groups) loading/storing
	15 Apr 2008 - host merging (due to DHCP, for example)
	22 Jul 2008 - subnet color scheme
*/

#ifndef __DATABASE_API_H_INCLUDED__
#define __DATABASE_API_H_INCLUDED__

#include "Portable.h"

// Forward declarations
class CHost;
class CSubnet;
class CGroup;


const int MAX_NAME_LENGTH = 128;

// Database constants
const long NET_OBJECT_ID_UNKNOWN = -1;

// NetObject types
const long NET_OBJECT_TYPE_SUBNET = 1;
const long NET_OBJECT_TYPE_HOST = 2;
// NetObject properties
const long NET_OBJECT_PROPERTY_PARENT			= 1;
const long NET_OBJECT_PROPERTY_OWN_NAME			= 2;
const long NET_OBJECT_PROPERTY_GIVEN_NAME		= 3;
const long NET_OBJECT_PROPERTY_IP_ADDRESS		= 4;
const long NET_OBJECT_PROPERTY_SUBNET_MASK		= 5;
const long NET_OBJECT_PROPERTY_LAST_REPLY_TIME	= 6;
const long NET_OBJECT_PROPERTY_IFACE_MAC		= 7; // MAC-address of local iface connected through
const long NET_OBJECT_PROPERTY_FLAGS			= 8;
const long NET_OBJECT_PROPERTY_DISTANCE			= 9;
const long NET_OBJECT_PROPERTY_OWN_MAC			= 10; // Own MAC-address
const long NET_OBJECT_PROPERTY_NETBIOS_DOMAIN_NAME	= 11;
const long NET_OBJECT_PROPERTY_SERVICES_FLAGS	= 12;
const long NET_OBJECT_PROPERTY_SUBNET_COLOR_SCHEME	= 13;

const long DISTANCE_UNKNOWN = 0xFFFF;

typedef struct
{
	long			ID;		// Database identifier
	unsigned long	uIpAddress;
	unsigned long	uSubnetMask;
	char			szGivenName[MAX_NAME_LENGTH];
	char			szIfaceMacAddress[32]; // 6 bytes of MAC-address, each bytes are 2 bytes in text form, +1 byte for terminating zero
	long			flags;
	int				nDistanceInHops;
	LOGICAL			bSkipped;

	void InitNetObject()
	{
		ID = NET_OBJECT_ID_UNKNOWN;
		uIpAddress = uSubnetMask = 0;
		szGivenName[0] = 0;
		szIfaceMacAddress[0] = 0;
		flags = 0;
		nDistanceInHops = DISTANCE_UNKNOWN;
		bSkipped = LOGICAL_FALSE;
	}
} SNetObjectDesc;

typedef struct : public SNetObjectDesc
{
	void Init()
	{
		InitNetObject();
		Color.Init(0,0,128);
		TextColor.Init(0,128,128);
	}
	SRGBA Color, TextColor;
} SSubnetDesc;

typedef struct SHostDesc : public SNetObjectDesc
{
	char	szName[MAX_NAME_LENGTH];
	char	szNetBiosDomainName[MAX_NAME_LENGTH];
	char	szOwnMacAddress[32]; // 6 bytes of MAC-address, each bytes are 2 bytes in text form, +1 byte for terminating zero
	unsigned long ServicesFlags;
	CHost *pCreatedHost;
	
	void Init()
	{
		InitNetObject();
		szName[0] = szNetBiosDomainName[0] = 0;
		szOwnMacAddress[0] = 0;
		ServicesFlags = 0;
		pCreatedHost = NULL;
	}
	LOGICAL IsMergeable(SHostDesc *pAnotherDesc)
	{
		return (flags == pAnotherDesc->flags &&
			ServicesFlags == pAnotherDesc->ServicesFlags &&
			szIfaceMacAddress[0]!= 0	&& !strcmp(szIfaceMacAddress, pAnotherDesc->szIfaceMacAddress) &&
			szOwnMacAddress[0] != 0		&& !strcmp(szOwnMacAddress, pAnotherDesc->szOwnMacAddress) &&
			szName[0] != 0				&& !strcmp(szName, pAnotherDesc->szName) &&
			szNetBiosDomainName[0] != 0	&& !strcmp(szNetBiosDomainName, pAnotherDesc->szNetBiosDomainName));

	}
	void Merge(SHostDesc *pAnotherDesc)
	{
		if (pAnotherDesc->szGivenName[0])
		{
			strcat(szGivenName, " ");
			strcat(szGivenName, pAnotherDesc->szGivenName);
		}
		uIpAddress = pAnotherDesc->uIpAddress;
	}
} SHostDesc;

typedef struct
{
	CHost *pSrcHost, *pDstHost;
	time_t FromTime, ToTime;
	unsigned short proto;
	unsigned short port;
	long packets;
	double packets_per_second;
	long octets;
	double octets_per_second;
	LOGICAL bInUse;
} STrafficRecord;

typedef struct
{
	long ID;
	char szName[MAX_NAME_LENGTH];
	LOGICAL bDefault;
	long Type;
	char szParams[256];
} SGroupDesc;


class IDatabaseAPI
{
// Construction/destruction
public:

// Public interface
public:
	virtual LOGICAL Create() = 0;
	virtual void StoreTrafficRecord(STrafficRecord *pRecord) = 0;
	virtual void MergeHosts(SHostDesc *pDstDesc, SHostDesc *pSrcDesc) = 0;
	virtual void DeleteHost(unsigned long ID) = 0;
	// Queries
	virtual LOGICAL QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets) = 0;
	virtual LOGICAL QueryHosts(SHostDesc *&pHostDescs, int &nHosts) = 0;
	virtual SSubnetDesc *FindSubnet(CSubnet *pSubnet) = 0;
	virtual SHostDesc *FindHost(CHost *pHost) = 0;
	virtual LOGICAL QueryTrafficFromHost(CHost *pHost, unsigned short proto, time_t StartTime, time_t EndTime, STrafficRecord *&pRecords, int &nRecords) = 0;
	virtual LOGICAL QueryTrafficToHost(CHost *pHost, unsigned short proto, time_t StartTime, time_t EndTime, STrafficRecord *&pRecords, int &nRecords) = 0;
	virtual LOGICAL QueryGroups(SGroupDesc *&pGroups, int &nGroups) = 0;
	virtual LOGICAL LoadHostGrouping(CHost *pHost) = 0;

	// Events
	virtual void NewHostCreated(CHost *pHost) = 0;
	virtual void HostChanged(CHost *pHost) = 0;
	virtual void NewSubnetCreated(CSubnet *pSubnet) = 0;
	virtual void SubnetChanged(CSubnet *pSubnet) = 0;
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet) = 0;
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet) = 0;
	virtual void NewGroupCreated(CGroup *pGroup) = 0;
	virtual void GroupChanged(CGroup *pGroup) = 0;
	virtual void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup) = 0;

// Protected overridables
protected:
	// Create tables
	virtual LOGICAL CreateTables() = 0;
	virtual LOGICAL CreateObjectsTable() = 0;
	virtual LOGICAL CreateObjectPropertiesTable() = 0;
	virtual LOGICAL CreateEventLogTable() = 0;
	virtual LOGICAL CreateStringTable() = 0;
	virtual LOGICAL CreateTrafficTable() = 0;
	virtual LOGICAL CreateGroupsTable() = 0;
	virtual LOGICAL CreateGroupingTable() = 0;

};

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
	virtual void MergeHosts(SHostDesc *pDstDesc, SHostDesc *pSrcDesc) { }
	virtual void DeleteHost(unsigned long ID) { }
	// Queries
	virtual LOGICAL QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets) { pSubnetDescs = NULL; nSubnets = 0; return LOGICAL_TRUE; }
	virtual LOGICAL QueryHosts(SHostDesc *&pHostDescs, int &nHosts) { pHostDescs = NULL; nHosts = 0; return LOGICAL_TRUE; }
	virtual SSubnetDesc *FindSubnet(CSubnet *pSubnet) { return NULL; }
	virtual SHostDesc *FindHost(CHost *pHost) { return NULL; }
	virtual LOGICAL QueryTrafficFromHost(CHost *pHost, unsigned short proto, time_t StartTime, time_t EndTime, STrafficRecord *&pRecords, int &nRecords) { return LOGICAL_FALSE; }
	virtual LOGICAL QueryTrafficToHost(CHost *pHost, unsigned short proto, time_t StartTime, time_t EndTime, STrafficRecord *&pRecords, int &nRecords) { return LOGICAL_FALSE; }
	virtual LOGICAL QueryGroups(SGroupDesc *&pGroups, int &nGroups) { return LOGICAL_TRUE; }
	virtual LOGICAL LoadHostGrouping(CHost *pHost) { return LOGICAL_TRUE; }

	// Events
	virtual void NewHostCreated(CHost *pHost) { }
	virtual void HostChanged(CHost *pHost) { }
	virtual void NewSubnetCreated(CSubnet *pSubnet) { }
	virtual void SubnetChanged(CSubnet *pSubnet) { }
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet) { }
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet) { }
	virtual void NewGroupCreated(CGroup *pGroup) { }
	virtual void GroupChanged(CGroup *pGroup) { }
	virtual void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup) { }

// Protected overridables
protected:
	// Create tables
	virtual LOGICAL CreateTables() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateObjectsTable() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateObjectPropertiesTable() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateEventLogTable()  { return LOGICAL_TRUE; }
	virtual LOGICAL CreateStringTable() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateTrafficTable() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateGroupsTable() { return LOGICAL_TRUE; }
	virtual LOGICAL CreateGroupingTable() { return LOGICAL_TRUE; }

};

extern IDatabaseAPI *g_pDB; // global database, must be initialized at database creation

#endif //__DATABASE_API_H_INCLUDED__
