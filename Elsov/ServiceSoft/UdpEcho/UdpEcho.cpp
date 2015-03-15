// UdpEcho.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "Rip.h"
#include "IcmpSockets.h"
#include "NetBiosSocket.h"
#include "SNMP_stuff.h"


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Not enough arguments\n");
		return 0;
	}

	int i1, i2, i3, i4;
	BYTE b1=0, b2=0, b3=0, b4=0;
	unsigned short wPort = 7;

	if (sscanf(argv[1], "%d.%d.%d.%d", &i1, &i2, &i3, &i4) != 4)
	{
		printf("Invalid address\n");
		return 0;
	}

	b1=LOBYTE(LOWORD(i1));
	b2=LOBYTE(LOWORD(i2));
	b3=LOBYTE(LOWORD(i3));
	b4=LOBYTE(LOWORD(i4));

	char pszAddr[128];

	sprintf(pszAddr, "%d.%d.%d.%d", b1, b2, b3, b4);
	IPADDRESS_TYPE uIP = StringToAddress(pszAddr);
	
	/*CRipSocket rip;
	rip.SendRipRequest(uIP);

	CUdpSocket sock;
	if (!sock.EnableBroadcasting())
	{
		printf("WARNING: Cannot set BROADCASTING = TRUE for UDP\n");
	}

	if (sock.SendBroadcastEchoRequest())
	{
		printf(" UDP Success\n");
	}
	else
	{
		printf(" UDP Error\n");
	}*/

	/*CIcmpSocket ping;
	if (!ping.EnableBroadcasting())
	{
		printf("WARNING: Cannot set BROADCASTING = TRUE for ICMP\n");
	}
	
	for (int i = 0; i < 3; i++)
	{
		if (ping.SendBroadcastEchoRequest())
			printf(" ICMP2 Success\n");
		else
			printf(" ICMP2 Error\n");
	}*/

	// Tracert
	/*CTracert tracert;
	if (!tracert.SetPingTimeout(3000))
		printf("cannot set timeout\n");
	SIpTraceRoute *pRoute = tracert.GetRoute(pszAddr);*/

/*	CNetBiosSocket netbios;
	netbios.SendStatRequest(inet_addr("192.168.31.76"), uIP);
	netbios.SendStatRequest(0, uIP);*/

/*	CRipSocket rip;
	BOOL b = rip.SendRipFullTableRequest(uIP);*/

	/*CIcmpSocket pinger;
	if (!pinger.EnableBroadcasting())
	{
		printf("WARNING: Cannot set BROADCASTING = TRUE for ICMP\n");
	}
	pinger.SendRouterSolicitation(uIP);
		unsigned long uEC = pinger.GetLastErrorCode();
		printf("Error code = %ld\n", uEC);*/

	CSnmpSocket snmp;
	snmp.SetReadTimeout(1000);
   // read_objid(".1.3.6.1.2.1.7.4.0", anOID, &anOID_len);
	unsigned int oid[] = { 0x01, 0x03, 0x06, 0x01, 0x02, 0x01, 0x07, 0x01, 0x00 };
//	unsigned int oid[] = { 0x01, 0x03, 124096, 0x01, 0x02, 0x01, 0x07, 0x04, 0x00 };
///	unsigned int oid[] = { 1,3,6,1,4,1,7840,3,1,2,2,2,2,1,1,1 };
	int oid_len = sizeof(oid)/sizeof(oid[0]);
	SSnmpReply reply;
	//snmp.SendSnmpSetUnsignedIntegerRequest(uIP, "private", oid, oid_len, 1, 129000);
	while (1)
	{
		snmp.SendSnmpGetRequest(uIP, "Zhenya", oid, oid_len, 1);
		if (snmp.GetSnmpReply(&reply))
		{
			if (reply.m_StatusCode == SNMP_ERROR_CODE_NO_SUCH_NAME)
				printf("No such name\n");
			switch (reply.m_type)
			{
			case SNMP_FIELD_TYPE_INTEGER:
				printf("= %d\n", reply.m_iIntegerValue);
				break;
			case SNMP_FIELD_TYPE_GAUGE:
			case SNMP_FIELD_TYPE_COUNTER32:
				printf("= %d\n", reply.m_uiIntegerValue);
				break;
			default:
				printf("field type = %d\n", reply.m_type);
			}
		}
		Sleep(500);
	}
	
	printf("press any key\n");
	getch();
	
	return 0;
}
