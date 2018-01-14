// File IpHelper.cpp - implementation of IP-helper classes (wrappers for iphlpapi.dll)
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

#include <stdio.h>
#include "IpHelper.h"

//#pragma comment(lib, "iphlpapi.lib")

IpHelper::IpHelper()
{
    m_pIpAddressesBuffer = NULL;
    m_bIpAddressesTableInitialized = FALSE;
    m_pIfacesTable = NULL;
    m_bIfacesTableInitialized = FALSE;
    m_pAdaptersInfo = NULL;
    m_bAdaptersTableInitialized = FALSE;
    m_pArpCacheTable = NULL;
    m_bArpCacheTableInitialized = FALSE;
}

//virtual
IpHelper::~IpHelper()
{
    free(m_pIpAddressesBuffer);
    free(m_pIfacesTable);
    free(m_pAdaptersInfo);
    free(m_pArpCacheTable);
}

unsigned int IpHelper::RequeryIpAddressTable()
{
    unsigned long BufferSize = 0;
    DWORD ErrorCode = ::GetIpAddrTable(NULL, &BufferSize, FALSE);
    m_pIpAddressesBuffer = (PMIB_IPADDRTABLE)realloc(m_pIpAddressesBuffer, BufferSize);
    ErrorCode = ::GetIpAddrTable(m_pIpAddressesBuffer, &BufferSize, FALSE);
    m_bIpAddressesTableInitialized = TRUE;
    return 1;
}

// GetIpAddressesCount
// Returns the number of IP-addresses currently available on this workstation (include so-called local)
unsigned long IpHelper::GetIpAddressesCount()
{
    if (!m_bIpAddressesTableInitialized)
        RequeryIpAddressTable();
    if (!m_bIpAddressesTableInitialized)
        return 0; //failed
    return m_pIpAddressesBuffer->dwNumEntries; //success
}

unsigned long IpHelper::GetIpAddress(unsigned long index)
{
    if (!m_bIpAddressesTableInitialized)
        RequeryIpAddressTable();
    if (!m_bIpAddressesTableInitialized)
        return 0; //failed
    if (index >= m_pIpAddressesBuffer->dwNumEntries)
        return 0; //too large number of IpAddress index
    return m_pIpAddressesBuffer->table[index].dwAddr;
}

unsigned long IpHelper::GetIpSubnetMask(unsigned long index)
{
    if (!m_bIpAddressesTableInitialized)
        RequeryIpAddressTable();
    if (!m_bIpAddressesTableInitialized)
        return 0; //initialization failed
    if (index >= m_pIpAddressesBuffer->dwNumEntries)
        return 0; //too large number of IpSubnetMask index
    return m_pIpAddressesBuffer->table[index].dwMask;
}

BOOL IpHelper::IsIpAddressLocal(unsigned long index)
{
    if (!m_bIpAddressesTableInitialized)
        RequeryIpAddressTable();
    if (!m_bIpAddressesTableInitialized)
        return 0; //failed
    if (index >= m_pIpAddressesBuffer->dwNumEntries)
        return FALSE; //too large number of index
    return (m_pIpAddressesBuffer->table[index].dwIndex == 1);
}

unsigned int IpHelper::RequeryIfacesTable()
{
    unsigned long BufferSize = 0;
    DWORD ErrorCode = ::GetIfTable(NULL, &BufferSize, FALSE);
    m_pIfacesTable = (PMIB_IFTABLE)realloc(m_pIfacesTable, BufferSize);
    ErrorCode = ::GetIfTable(m_pIfacesTable, &BufferSize, FALSE);
    m_bIfacesTableInitialized = TRUE;
    return 1;
}

// GetIfacesCount
// Returns the number of interfaces currently available on this workstation
unsigned long IpHelper::GetIfacesCount()
{
    if (!m_bIfacesTableInitialized)
        RequeryIfacesTable();
    if (!m_bIfacesTableInitialized)
        return 0; //initialization failed
    return m_pIfacesTable->dwNumEntries; //success
}

