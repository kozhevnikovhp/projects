// PingListener.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include "SnifferSockets.h"
#include "IpHelper.h"

class CListenerSocket : public CSnifferSocket
{
//Attributes
public:
	CListenerSocket()
	{
		m_nPackets = 0;
	}
	virtual ~CListenerSocket() {}
// Public methods
// Public overridables
// Public members

// Protected methods
protected:

// Protected overridables
protected:
	virtual void OnIpPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
	{
		++m_nPackets;
		return FALSE;
	}
	virtual void OnIcmpPacket(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
	{
		printf("%d ICMP\tlen = %5d (from %s\t to %s)\n", m_nPackets, ntohs(pIpHeader->total_len),
					AddressToString(szFrom, pIpHeader->sourceIP),
					AddressToString(szTo, pIpHeader->destIP));
	}
	virtual void OnTcpPacket(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
	{
		unsigned short SrcPortNo = ntohs(pTcpHeader->SrcPortNo);
		unsigned short DstPortNo = ntohs(pTcpHeader->DstPortNo);
		printf("%d TCP:%5d/%5d len = %5d (from %s\t to %s)\n", m_nPackets, SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
			AddressToString(szFrom, pIpHeader->sourceIP),
			AddressToString(szTo, pIpHeader->destIP));
	}
	virtual void OnUdpPacket(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
	{
		unsigned short SrcPortNo = ntohs(pUdpHeader->SrcPortNo);
		unsigned short DstPortNo = ntohs(pUdpHeader->DstPortNo);
		printf("%d UDP:%5d/%5d len = %5d (from %s\t to %s)\n", m_nPackets, SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
			AddressToString(szFrom, pIpHeader->sourceIP),
			AddressToString(szTo, pIpHeader->destIP));
	}
	virtual void OnUnknownProtoPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
	{
		printf("%d UNKNOWN: PROTO=%d, %s -> %s\n", m_nPackets, pIpHeader->proto,
			AddressToString(szFrom, pIpHeader->sourceIP),
			AddressToString(szTo, pIpHeader->destIP));
	}

// Protected members
protected:
	unsigned int m_nPackets;
	char szFrom[64], szTo[64];
};

int main(int argc, char* argv[])
{
	char szIpAddress[32];
	CListenerSocket sniffer;

	printf("This workstation has the following IP-addresses:\n");
	CIpHelper helper;
	for (unsigned int i = 0; i < helper.GetIpAddressesCount(); i++)
	{
		DWORD IpAddress = helper.GetIpAddress(i);
		printf("IP-Address %d : %s\n", i, AddressToString(szIpAddress, IpAddress));
	}
	do
	{
		printf("Which one would you like to listen? (number please, not address) "); scanf("%d", &i);
	} while (i<0 && i >= helper.GetIpAddressesCount());

	sniffer.Bind(helper.GetIpAddress(i));
	sniffer.StartListening();
	
	sockaddr_in src;
	char szFrom[32], szTo[32];
	ZeroMemory(&src, sizeof(src));
	ZeroMemory(&szFrom, sizeof(szFrom));
	ZeroMemory(&szTo, sizeof(szTo));
	unsigned int nReadBytes = 0;
	unsigned int nPackets = 0;
	while (1)
	{
		if (sniffer.WaitForPacket())
		{
		//	if (nPackets >= 100)
		//		sniffer.StopListening();
		}
	}
	return 0;
}

