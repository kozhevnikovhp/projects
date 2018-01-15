#include <stdio.h>
#include <string.h>
#include <time.h>
#include "network/sniffer.h"
#include "network/misc.h"
#include "portable/portable.h"

using namespace common::network;

class ProtocolStat
{
public:
    ProtocolStat()
        : nInputPackets_(0), nOutputPackets_(0), nInputOctets_(0), nOutputOctets_()
    { }

    void report(const char *pszFileName, ProtocolStat &prev, unsigned int deltaTime) const
    {
        FILE *pFile = fopen(pszFileName, "a");
        if (pFile)
        {
            time_t rawtime;
            char strTime[80];

            time(&rawtime);
            struct tm *pTimeInfo = localtime(&rawtime);

            strftime(strTime, sizeof(strTime), "%d.%m.%Y %H:%M:%S", pTimeInfo);
            // Absolute numbers
            fprintf(pFile, "%s\t\t%lld\t%lld\t%lld\t%lld", strTime, nInputPackets_, nInputOctets_, nOutputPackets_, nOutputOctets_);
            //fprintf(stdout, "%s\t\t%lld\t%lld\t%lld\t%lld", strTime, nInputPackets_, nInputOctets_, nOutputPackets_, nOutputOctets_);
            // Difference with previous one
            fprintf(pFile, "\t\t%lld\t%lld\t%lld\t%lld", nInputPackets_-prev.nInputPackets_, nInputOctets_-prev.nInputOctets_, nOutputPackets_-prev.nOutputPackets_, nOutputOctets_-prev.nOutputOctets_);
            // Speed (/sec)
            fprintf(pFile, "\t\t%.2f\t%.2f\t%.2f\t%.2f", (nInputPackets_-prev.nInputPackets_)/double(deltaTime), (nInputOctets_-prev.nInputOctets_)/double(deltaTime), (nOutputPackets_-prev.nOutputPackets_)/double(deltaTime), (nOutputOctets_-prev.nOutputOctets_)/double(deltaTime));

            fprintf(pFile, "\n");
            //fprintf(stdout, "\n");
        }
        fclose(pFile);
    }

    void update(unsigned int nPacketSize, bool bInput)
    {
        if (bInput)
        {
            ++nInputPackets_;
            nInputOctets_ += nPacketSize;
        }
        else
        {
            ++nOutputPackets_;
            nOutputOctets_ += nPacketSize;
        }
    }
protected:
    long long nInputPackets_;
    long long nOutputPackets_;
    long long nInputOctets_;
    long long nOutputOctets_;
};

