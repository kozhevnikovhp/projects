#include <QCoreApplication>
#include "SnifferSockets.h"
#include "IpHelper.h"

class CListenerSocket : public CSnifferSocket
{
//Attributes
public:
    CListenerSocket(IPADDRESS_TYPE myAddress)
    {
        myAddress_ = myAddress;
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
    virtual bool OnIpPacket(SIpHeader *pIpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        ++m_nPackets;
        return false;
    }
    virtual void OnIcmpPacket(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        printf("%d ICMP\tlen = %5d (from %s\t to %s)\n", m_nPackets, ntohs(pIpHeader->total_len),
                    AddressToString(szFrom, pIpHeader->sourceIP),
                    AddressToString(szTo, pIpHeader->destIP));
    }
    virtual void OnTcpPacket(SIpHeader *pIpHeader, STcpHeader *pTcpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        if (pIpHeader->sourceIP == myAddress_)
            return; // do nothing
        if (pIpHeader->destIP == myAddress_)
            return; // do nothing
        unsigned short SrcPortNo = ntohs(pTcpHeader->SrcPortNo);
        unsigned short DstPortNo = ntohs(pTcpHeader->DstPortNo);
        printf("%d TCP:%5d/%5d len = %5d (from %s\t to %s)\n", m_nPackets, SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
            AddressToString(szFrom, pIpHeader->sourceIP),
            AddressToString(szTo, pIpHeader->destIP));
    }
    virtual void OnUdpPacket(SIpHeader *pIpHeader, SUdpHeader *pUdpHeader, unsigned char *pUserData, unsigned int nUserDataLength)
    {
        if (pIpHeader->sourceIP == myAddress_)
            return; // do nothing
        if (pIpHeader->destIP == myAddress_)
            return; // do nothing
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
    IPADDRESS_TYPE myAddress_;
};

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    char szIpAddress[32];

    printf("This workstation has the following IP-addresses:\n");
    CIpHelper helper;
    unsigned int i;
    for (i = 0; i < helper.GetIpAddressesCount(); i++)
    {
        DWORD IpAddress = helper.GetIpAddress(i);
        helper.Get
        printf("IP-Address %d : %s\n", i, AddressToString(szIpAddress, IpAddress));
    }
    do
    {
        printf("Which one would you like to listen? (number please, not address) ");
        scanf("%d", &i);
    } while (i<0 && i >= helper.GetIpAddressesCount());

    CListenerSocket sniffer(helper.GetIpAddress(i));
    sniffer.Bind(helper.GetIpAddress(i));
    sniffer.EnablePromiscMode();

    while (1)
    {
        if (sniffer.WaitForPacket())
        {
        }
    }

    return 0;
    //return a.exec();
}
