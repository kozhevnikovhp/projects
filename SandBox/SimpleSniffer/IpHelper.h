// File IpHelper.h - interfaces for IP-helper classes (wrappers for iphlpapi.dll)
// Created:	October 14, 2003
// Author:	Evgeny Kozhevnikov evgeny@kozhevnikov.org
//
// Revision history
// Oct 14, 2003 - initial creation
// Jan 20, 2004 - AddIpAddress() method added
// Aug 10, 2006 - ARP-cache
//
// Remarks:
// Include iphlpapi.lib library file in your build list

#ifndef __IP_HELPER_H_INCLUDED__
#define __IP_HELPER_H_INCLUDED__

#include <iphlpapi.h>
#include "IpSockets.h"

typedef struct
{
	int nAdapter;
	IPADDRESS_TYPE IpAddress;
	unsigned char MacAddress[8]; // max length of MAC-address is 8 bytes
	unsigned long MacAddressLength;
} SArpCacheRecord;

class CIpHelper
{
//Constructors/destructors
public:
	CIpHelper();
	virtual ~CIpHelper();

// Public methods
public:
// Ip-addresses
	unsigned int RequeryIpAddressTable();
	unsigned long GetIpAddressesCount(); // the number of IP-addresses currently available on this workstation (include so-called local)
	unsigned long GetIpAddress(unsigned long index);
	unsigned long GetIpSubnetMask(unsigned long index);
	BOOL IsIpAddressLocal(unsigned long index);
// Interfaces
	unsigned int RequeryIfacesTable();
	unsigned long GetIfacesCount(); // the number of interfaces currently available on this workstation
	unsigned long GetIfaceIpAddressIndex(unsigned long index); // returns index in interface table of IP-address specified by index in IpAddressesTable
	const char *GetIfaceDesc(unsigned long index);
	unsigned char *GetIfacePhysAddress(unsigned long index);
	unsigned long GetIfacePhysAddressLength(unsigned long index);
	unsigned long GetIfacePhysAddressStr(unsigned long index, char *pszBufferForAddress);
// IP-addresses and interfaces managing
	unsigned long AddAdapterIpAddress(unsigned long Iface, DWORD IpAddress, DWORD SubnetMask);
// Adapters
	unsigned int RequeryAdaptersTable();
	unsigned long GetAdaptersCount(); // the number of network adapters currently available on this workstation
	const char *GetAdapterDescription(unsigned int nAdapter);
	const char *GetAdapterName(unsigned int nAdapter);
// ARP-cache
	int RequeryArpCacheTable();
	int GetArpCacheRecordCount();
	BOOL GetArpCacheRecord(int index, SArpCacheRecord &ArpCacheRecord);
	void FlashArpCacheTable() { m_bArpCacheTableInitialized = FALSE; }

// Protected members
protected:

// Protected members
	PMIB_IPADDRTABLE m_pIpAddressesBuffer;
	BOOL m_bIpAddressesTableInitialized;
	PMIB_IFTABLE m_pIfacesTable;
	BOOL m_bIfacesTableInitialized;
	PIP_ADAPTER_INFO m_pAdaptersInfo;
	BOOL m_bAdaptersTableInitialized;
	PMIB_IPNETTABLE m_pArpCacheTable;
	BOOL m_bArpCacheTableInitialized;
};

#endif //__IP_HELPER_H_INCLUDED__