class ListenerSocket : public SnifferSocket
{
//Attributes
public:
    ListenerSocket(IPADDRESS_TYPE teloIP)
    {
        bInputPacket_ = false;
        teloIP_ = teloIP;
        lastStatTime_ = 0;
        IPADDRESS_TYPE ifaceIP;
        std::string ifaceName; // not used so far for Windows, Linux only
        bool bSuccess = common::network::findBestInterface(teloIP, ifaceIP, subnetMask_, ifaceName);
        if (bSuccess)
        {
#if (SOCKETS_WSA)
            printf("Listening local interface %s to figure out traffic of Telo %s...\n",
                   addressToDotNotation(ifaceIP).c_str(),
                   addressToDotNotation(teloIP_).c_str());
            promiscModeOn(ifaceIP);
#elif (SOCKETS_BSD)
            printf("Listening local interface %s to figure out traffic of Telo %s...\n",
                   ifaceName.c_str(),
                   addressToDotNotation(teloIP_).c_str());
            promiscModeOn(ifaceName.c_str());
#endif
        }
        else
        {
            printf("ERROR: could not find appropriate interface for listenning\n");
        }
        reportStatistics();
    }


// Public methods
// Public overridables
// Public members

// Protected methods
protected:
    bool isPacketOfInterest(const SIpHeader *pIpHeader) const
    {
        if ((pIpHeader->sourceIP != teloIP_) && (pIpHeader->destIP != teloIP_))
            return false; // Telo traffic only
        if (isTheSameSubnet(pIpHeader->sourceIP, pIpHeader->destIP, subnetMask_))
            return false; // Skip LAN traffic
        return true;
    }
    void reportStatistics()
    {
        unsigned int currentTime = common::portable::portableGetCurrentTimeSec();
        unsigned int deltaTime = currentTime - lastStatTime_;
        if (deltaTime < 60*1) // every 1 min
            return; // too early to do something
        IpStatTotal_.report("ip.txt", IpStatLast_, deltaTime);
        IcmpStatTotal_.report("icmp.txt", IcmpStatLast_, deltaTime);
        IgmpStatTotal_.report("igmp.txt", IgmpStatLast_, deltaTime);
        TcpStatTotal_.report("tcp.txt", TcpStatLast_, deltaTime);
        UdpStatTotal_.report("udp.txt", UdpStatLast_, deltaTime);

        lastStatTime_ = currentTime;
        memcpy(&IpStatLast_, &IpStatTotal_, sizeof(ProtocolStat));
        memcpy(&IcmpStatLast_, &IcmpStatTotal_, sizeof(ProtocolStat));
        memcpy(&IgmpStatLast_, &IgmpStatTotal_, sizeof(ProtocolStat));
        memcpy(&TcpStatLast_, &TcpStatTotal_, sizeof(ProtocolStat));
        memcpy(&UdpStatLast_, &UdpStatTotal_, sizeof(ProtocolStat));
    }

// Protected overridables
protected:
    virtual void ipPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, const unsigned char *pPayload,  int nPayloadLen)
    {
        reportStatistics();
        bPacketOfInterest_ = isPacketOfInterest(pIpHeader);
        if (!bPacketOfInterest_)
            return;
        bInputPacket_ = (pIpHeader->destIP == teloIP_);
        IpStatTotal_.update(nPacketLen, bInputPacket_);
    }
    virtual void icmpPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, SIcmpHeader *pIcmpHeader, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        printf("ICMP\tlen = %5d (from %s\t to %s)\n", ntohs(pIpHeader->total_len),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
        IcmpStatTotal_.update(nPacketLen, bInputPacket_);
    }
    virtual void igmpPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, SIgmpHeader *pIgmpHeader, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        printf("IGMP\tlen = %5d (from %s\t to %s)\n", ntohs(pIpHeader->total_len),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
        IgmpStatTotal_.update(nPacketLen, bInputPacket_);
    }
    virtual void tcpPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, STcpHeader *pTcpHeader, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        unsigned short SrcPortNo = ntohs(pTcpHeader->SrcPortNo);
        unsigned short DstPortNo = ntohs(pTcpHeader->DstPortNo);
        printf("TCP:%5d/%5d len = %5d (from %s\t to %s)\n", SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
        TcpStatTotal_.update(nPacketLen, bInputPacket_);
    }
    virtual void udpPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, SUdpHeader *pUdpHeader, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        unsigned short SrcPortNo = ntohs(pUdpHeader->SrcPortNo);
        unsigned short DstPortNo = ntohs(pUdpHeader->DstPortNo);
        printf("UDP:%5d/%5d len = %5d (from %s\t to %s)\n", SrcPortNo, DstPortNo, ntohs(pIpHeader->total_len),
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
        UdpStatTotal_.update(nPacketLen, bInputPacket_);
    }
    virtual void unknownProtoPacketCaptured(const SIpHeader *pIpHeader, int nPacketLen, const unsigned char *pPayload, int nPayloadLen)
    {
        if (!bPacketOfInterest_)
            return;
        printf("UNKNOWN: PROTO=%d, %s -> %s\n", pIpHeader->proto,
            addressToDotNotation(pIpHeader->sourceIP).c_str(),
            addressToDotNotation(pIpHeader->destIP).c_str());
    }

// Protected members
protected:
    IPADDRESS_TYPE subnetMask_, teloIP_;
    ProtocolStat IpStatTotal_, IpStatLast_;
    ProtocolStat UdpStatTotal_, UdpStatLast_;
    ProtocolStat TcpStatTotal_, TcpStatLast_;
    ProtocolStat IcmpStatTotal_, IcmpStatLast_;
    ProtocolStat IgmpStatTotal_, IgmpStatLast_;
    unsigned int lastStatTime_;
    bool bPacketOfInterest_;
    bool bInputPacket_;
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Not enough arguments\nUsage: TrafficCounter TeloIP\n");
        return 0;
    }

    IpSocket::InitSockets();

    IPADDRESS_TYPE teloIP = stringToAddress(argv[1]);
    if (!teloIP)
    {
        printf("Cannot resolve %s to IP-address\n", argv[1]);
        return 0;
    }

    ListenerSocket sniffer(teloIP);

    if (!sniffer.isCreated())
    {
        printf("Abnormal termination!\n");
        return 1;
    }

    while (1)
    {
        if (sniffer.waitForPacket())
        {
        }
    }

    return 0;
}