// returns index in interface table of IP-address specified by index in IpAddressesTable
unsigned long IpHelper::GetIfaceIpAddressIndex(unsigned long index)
{
    if (!m_bIfacesTableInitialized)
        RequeryIfacesTable();
    if (!m_bIfacesTableInitialized)
        return 0; //initialization failed
    if (!m_bIpAddressesTableInitialized)
        RequeryIpAddressTable();
    if (!m_bIpAddressesTableInitialized)
        return (unsigned long)-1; //initialization failed
    if (index >= m_pIpAddressesBuffer->dwNumEntries)
        return (unsigned long)-1; //too large number of index
    for (unsigned long i = 0; i < GetIfacesCount(); i++)
    {
        if (m_pIfacesTable->table[i].dwIndex == m_pIpAddressesBuffer->table[index].dwIndex)
            return i;
    }
    return (unsigned long)-1;
}

const char *IpHelper::GetIfaceDesc(unsigned long index)
{
    if (!m_bIfacesTableInitialized)
        RequeryIfacesTable();
    if (!m_bIfacesTableInitialized)
        return ""; //initialization failed
    if (index >= m_pIfacesTable->dwNumEntries)
        return ""; //too large number of index
    m_pIfacesTable->table[index].bDescr[m_pIfacesTable->table[index].dwDescrLen] = 0; // terminating zero
    return (const char *)m_pIfacesTable->table[index].bDescr;
}

unsigned char *IpHelper::GetIfacePhysAddress(unsigned long index)
{
    if (!m_bIfacesTableInitialized)
        RequeryIfacesTable();
    if (!m_bIfacesTableInitialized)
        return 0; //initialization failed
    if (index >= m_pIfacesTable->dwNumEntries)
        return 0; //too large number of index
    return m_pIfacesTable->table[index].bPhysAddr;
}

unsigned long IpHelper::GetIfacePhysAddressLength(unsigned long index)
{
    if (!m_bIfacesTableInitialized)
        RequeryIfacesTable();
    if (!m_bIfacesTableInitialized)
        return 0; //initialization failed
    if (index >= m_pIfacesTable->dwNumEntries)
        return 0; //too large number of index
    return m_pIfacesTable->table[index].dwPhysAddrLen;
}

unsigned long IpHelper::GetIfacePhysAddressStr(unsigned long index, char *pszBufferForAddress)
{
    static char pszAdd[4];
    pszBufferForAddress[0] = 0;
    if (!m_bIfacesTableInitialized)
        RequeryIfacesTable();
    if (!m_bIfacesTableInitialized)
        return 0; //initialization failed
    if (index >= m_pIfacesTable->dwNumEntries)
        return 0; //too large number of index
    if (m_pIfacesTable->table[index].dwPhysAddrLen == 0)
        return 0;
    sprintf(pszBufferForAddress, "%02X", m_pIfacesTable->table[index].bPhysAddr[0]);
    for (unsigned int i = 1; i < m_pIfacesTable->table[index].dwPhysAddrLen; i++)
    {
        sprintf(pszAdd, "-%02X", m_pIfacesTable->table[index].bPhysAddr[i]);
        strcat(pszBufferForAddress, pszAdd);
    }
    return strlen(pszBufferForAddress);
}

unsigned long IpHelper::AddAdapterIpAddress(unsigned long nAdapter, DWORD IpAddress, DWORD SubnetMask)
{
    // Determine adapter index
    if (!m_bAdaptersTableInitialized)
        RequeryAdaptersTable();
    if (!m_bAdaptersTableInitialized)
        return 0; //initialization failed
    if (nAdapter >= GetAdaptersCount())
        return 0; //too large number of index
    unsigned long Context, Instance;
    DWORD ErrorCode = ::AddIPAddress(IpAddress, SubnetMask, m_pAdaptersInfo[nAdapter].Index, &Context, &Instance);
    return 0;
}

