/*
 *  ip.h
 *
 *  Copyright (C) 2015-2018 Ooma Incorporated. All rights reserved.
 *
 */

#pragma once

typedef int SOCKET;
const int INVALID_SOCKET  = (~0);
const int SOCKET_ERROR    = (-1);
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

typedef uint32_t IPADDRESS_TYPE;
typedef unsigned short IPPORT;

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
    unsigned char	h_len:4;            // length of the header
    unsigned char	version:4;          // version of IP
    unsigned char	tos;                // type of service
    unsigned short	total_len;          // total length of packet
    unsigned short	ident;              // unique identifier

    unsigned char       frag_offset:5;      // Fragment offset field
    unsigned char       more_fragment :1;
    unsigned char       dont_fragment :1;
    unsigned char       reserved_zero :1;

    unsigned char       frag_offset1;       //fragment offset

    unsigned char	ttl;                // time to live value
    unsigned char	proto;              // protocol (TCP, UDP, etc.)
    unsigned short	checksum;           // IP checksum
    IPADDRESS_TYPE	sourceIP;           // source IP address
    IPADDRESS_TYPE	destIP;             // destination IP address

    unsigned short getHeaderLen() const { return h_len*4; }
    unsigned short getPacketLen() const { return ntohs(total_len); }
} SIpHeader;

typedef struct
{
    IPPORT srcPortNo;
    IPPORT dstPortNo;
    unsigned short length;
    unsigned short checksum;

    IPPORT getSrcPortNo() const { return ntohs(srcPortNo); }
    IPPORT getDstPortNo() const { return ntohs(dstPortNo); }
} SUdpHeader;

typedef struct
{
    IPPORT srcPortNo;
    IPPORT dstPortNo;

    int32_t seqNum; // sequence number - 32 bits
    int32_t ackNum; // acknowledgement number - 32 bits

    unsigned char ns:1; //Nonce Sum Flag Added in RFC 3540.
    unsigned char reserved_part1:3; //according to rfc
    unsigned char h_len:4; // length of the header

    unsigned char fin:1; //Finish Flag
    unsigned char syn:1; //Synchronise Flag
    unsigned char rst:1; //Reset Flag
    unsigned char psh:1; //Push Flag
    unsigned char ack:1; //Acknowledgement Flag
    unsigned char urg:1; //Urgent Flag

    unsigned char ecn:1; //ECN-Echo Flag
    unsigned char cwr:1; //Congestion Window Reduced Flag

    unsigned short window; // window
    unsigned short checksum; // checksum
    unsigned short urgent_pointer; // urgent pointer

    unsigned short getHeaderLen() const { return h_len*4; }
    IPPORT getSrcPortNo() const { return ntohs(srcPortNo); }
    IPPORT getDstPortNo() const { return ntohs(dstPortNo); }
    int32_t getSeqNum() const { return ntohl(seqNum); }
    int32_t getAckNum() const { return ntohl(ackNum); }
} STcpHeader;

struct SPseudoHeader
{
    IPADDRESS_TYPE src_addr; //
    IPADDRESS_TYPE dst_addr; //
    unsigned char zero ; //
    unsigned char proto; //
    unsigned short length; //
};

// ICMP header definition
typedef struct
{
    unsigned char   type;	// ICMP packet type
    unsigned char   code;	// type subcode
    unsigned short  checksum;	// packet checksum
    unsigned short  id;		// unique packet ID
    unsigned short  seq;	// packet sequence number
    int32_t         timestamp;  // timestamp
} SIcmpHeader;

// IGMP-protocol (RFC-1112, 2236, 3376)
typedef struct
{
    unsigned char type;
    unsigned char unused;
    unsigned short CRC;
    IPADDRESS_TYPE IP;      // Group address
} SIgmpHeader;

#pragma pack(pop)