#if 0
void StartSniffing (SOCKET Sock); //This will sniff here and there

void ProcessPacket (char* , int); //This will decide how to digest
void PrintIpHeader (char*);
void PrintIcmpPacket (char* , int);
void PrintUdpPacket (char* , int);
void PrintTcpPacket (char* , int);
void ConvertToHex (char* , unsigned int);
void PrintData (char* , int);

typedef struct ip_hdr
{
    unsigned char ip_header_len:4; // 4-bit header length (in 32-bit words) normally=5 (Means 20 Bytes may be 24 also)
    unsigned char ip_version :4; // 4-bit IPv4 version
    unsigned char ip_tos; // IP type of service
    unsigned short ip_total_length; // Total length
    unsigned short ip_id; // Unique identifier

    unsigned char ip_frag_offset :5; // Fragment offset field

    unsigned char ip_more_fragment :1;
    unsigned char ip_dont_fragment :1;
    unsigned char ip_reserved_zero :1;

    unsigned char ip_frag_offset1; //fragment offset

    unsigned char ip_ttl; // Time to live
    unsigned char ip_protocol; // Protocol(TCP,UDP etc)
    unsigned short ip_checksum; // IP checksum
    unsigned int ip_srcaddr; // Source address
    unsigned int ip_destaddr; // Source address
} IPV4_HDR;

typedef struct udp_hdr
{
    unsigned short source_port; // Source port no.
    unsigned short dest_port; // Dest. port no.
    unsigned short udp_length; // Udp packet length
    unsigned short udp_checksum; // Udp checksum (optional)
} UDP_HDR;

// TCP header
typedef struct tcp_header
{
    unsigned short source_port; // source port
    unsigned short dest_port; // destination port
    unsigned int sequence; // sequence number - 32 bits
    unsigned int acknowledge; // acknowledgement number - 32 bits

    unsigned char ns :1; //Nonce Sum Flag Added in RFC 3540.
    unsigned char reserved_part1:3; //according to rfc
    unsigned char data_offset:4; /*The number of 32-bit words in the TCP header.
    This indicates where the data begins.
    The length of the TCP header is always a multiple
    of 32 bits.*/

    unsigned char fin :1; //Finish Flag
    unsigned char syn :1; //Synchronise Flag
    unsigned char rst :1; //Reset Flag
    unsigned char psh :1; //Push Flag
    unsigned char ack :1; //Acknowledgement Flag
    unsigned char urg :1; //Urgent Flag

    unsigned char ecn :1; //ECN-Echo Flag
    unsigned char cwr :1; //Congestion Window Reduced Flag

    ////////////////////////////////

    unsigned short window; // window
    unsigned short checksum; // checksum
    unsigned short urgent_pointer; // urgent pointer
} TCP_HDR;

typedef struct icmp_hdr
{
    BYTE type; // ICMP Error type
    BYTE code; // Type sub code
    USHORT checksum;
    USHORT id;
    USHORT seq;
} ICMP_HDR;

FILE *logfile;
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j;
struct sockaddr_in source,dest;
char hex[2];

//Its free!
IPV4_HDR *iphdr;
TCP_HDR *tcpheader;
UDP_HDR *udpheader;
ICMP_HDR *icmpheader;