// the number of network adapters currently available on this workstation
unsigned long IpHelper::GetAdaptersCount()
{
    if (!m_bAdaptersTableInitialized)
        RequeryAdaptersTable();
    if (!m_bAdaptersTableInitialized)
        return 0; //failed
    unsigned long nAdapters = 0;
    for (PIP_ADAPTER_INFO pAdapter = m_pAdaptersInfo; pAdapter; pAdapter = pAdapter->Next)
        ++nAdapters;
    return nAdapters;//success
}

unsigned int IpHelper::RequeryAdaptersTable()
{
    unsigned long BufferSize = 0;
    DWORD ErrorCode = ::GetAdaptersInfo(NULL, &BufferSize);
    m_pAdaptersInfo = (PIP_ADAPTER_INFO)realloc(m_pAdaptersInfo, BufferSize);
    ErrorCode = ::GetAdaptersInfo(m_pAdaptersInfo, &BufferSize);
    m_bAdaptersTableInitialized = TRUE;
    return 1;
}

const char *IpHelper::GetAdapterDescription(unsigned int nAdapter)
{
    if (!m_bAdaptersTableInitialized)
        RequeryAdaptersTable();
    if (!m_bAdaptersTableInitialized)
        return 0; //initialization failed
    if (nAdapter >= GetAdaptersCount())
        return "Unknown"; //too large number of index
    return m_pAdaptersInfo[nAdapter].Description;
}

const char *IpHelper::GetAdapterName(unsigned int nAdapter)
{
    if (!m_bAdaptersTableInitialized)
        RequeryAdaptersTable();
    if (!m_bAdaptersTableInitialized)
        return 0; //initialization failed
    if (nAdapter >= GetAdaptersCount())
        return "Unknown"; //too large number of index
    return m_pAdaptersInfo[nAdapter].AdapterName;
}

int IpHelper::RequeryArpCacheTable()
{
    m_bArpCacheTableInitialized = FALSE;
    unsigned long BufferSize = 0;
    DWORD ErrorCode = ::GetIpNetTable(NULL, &BufferSize, TRUE);
    if (ErrorCode != NO_ERROR)
        return 0;
    m_pArpCacheTable = (PMIB_IPNETTABLE)realloc(m_pArpCacheTable, BufferSize);
    ErrorCode = ::GetIpNetTable(m_pArpCacheTable, &BufferSize, TRUE);
    if (ErrorCode != NO_ERROR)
        return 0;
    m_bArpCacheTableInitialized = TRUE;
    RequeryAdaptersTable(); // adapters info will be used in GetArpCashRecord() method
    return 1;
}

int IpHelper::GetArpCacheRecordCount()
{
    if (!m_bArpCacheTableInitialized)
        RequeryArpCacheTable();
    if (!m_bArpCacheTableInitialized)
        return 0; //initialization failed
    return m_pArpCacheTable->dwNumEntries; //success
}

BOOL IpHelper::GetArpCacheRecord(int index, SArpCacheRecord &ArpCacheRecord)
{
    if (!m_bArpCacheTableInitialized)
        RequeryArpCacheTable();
    if (!m_bArpCacheTableInitialized)
        return 0; //initialization failed
    if (index >= GetArpCacheRecordCount())
        return FALSE;
    MIB_IPNETROW *pRow = m_pArpCacheTable->table+index;
    if (pRow->dwPhysAddrLen != 6)
        pRow=pRow;//debug
    for (unsigned long iAdapter = 0; iAdapter < GetAdaptersCount(); iAdapter++)
    {
        PIP_ADAPTER_INFO pAdapter = m_pAdaptersInfo+iAdapter;
        if (pAdapter->Index == pRow->dwIndex)
            ArpCacheRecord.nAdapter = iAdapter;
    }
    ArpCacheRecord.IpAddress = pRow->dwAddr;
    ArpCacheRecord.MacAddressLength = pRow->dwPhysAddrLen;
    memcpy(&ArpCacheRecord.MacAddress, pRow->bPhysAddr, ArpCacheRecord.MacAddressLength);
    return TRUE;
}


