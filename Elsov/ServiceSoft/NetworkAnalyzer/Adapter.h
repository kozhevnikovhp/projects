/* Adapter.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	01 Jul 2006 - initial creation
	10 Aug 2006 - RIP requests
	21 Aug 2006 - tracert and keepalive
	28 Nov 2006 - loading network objects and their properties from database
	20 May 2007 - transit traffic processing and storing it in DB
*/

#ifndef __ADAPTER_H_INCLUDED__
#define __ADAPTER_H_INCLUDED__

#include "Portable.h"
#include "AddressTable.h"
#include "DatabaseAPI.h"

//Forward declarations
class CNetInterface;
class CHost;
class CIcmpSocket;

#pragma pack(push, 1)

///////////////////////////////////////////////////////////////
// Name service, UDP port 137

typedef struct // RFC-883, 1002
{
protected:
	unsigned short TransactionID;
	
	unsigned char RD:1;		// Recursion Desired
	unsigned char TC:1;		// TrunCation
	unsigned char AA:1;		// Authoritative Answer 
	unsigned char OPcode:4;
	unsigned char QR:1;		// query(0) or response(1)

	unsigned char Rcode:4;		// Response code
	unsigned char NMflags:3;	// unused
	unsigned char RA:1;		// Recursion Available 
	
	unsigned short QDcount;
	unsigned short ANcount;
	unsigned short NScount;
	unsigned short ARcount;
public:
	LOGICAL IsTypeQuery() { return (QR == 0); }
	LOGICAL IsTypeAnswer() { return !IsTypeQuery(); }

	LOGICAL IsOperationQuery() { return (OPcode == 0); }
	LOGICAL IsOperationRegistration() { return (OPcode == 0x5); }
	LOGICAL IsOperationRelease() { return (OPcode == 0x6); }
	LOGICAL IsOperationWAck() { return (OPcode == 0x7); }
	LOGICAL IsOperationRefresh() { return (OPcode == 0x8); }

	LOGICAL GetQuestionCount() { return ntohs(QDcount); }
	LOGICAL GetAnswerCount() { return ntohs(ANcount); }
	LOGICAL GetAdditionalRecordCount() { return ntohs(ARcount); }
} SNameServiceHeader;


///////////////////////////////////////////////////////////////
// Name service, UDP port 138

typedef struct // RFC-883, 1002
{
protected:
	unsigned char MSG_TYPE;			// 0x10...0x16
	unsigned char Flags;			// flags
	unsigned short DatagramID;		// Datagram ID 
	unsigned long SourceIP;			// Source IP-address
	unsigned short SourcePort;		// Source port No
	unsigned short DatagramLength;	// Datagram length
	unsigned short PacketOffset;	//

public:
	LOGICAL IsDirectUniqueDgm() { return (MSG_TYPE == 0x10); }
	LOGICAL IsDirectGroupDgm() { return (MSG_TYPE == 0x11); }
	LOGICAL IsBroadcastDgm() { return (MSG_TYPE == 0x12); }
	LOGICAL IsErrorDgm() { return (MSG_TYPE == 0x13); }
	LOGICAL IsDgmQueryRequest() { return (MSG_TYPE == 0x14); }
	LOGICAL IsPositiveResponse() { return (MSG_TYPE == 0x15); }
	LOGICAL IsNegativeResponse() { return (MSG_TYPE == 0x16); }

	IPADDRESS_TYPE GetIP() { return SourceIP; }	
} SNetbiosDatagramHeader;

#pragma pack(pop)


// CAdapter class - network adapter. Can contain 1 or more network interfaces (CNetInterface objects).
// 
class CAdapter
{
	friend class CSpySocketThread;
	friend class CBackgroundThread;
	friend class CTracertThread;
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
	IPADDRESS_TYPE GetIpAddress();
	char *GetMacAddress() { return m_szMacAddress; }
	CIcmpSocket *GetBestPinger(IPADDRESS_TYPE uIpAddress);

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	void Init(char *pszMacAddress);
	LOGICAL Run();
	void NetworkChanged();
	void HostChanged(CHost *pHost);
	void NewSubnetCreated(CSubnet *pSubnet);
	void SubnetChanged(CSubnet *pSubnet);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet);

	CHost *CreateHost(IPADDRESS_TYPE uIpAddress, CSubnet *pSubnet = NULL, long DatabaseID = NET_OBJECT_ID_UNKNOWN);
	CHost *CreateHost(SHostDesc *pDesc);
	CSubnet *CreateSubnet(IPADDRESS_TYPE uIpAddress, IPADDRESS_TYPE uSubnetMask, long DatabaseID = NET_OBJECT_ID_UNKNOWN);
	CSubnet *CreatePseudoSubnet(IPADDRESS_TYPE uIpAddress, IPADDRESS_TYPE uSubnetMask, long DatabaseID = NET_OBJECT_ID_UNKNOWN);
	CSubnet *CreateSubnet(SSubnetDesc *pDesc);
	void HostNameResolved(IPADDRESS_TYPE uIpAddress, char *pszName);

	// 
	CHost *GetNextHost(IPADDRESS_TYPE &uIpAddress);
	CSubnet *GetNextSubnet(IPADDRESS_TYPE &uIpAddress);
	CNetInterface *FindInterface(IPADDRESS_TYPE uIpAddress);
	CNetInterface *GetBestInterface(IPADDRESS_TYPE uIpAddress);
	CHost *FindHost(IPADDRESS_TYPE uIpAddress);
	void SendRipFullTableRequest();
	void KeepAliveHost(CHost *pHost);
	void TracertHost(CHost *pHost);
	void UpdateTrafficDatabase();
	
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
	void ProcessUdp137(SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength);
	void ProcessUdp137QueryAnswer(SNameServiceHeader *pNsHeader, unsigned char *pCurrent);
	void ProcessUdp137Query(SNameServiceHeader *pNsHeader, unsigned char *pCurrent);
	void ProcessUdp137Registration(SNameServiceHeader *pNsHeader, unsigned char *pCurrent);
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

	CHost *AddNewInterface(CNetwork *pNetwork, IPADDRESS_TYPE uIpAddress, IPADDRESS_TYPE uSubnetMask);
// Protected overridables
protected:

// Protected members
protected:
	
	CNetwork *m_pNetwork;
	CSpySocketThread *m_pSpyThread;
	CBackgroundThread *m_pBackgroundThread;
	CTracertThread *m_pTracertThread;
	CNetInterface *m_pFirstInterface;
	CAddressTable m_HostTable;
	CAddressTable m_SubnetTable;
	CPortableCriticalSection m_TrafficStoringCriticalSection;
	char m_szMacAddress[32];
	LOGICAL m_bInitialized;

	unsigned long m_uLastRipRouterRequestTime;
};

#endif //__ADAPTER_H_INCLUDED__