int main()
{
    SOCKET sniffer;
    struct in_addr addr;
    int in;

    char hostname[100];
    struct hostent *local;
    WSADATA wsa;

    logfile=fopen("log.txt","w");
    if(logfile == NULL)
    {
        printf("Unable to create file.");
    }

    //Initialise Winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("WSAStartup() failed.\n");
        return 1;
    }
    printf("Initialised");

    //Create a RAW Socket
    printf("\nCreating RAW Socket...");
    sniffer = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    if (sniffer == INVALID_SOCKET)
    {
        printf("Failed to create raw socket.\n");
        return 1;
    }
    printf("Created.");

    //Retrive the local hostname
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
    {
        printf("Error : %d",WSAGetLastError());
        return 1;
    }
    printf("\nHost name : %s \n",hostname);

    //Retrive the available IPs of the local host
    local = gethostbyname(hostname);
    printf("\nAvailable Network Interfaces : \n");
    if (local == NULL)
    {
        printf("Error : %d.\n",WSAGetLastError());
        return 1;
    }

    for (i = 0; local->h_addr_list[i] != 0; ++i)
    {
        memcpy(&addr, local->h_addr_list[i], sizeof(struct in_addr));
        printf("Interface Number : %d Address : %s\n",i,inet_ntoa(addr));
    }

    printf("Enter the interface number you would like to sniff : ");
    scanf("%d",&in);

    memset(&dest, 0, sizeof(dest));
    memcpy(&dest.sin_addr.s_addr,local->h_addr_list[in],sizeof(dest.sin_addr.s_addr));
    dest.sin_family = AF_INET;
    dest.sin_port = 0;

    printf("\nBinding socket to local system and port 0 ...");
    if (bind(sniffer,(struct sockaddr *)&dest,sizeof(dest)) == SOCKET_ERROR)
    {
        printf("bind(%s) failed.\n", inet_ntoa(addr));
        return 1;
    }
    printf("Binding successful");

    //Enable this socket with the power to sniff : SIO_RCVALL is the key Receive ALL ;)

    j=1;
    printf("\nSetting socket to sniff...");
    if (WSAIoctl(sniffer, SIO_RCVALL, &j, sizeof(j), 0, 0, (LPDWORD) &in , 0 , 0) == SOCKET_ERROR)
    {
        printf("WSAIoctl() failed.\n");
        return 1;
    }
    printf("Socket set.");

    //Begin
    printf("\nStarted Sniffing\n");
    printf("Packet Capture Statistics...\n");
    StartSniffing(sniffer); //Happy Sniffing

    //End
    closesocket(sniffer);
    WSACleanup();

    return 0;
}

void StartSniffing(SOCKET sniffer)
{
    char *Buffer = (char *)malloc(65536); //Its Big!
    int mangobyte;

    if (Buffer == NULL)
    {
        printf("malloc() failed.\n");
        return;
    }

    do
    {
        mangobyte = recvfrom(sniffer , Buffer , 65536 , 0 , 0 , 0); //Eat as much as u can

        if(mangobyte > 0)
        {
            ProcessPacket(Buffer, mangobyte);
        }
        else
        {
            printf( "recvfrom() failed.\n");
        }
    }
    while (mangobyte > 0);

    free(Buffer);
}

void ProcessPacket(char* Buffer, int Size)
{
    iphdr = (IPV4_HDR *)Buffer;
    ++total;

    switch (iphdr->ip_protocol) //Check the Protocol and do accordingly...
    {
        case 1: //ICMP Protocol
        ++icmp;
        PrintIcmpPacket(Buffer,Size);
        break;

        case 2: //IGMP Protocol
        ++igmp;
        break;

        case 6: //TCP Protocol
        ++tcp;
        PrintTcpPacket(Buffer,Size);
        break;

        case 17: //UDP Protocol
        ++udp;
        PrintUdpPacket(Buffer,Size);
        break;

        default: //Some Other Protocol like ARP etc.
        ++others;
        break;
    }
    printf("TCP : %d UDP : %d ICMP : %d IGMP : %d Others : %d Total : %d\r",tcp,udp,icmp,igmp,others,total);
}

