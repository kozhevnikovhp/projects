#ifndef __IP_H_INCLUDED__
#define __IP_H_INCLUDED__

typedef unsigned int IPADDRESS_TYPE;
typedef unsigned short IPPORT;

#ifdef SOCKETS_WSA
#include <winsock2.h>
#include <ws2tcpip.h>

#ifndef socklen_t
#define socklen_t int
#endif

#ifndef EINPROGRESS 
#define EINPROGRESS  WSAEINPROGRESS
#endif

#ifndef EALREADY
#define EALREADY     WSAEALREADY
#endif

#ifndef EISCONN
#define EISCONN      WSAEISCONN
#endif

#ifndef EINTR
#define EINTR        WSAEINTR
#endif

#ifndef EAGAIN
#define EAGAIN       WSAEAGAIN
#endif

#ifndef ENOTCONN
#define ENOTCONN     WSAENOTCONN
#endif

#ifndef ECONRESET
#define ECONNRESET   WSAECONNRESET
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT    WSAETIMEDOUT
#endif
#endif

#ifdef SOCKETS_BSD
typedef int SOCKET;
const int INVALID_SOCKET  = (~0);
const int SOCKET_ERROR    = (-1);
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

namespace common {

namespace network {

// Additional constants
const IPADDRESS_TYPE LOCAL_HOST = 0x0100007F; // ("127.0.0.1:)
const IPADDRESS_TYPE INADDR_ANY_HOST = 0x010000E0; //("224.0.0.1")
const IPADDRESS_TYPE INADDR_ANY_ROUTER = 0x020000E0; //("224.0.0.2")

typedef	union
{
    struct { unsigned char b1,b2,b3,b4; } bytes;
    unsigned long addr;
} SOctetConverter;

typedef union
{
    sockaddr addr;
    sockaddr_in addrin;
} sSockAddrIn;

#pragma pack(push, 1)

// IP header definition
typedef struct
{
        unsigned char	h_len:4;			// length of the header
        unsigned char	version:4;			// Version of IP
        unsigned char	tos;				// Type of service
        unsigned short	total_len;			// total length of packet
        unsigned short	ident;				// unique identifier
        unsigned short	frag_and_flags;                 // flags
        unsigned char	ttl;				// time to live value
        unsigned char	proto;				// protocol (TCP, UDP, etc.)
        unsigned short	checksum;			// IP checksum
        unsigned long	sourceIP;			// source IP address
        unsigned long	destIP;				// destination IP address
} SIpHeader;

typedef struct
{
        unsigned short SrcPortNo;
        unsigned short DstPortNo;
        unsigned short Length;
        unsigned short CRC;
} SUdpHeader;

typedef struct
{
        unsigned short	SrcPortNo;
        unsigned short	DstPortNo;
        unsigned long	Other[5];
} STcpHeader;

struct SPseudoHeader
{
        unsigned int src_addr; //
        unsigned int dst_addr; //
        unsigned char zero ; //
        unsigned char proto; //
        unsigned short length; //
};

// ICMP header definition
typedef struct
{
        unsigned char	i_type;						// ICMP packet type
        unsigned char	i_code;						// type subcode
        unsigned short	i_chksum;					// packet checksum
        unsigned short	i_id;						// unique packet ID
        unsigned short	i_seq;						// packet sequence number
        unsigned long	timestamp;					// timestamp
} SIcmpHeader;

// IGMP-protocol (RFC-1112, 2236, 3376)
typedef struct
{
        unsigned char type;
        unsigned char unused;
        unsigned short CRC;
        unsigned long IP;			// Group address
} SIgmpHeader;

#pragma pack(pop)


// cIpSocket - class for all IP-sockets
class IpSocket
{
//Constructors/destructors
public:
    IpSocket();
    virtual ~IpSocket();
    static int InitSockets();
    static int FinitSockets();

// Public methods
    unsigned long getLastErrorCode() const { return m_LastErrorCode; }
    void storeLastErrorCode();
    SOCKET GetSocket() const { return m_Socket; }
    bool isCreated() const { return (m_Socket != INVALID_SOCKET); }
    bool isDestroyed() const { return (m_Socket == INVALID_SOCKET); }
    void destroy();
    bool bind(IPPORT uPort, IPADDRESS_TYPE InterfaceIpAddress = INADDR_ANY);
    bool SetReadTimeout(unsigned int timeout);
    bool SetWriteTimeout(unsigned int timeout);
    bool SetSendTTL(unsigned char ttl);
    bool SetSendTOS(unsigned char tos);
    bool EnableBroadcasting(bool bEnable = true);
    bool WriteBroadcast(void *pBufferToSend, size_t nBytesToSend, int &nSentBytes, IPPORT portNo);

        // Error codes
    //       bool IsEC_Unreach() { return GetLastErrorCode() == WSAEHOSTUNREACH; }
    //      bool IsEC_Timeout() { return GetLastErrorCode() == WSAETIMEDOUT; }
// Public overridables
    virtual bool ReadFrom(void *pBufferToFill, size_t nBufferSize, int &nReadBytes, IPADDRESS_TYPE &IpAddress, IPPORT &portNo);
    virtual bool WriteTo(void *pBufferToSend, size_t nBytesToSend, int &nSentBytes, IPADDRESS_TYPE IpAddress, IPPORT portNo);
    virtual bool WriteTo(void *pBufferToSend, size_t nBytesToSend, int &nSentBytes, char *pszIpAddress, IPPORT portNo);
// Public members

protected:
// Protected methods
    bool create(int af, int type, int protocol);
    bool SetOption(int level, int OptionName, char *pcValue, socklen_t OptionLength);
    bool GetOption(int level, int OptionName, char *pcValue, socklen_t &OptionLength);
    unsigned short CalcCheckSum(unsigned short *pData, unsigned short);
    unsigned short CalcPseudoCheckSum(SIpHeader *pIpHeader, unsigned short *pData, unsigned short);

// Protected overridables
    virtual bool ReadFrom(void *pBufferToFill, size_t nBufferSize, int &nReadBytes, struct sockaddr *pSrc);
  //virtual bool ReadFrom(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes, struct sockaddr_in *a_pSrc);
    virtual bool WriteTo(void *pBufferToSend, size_t nBytesToSend, int &nSentBytes, struct sockaddr *pDest);
    virtual void onBeforeClosing() {}

// Protected members
    SOCKET m_Socket;
    unsigned long m_LastErrorCode;
    unsigned int m_SocketSeqNumber; // numerates all sockets of this application
    static unsigned int m_SocketCount;
    static unsigned int m_MaxSocketSeqNumber;
    static bool bSocketsInitialized_;

    unsigned char m_cTTL;
    unsigned char m_cTOS;
};

// CUserSocket - UDP-stack and TCP-stack sockets classes will be derived from it

class UserSocket : public IpSocket
{
//Attributes
public:
    UserSocket();
    virtual ~UserSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
	virtual IPPORT getDefaultPort() { return 0; } // must be overriden
// Protected members
};


} // namespace network

} // namespace common

#endif // __IP_H_INCLUDED__
