/* DatabaseAPI.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	25 Oct 2006 - initial creation

*/

#ifndef __DATABASE_API_H_INCLUDED__
#define __DATABASE_API_H_INCLUDED__

#include "Portable.h"

// Forward declarations
class CHost;
class CSubnet;


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

const long DISTANCE_UNKNOWN = 0xFFFF;

// Forward declarations

typedef struct
{
	long			ID;		// Database identifier
	unsigned long	uIpAddress;
	unsigned long	uSubnetMask;
	char			szGivenName[MAX_NAME_LENGTH];
	char			szIfaceMacAddress[32]; // 6 bytes of MAC-address, each bytes are 2 bytes in text form, +1 byte for terminating zero
	long			flags;
	int		nDistanceInHops;		

	void InitNetObject()
	{
		ID = NET_OBJECT_ID_UNKNOWN;
		uIpAddress = uSubnetMask = 0;
		szGivenName[0] = 0;
		szIfaceMacAddress[0] = 0;
		flags = 0;
		nDistanceInHops = DISTANCE_UNKNOWN;
	}
} SNetObjectDesc;

typedef struct : public SNetObjectDesc
{
	void Init()
	{
		InitNetObject();
	}
} SSubnetDesc;

typedef struct : public SNetObjectDesc
{
	char	szName[MAX_NAME_LENGTH];
	
	void Init()
	{
		InitNetObject();
		szName[0] = 0;
	}
} SHostDesc;

typedef struct STrafficRecord
{
	CHost *pSrcHost, *pDstHost;
	time_t FromTime, ToTime;
	unsigned short proto;
	unsigned short port;
	long packets;
	long octets;
} STrafficRecord;


class IDatabaseAPI
{
// Construction/destruction
public:

// Public interface
public:
	virtual LOGICAL Create() = 0;
	virtual void StoreTrafficRecord(STrafficRecord *pRecord) = 0;
	// Queries
	virtual LOGICAL QuerySubnets(SSubnetDesc *&pSubnetDescs, int &nSubnets) = 0;
	virtual LOGICAL QueryHosts(SHostDesc *&pHostDescs, int &nHosts) = 0;
	virtual SSubnetDesc *FindSubnet(CSubnet *pSubnet) = 0;
	virtual SHostDesc *FindHost(CHost *pHost) = 0;

	// Events
	virtual void NewHostCreated(CHost *pHost) = 0;
	virtual void HostChanged(CHost *pHost) = 0;
	virtual void NewSubnetCreated(CSubnet *pSubnet) = 0;
	virtual void SubnetChanged(CSubnet *pSubnet) = 0;
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet) = 0;
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet) = 0;

// Protected overridables
protected:
	// Create tables
	virtual LOGICAL CreateTables() = 0;
	virtual LOGICAL CreateObjectsTable() = 0;
	virtual LOGICAL CreateObjectPropertiesTable() = 0;
	virtual LOGICAL CreateEventLogTable() = 0;
	virtual LOGICAL CreateStringTable() = 0;
	virtual LOGICAL CreateTrafficTable() = 0;

};

#endif //__DATABASE_API_H_INCLUDED__