void PrintIpHeader (char* Buffer )
{
    unsigned short iphdrlen;

    iphdr = (IPV4_HDR *)Buffer;
    iphdrlen = iphdr->ip_header_len*4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iphdr->ip_srcaddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iphdr->ip_destaddr;

    fprintf(logfile,"\n");
    fprintf(logfile,"IP Header\n");
    fprintf(logfile," |-IP Version : %d\n",(unsigned int)iphdr->ip_version);
    fprintf(logfile," |-IP Header Length : %d DWORDS or %d Bytes\n",(unsigned int)iphdr->ip_header_len,((unsigned int)(iphdr->ip_header_len))*4);
    fprintf(logfile," |-Type Of Service : %d\n",(unsigned int)iphdr->ip_tos);
    fprintf(logfile," |-IP Total Length : %d Bytes(Size of Packet)\n",ntohs(iphdr->ip_total_length));
    fprintf(logfile," |-Identification : %d\n",ntohs(iphdr->ip_id));
    fprintf(logfile," |-Reserved ZERO Field : %d\n",(unsigned int)iphdr->ip_reserved_zero);
    fprintf(logfile," |-Dont Fragment Field : %d\n",(unsigned int)iphdr->ip_dont_fragment);
    fprintf(logfile," |-More Fragment Field : %d\n",(unsigned int)iphdr->ip_more_fragment);
    fprintf(logfile," |-TTL : %d\n",(unsigned int)iphdr->ip_ttl);
    fprintf(logfile," |-Protocol : %d\n",(unsigned int)iphdr->ip_protocol);
    fprintf(logfile," |-Checksum : %d\n",ntohs(iphdr->ip_checksum));
    fprintf(logfile," |-Source IP : %s\n",inet_ntoa(source.sin_addr));
    fprintf(logfile," |-Destination IP : %s\n",inet_ntoa(dest.sin_addr));
}

void PrintTcpPacket(char* Buffer, int Size)
{
    unsigned short iphdrlen;

    iphdr = (IPV4_HDR *)Buffer;
    iphdrlen = iphdr->ip_header_len*4;

    tcpheader=(TCP_HDR*)(Buffer+iphdrlen);

    fprintf(logfile,"\n\n***********************TCP Packet*************************\n");

    PrintIpHeader( Buffer );

    fprintf(logfile,"\n");
    fprintf(logfile,"TCP Header\n");
    fprintf(logfile," |-Source Port : %u\n",ntohs(tcpheader->source_port));
    fprintf(logfile," |-Destination Port : %u\n",ntohs(tcpheader->dest_port));
    fprintf(logfile," |-Sequence Number : %u\n",ntohl(tcpheader->sequence));
    fprintf(logfile," |-Acknowledge Number : %u\n",ntohl(tcpheader->acknowledge));
    fprintf(logfile," |-Header Length : %d DWORDS or %d BYTES\n"
    ,(unsigned int)tcpheader->data_offset,(unsigned int)tcpheader->data_offset*4);
    fprintf(logfile," |-CWR Flag : %d\n",(unsigned int)tcpheader->cwr);
    fprintf(logfile," |-ECN Flag : %d\n",(unsigned int)tcpheader->ecn);
    fprintf(logfile," |-Urgent Flag : %d\n",(unsigned int)tcpheader->urg);
    fprintf(logfile," |-Acknowledgement Flag : %d\n",(unsigned int)tcpheader->ack);
    fprintf(logfile," |-Push Flag : %d\n",(unsigned int)tcpheader->psh);
    fprintf(logfile," |-Reset Flag : %d\n",(unsigned int)tcpheader->rst);
    fprintf(logfile," |-Synchronise Flag : %d\n",(unsigned int)tcpheader->syn);
    fprintf(logfile," |-Finish Flag : %d\n",(unsigned int)tcpheader->fin);
    fprintf(logfile," |-Window : %d\n",ntohs(tcpheader->window));
    fprintf(logfile," |-Checksum : %d\n",ntohs(tcpheader->checksum));
    fprintf(logfile," |-Urgent Pointer : %d\n",tcpheader->urgent_pointer);
    fprintf(logfile,"\n");
    fprintf(logfile," DATA Dump ");
    fprintf(logfile,"\n");

    fprintf(logfile,"IP Header\n");
    PrintData(Buffer,iphdrlen);

    fprintf(logfile,"TCP Header\n");
    PrintData(Buffer+iphdrlen,tcpheader->data_offset*4);

    fprintf(logfile,"Data Payload\n");
    PrintData(Buffer+iphdrlen+tcpheader->data_offset*4
    ,(Size-tcpheader->data_offset*4-iphdr->ip_header_len*4));

    fprintf(logfile,"\n###########################################################");
}

