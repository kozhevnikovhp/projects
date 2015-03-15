/* Adapter.h

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

#ifndef __ADAPTER_H_INCLUDED__
#define __ADAPTER_H_INCLUDED__

#include "Portable.h"
#include "AddressTable.h"
#include "DatabaseAPI.h"
#include "NetBiosSocket.h"
#include <queue>

//Forward declarations
class CNetInterface;
class CHost;
class CIcmpSocket;
class CBackgroundThread;
class COrganizeThread;
class CDatabaseThread;
class CTracertThread;
class CScheduleThread;
class CScheduleItem;
class CSchedule;


typedef LOGICAL (*SCHEDULE_ACTION)(CScheduleItem *pItem);

class CScheduleItem
{
//Constructors/destructors
public:
	CScheduleItem(CSchedule *pSchedule, CNetObject *pObject, SCHEDULE_ACTION pAction, time_t TargetTime);
	CScheduleItem(CScheduleItem *pItem); // copy constructor
	virtual ~CScheduleItem();

//Public methods
public:
	time_t GetTargetTime() { return m_TargetTime; }
	time_t MoveTargetTime(time_t MoveBy) { return (m_TargetTime += MoveBy); }
	CNetObject *GetObject() { return m_pObject; }
	LOGICAL Execute();
	bool operator < (const CScheduleItem &Item) const
	{
		return (m_TargetTime > Item.m_TargetTime);
	}

//Protected members
protected:
	CSchedule *m_pSchedule;
	time_t m_TargetTime;
	CNetObject *m_pObject;
	SCHEDULE_ACTION m_pAction;
};


class CSchedule : public std::priority_queue<CScheduleItem>
{
//Constructors/destructors
public:
	CSchedule();
	virtual ~CSchedule();

// Public members
public:
	void NewItem(CNetObject *pObject, SCHEDULE_ACTION pAction, time_t TargetTime);
	LOGICAL ExecuteNextItem();

// Protected methods
protected:
	void AddItem(CScheduleItem &Item);

//Protected members
protected:
	CPortableCriticalSection m_CriticalSection;
};

// CAdapter class - network adapter. Can contain 1 or more network interfaces (CNetInterface objects).
// 
class CAdapter
{
	friend class CSpySocketThread;
	friend class CNetwork;
	friend class CHost;
	friend class CSubnet;
	friend class CNetInterface;
//Constuctors/destructors
public:
	CAdapter();
	virtual ~CAdapter();

// Public methods
public:
	IPADDRESS_TYPE GetIpAddress() const;
	char *GetMacAddress() { return m_szMacAddress; }
	CIcmpSocket *GetBestPinger(IPADDRESS_TYPE uIpAddress) const;
	void SendRipFullTableRequest();
	void TracertAllHosts();
	void UpdateTrafficDatabase();
	void OrganizeHostsBehindRouters();

// Public overridables
public:

// Public members
public:
	CHost *CreateHost(IPADDRESS_TYPE uIpAddress, CSubnet *pSubnet = NULL, long DatabaseID = NET_OBJECT_ID_UNKNOWN);
	CHost *CreateHost(SHostDesc *pDesc);
	CSubnet *CreateSubnet(IPADDRESS_TYPE uIpAddress, IPADDRESS_TYPE uSubnetMask, long DatabaseID = NET_OBJECT_ID_UNKNOWN);
	CSubnet *CreatePseudoSubnet(IPADDRESS_TYPE uIpAddress, IPADDRESS_TYPE uSubnetMask, long DatabaseID = NET_OBJECT_ID_UNKNOWN);
	CSubnet *CreateSubnet(SSubnetDesc *pDesc);

// Protected methods
protected:
	void Init(char *pszMacAddress);
	void AddHostToSchedule(CHost *pHost);
	void AddSubnetToSchedule(CSubnet *pSubnet);
	LOGICAL Run();

	void HostNameResolved(IPADDRESS_TYPE uIpAddress, char *pszName);

	// 
	CHost *GetNextHost(IPADDRESS_TYPE &uIpAddress);
	CSubnet *GetNextSubnet(IPADDRESS_TYPE &uIpAddress);
	CNetInterface *FindInterface(IPADDRESS_TYPE uIpAddress);
	CNetInterface *GetBestInterface(IPADDRESS_TYPE uIpAddress) const;
	CHost *FindHost(IPADDRESS_TYPE uIpAddress);
	CSubnet *FindSubnet(IPADDRESS_TYPE uIpAddress);
///	void KeepAliveHost(CHost *pHost);
	void TracertHost(CHost *pHost);
	
	// Packet processing
	LOGICAL IpPacketCaptured(SIpHeader *pIpHeader);
	
	// ICMP
	void IcmpPacketCaptured(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpEchoReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpDestUnreachable(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpSourceQuench(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpRedirect(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpEchoRequest(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpRouterAdvertisement(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpRouterSolicitation(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpTimeExceeded(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpBadParameter(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpTimeStampQuery(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpTimeStampReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpInformationRequest(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpInformationReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpAddressMaskRequest(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIcmpAddressMaskReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);

	// IGMP
	void IgmpPacketCaptured(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIgmpMembershipQuery(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIgmpMembershipReportV1(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessDVMRP(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIgmpMembershipReportV2(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIgmpMembershipReportV3(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessIgmpLeaveGroup(SIpHeader *pIpHeader, SIgmpHeader *pIgmpHeader, unsigned char *pUserData, unsigned int nUserDataLength);

	// UDP
	void UdpPacketCaptured(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	// NetBIOS:137
	void ProcessUdp137(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessUdp137QueryAnswer(SIpHeader *pIpHeader, SNameServiceHeader *pNsHeader, unsigned char *pCurrent);
	void ProcessUdp137Query(SIpHeader *pIpHeader, SNameServiceHeader *pNsHeader, unsigned char *pCurrent);
	void ProcessUdp137Registration(SIpHeader *pIpHeader, SNameServiceHeader *pNsHeader, unsigned char *pCurrent);
	// NetBIOS:138
	void ProcessUdp138(SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessUdp138OrdinaryDgm(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent);
	void ProcessUdp138ErrorDgm(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent);
	void ProcessUdp138QueryRequest(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent);
	void ProcessUdp138PositiveResponse(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent);
	void ProcessUdp138NegativeResponse(SNetbiosDatagramHeader *pDgmHeader, unsigned char *pCurrent);
	// RIP
	void ProcessRIP(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength);

	// TCP
	void TcpPacketCaptured(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength);

	CHost *AddNewInterface(IPADDRESS_TYPE uIpAddress, IPADDRESS_TYPE uSubnetMask);
// Protected overridables
protected:

// Protected members
protected:
	
	CSpySocketThread *m_pSpyThread;
	COrganizeThread *m_pOrganizeThread;
	CDatabaseThread *m_pDatabaseThread;
	CTracertThread *m_pTracertThread;
	CScheduleThread *m_pScheduleThread;
	CSchedule m_Schedule;
	CNetInterface *m_pFirstInterface;
	CAddressTable m_HostTable;
	CAddressTable m_SubnetTable;
	CPortableCriticalSection m_TrafficStoringCriticalSection;
	char m_szMacAddress[32];
	LOGICAL m_bInitialized;

	unsigned long m_uLastRipRouterRequestTime;
};

#endif //__ADAPTER_H_INCLUDED__