void PrintUdpPacket(char *Buffer,int Size)
{
    unsigned short iphdrlen;

    iphdr = (IPV4_HDR *)Buffer;
    iphdrlen = iphdr->ip_header_len*4;

    udpheader = (UDP_HDR *)(Buffer + iphdrlen);

    fprintf(logfile,"\n\n***********************UDP Packet*************************\n");

    PrintIpHeader(Buffer);

    fprintf(logfile,"\nUDP Header\n");
    fprintf(logfile," |-Source Port : %d\n",ntohs(udpheader->source_port));
    fprintf(logfile," |-Destination Port : %d\n",ntohs(udpheader->dest_port));
    fprintf(logfile," |-UDP Length : %d\n",ntohs(udpheader->udp_length));
    fprintf(logfile," |-UDP Checksum : %d\n",ntohs(udpheader->udp_checksum));

    fprintf(logfile,"\n");
    fprintf(logfile,"IP Header\n");

    PrintData(Buffer,iphdrlen);

    fprintf(logfile,"UDP Header\n");

    PrintData(Buffer+iphdrlen,sizeof(UDP_HDR));

    fprintf(logfile,"Data Payload\n");

    PrintData(Buffer+iphdrlen+sizeof(UDP_HDR) ,(Size - sizeof(UDP_HDR) - iphdr->ip_header_len*4));

    fprintf(logfile,"\n###########################################################");
}

void PrintIcmpPacket(char* Buffer , int Size)
{
    unsigned short iphdrlen;

    iphdr = (IPV4_HDR *)Buffer;
    iphdrlen = iphdr->ip_header_len*4;

    icmpheader=(ICMP_HDR*)(Buffer+iphdrlen);

    fprintf(logfile,"\n\n***********************ICMP Packet*************************\n");
    PrintIpHeader(Buffer);

    fprintf(logfile,"\n");

    fprintf(logfile,"ICMP Header\n");
    fprintf(logfile," |-Type : %d",(unsigned int)(icmpheader->type));

    if((unsigned int)(icmpheader->type)==11)
    {
        fprintf(logfile," (TTL Expired)\n");
    }
    else if((unsigned int)(icmpheader->type)==0)
    {
        fprintf(logfile," (ICMP Echo Reply)\n");
    }

    fprintf(logfile," |-Code : %d\n",(unsigned int)(icmpheader->code));
    fprintf(logfile," |-Checksum : %d\n",ntohs(icmpheader->checksum));
    fprintf(logfile," |-ID : %d\n",ntohs(icmpheader->id));
    fprintf(logfile," |-Sequence : %d\n",ntohs(icmpheader->seq));
    fprintf(logfile,"\n");

    fprintf(logfile,"IP Header\n");
    PrintData(Buffer,iphdrlen);

    fprintf(logfile,"UDP Header\n");
    PrintData(Buffer+iphdrlen,sizeof(ICMP_HDR));

    fprintf(logfile,"Data Payload\n");
    PrintData(Buffer+iphdrlen+sizeof(ICMP_HDR) , (Size - sizeof(ICMP_HDR) - iphdr->ip_header_len*4));

    fprintf(logfile,"\n###########################################################");
}

/*
    Print the hex values of the data
*/
void PrintData (char* data , int Size)
{
    char a , line[17] , c;
    int j;

    //loop over each character and print
    for(i=0 ; i < Size ; i++)
    {
        c = data[i];

        //Print the hex value for every character , with a space. Important to make unsigned
        fprintf(logfile," %.2x", (unsigned char) c);

        //Add the character to data line. Important to make unsigned
        a = ( c >=32 && c <=128) ? (unsigned char) c : '.';

        line[i%16] = a;

        //if last character of a line , then print the line - 16 characters in 1 line
        if( (i!=0 && (i+1)%16==0) || i == Size - 1)
        {
            line[i%16 + 1] = '\0';

            //print a big gap of 10 characters between hex and characters
            fprintf(logfile ,"          ");

            //Print additional spaces for last lines which might be less than 16 characters in length
            for( j = strlen(line) ; j < 16; j++)
            {
                fprintf(logfile , "   ");
            }

            fprintf(logfile , "%s \n" , line);
        }
    }

    fprintf(logfile , "\n");
}
#endif
