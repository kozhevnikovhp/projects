// Ppd.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "Sockets.h"
#include "npStat\npStatLib.h"

const unsigned short PortNoIn=8201; // In IP port
const unsigned short PortNoOut=8201; // Out IP port
/* =========================================================================== */
/* forward declarations                                                        */
/* =========================================================================== */
int    prepStat(tlmHeader, char**);
//STATUS getConfigStat(npConfigSetting*);
void getModemStat(modemStatus*);
void getBstStat(bstStatus*);
//STATUS getSlotsStat(bckSlotsDistribution*);
void   getIpStat(ipStat*);
void   getUdpStat(udpStat*);
//void   getTcpStat(tcpStat*);
int    getDevAmount(void);
void   getDevStat(int, netDeviceData*);
//int    getRchParams(void);


int main(int argc, char* argv[])
{
	tlmHeader              npStatHdr;
	npConfigSetting        npConfigStat;
	modemStatus            npModemStat;
	bstStatus              npBstStat;
	bckSlotsDistribution   npSlotsStat;
	ipStat                 npIpStat;
	udpStat                npUdpStat;
	tcpStat                npTcpStat;
	netDeviceHdr           npDevHdr;
	netDeviceData          npDevStat;
	char *pNPStat = NULL;
	int npStatLen;

   int nPackets=0;
	int bSocketsOK=InitSockets();
	if (!bSocketsOK)
		return 1;

	// command reading loop
	struct sockaddr_in sock_from_param;
	memset(&sock_from_param, 0, sizeof(sock_from_param));
	SOCKET Socket=CreateUdpSocket(PortNoIn, &sock_from_param, TRUE);
	int fromlen=sizeof(sock_from_param);

	while (1)
	{
		int bytes=recvfrom(Socket, (char *)&npStatHdr, sizeof(npStatHdr), 0, (LPSOCKADDR)&sock_from_param, &fromlen);
		++nPackets;
		printf("Packet %d - %d bytes from %d.%d.%d.%d\n", nPackets, bytes,
					sock_from_param.sin_addr.S_un.S_un_b.s_b1,
					sock_from_param.sin_addr.S_un.S_un_b.s_b2,
					sock_from_param.sin_addr.S_un.S_un_b.s_b3,
					sock_from_param.sin_addr.S_un.S_un_b.s_b4);

		npStatLen = prepStat(npStatHdr, &pNPStat);
		Sleep(2000);
		bytes=sendto(Socket, pNPStat, npStatLen, 0, (LPSOCKADDR)&sock_from_param, fromlen);
        if (pNPStat)
		{
           free(pNPStat);
           pNPStat = NULL;
        }
	}
	
	FinitSockets();
	return 0;
}

int prepStat (tlmHeader npCommand, char **pBuffer)
{
   tlmHeader              npStatHdr;
   npConfigSetting        npConfigStat;
   modemStatus            npModemStat;
   bstStatus              npBstStat;
   bckSlotsDistribution   npSlotsStat;
   ipStat                 npIpStat;
   udpStat                npUdpStat;
   tcpStat                npTcpStat;
   netDeviceHdr           npDevHdr;
   netDeviceData          npDevStat;

   int                    i, j, bufLen=sizeof(tlmHeader);
   long                   npCmd;
   char *pBuf = NULL;
   char *currPos;

   memset (&npStatHdr, 0, sizeof(npStatHdr));
   npStatHdr.signature = htonl(SIGNATURE_FLAG);
   npStatHdr.serialNumber = npCommand.serialNumber;
   if ( (ntohl(npCommand.signature) != SIGNATURE_FLAG) &&
        (ntohs(npCommand.command) != CMD_TLM_GET) ) {

      npStatHdr.command = htons(CMD_TLM_STATUS_C_ERROR);

      if ((pBuf = (char *)malloc(sizeof(tlmHeader) ) ) == NULL)
	  {
         printf("malloc failed\n");
         return ( ERROR );
      }
      memset(pBuf, 0, sizeof (tlmHeader));
      memcpy (pBuf, &npStatHdr, sizeof(npStatHdr));

      return ( ERROR );
   }

   npCommand.tlmParamsMask = ntohl (npCommand.tlmParamsMask);

   for ( i=0; i < FLAGS_AMOUNT; i++ ) {

      npCmd = npCommand.tlmParamsMask & (NP_CONFIG_FLAG << i);

      switch ( npCmd ) {

/*         case NP_CONFIG_FLAG:
            memset (&npConfigStat, 0, sizeof(npConfigStat));
            getConfigStat(&npConfigStat);
            bufLen += sizeof(npConfigSetting);
            npStatHdr.tlmParamsMask |= NP_CONFIG_FLAG;
            break;*/
         case MODEM_PARAMS_FLAG:
            memset (&npModemStat, 0, sizeof(npModemStat));
			getModemStat(&npModemStat);
			bufLen += sizeof(modemStatus); 
			npStatHdr.tlmParamsMask |= MODEM_PARAMS_FLAG;
            break;
         case BST_PARAMETERS_FLAG:
			memset (&npBstStat, 0, sizeof(npBstStat));
			getBstStat(&npBstStat);
			bufLen += sizeof(bstStatus);
			npStatHdr.tlmParamsMask |= BST_PARAMETERS_FLAG;
            break;
/*         case BACKWARD_SLOTS_DISTR_FLAG:
            memset (&npSlotsStat, 0, sizeof(npSlotsStat));
            if ( getSlotsStat(&npSlotsStat) == OK ) {
               bufLen += sizeof(bckSlotsDistribution);
               npStatHdr.tlmParamsMask |= BACKWARD_SLOTS_DISTR_FLAG;
            }
            break; */
         case IP_STAT_FLAG:
            memset (&npIpStat, 0, sizeof(npIpStat));
            getIpStat (&npIpStat);
            bufLen += sizeof(npIpStat);
            npStatHdr.tlmParamsMask |= IP_STAT_FLAG;
            break;
         case UDP_STAT_FLAG:
            memset (&npUdpStat, 0, sizeof(npUdpStat));
            getUdpStat (&npUdpStat);
            bufLen += sizeof(npUdpStat);
            npStatHdr.tlmParamsMask |= UDP_STAT_FLAG;
            break;
/*         case TCP_STAT_FLAG:
            memset (&npTcpStat, 0, sizeof(npTcpStat));
            getTcpStat (&npTcpStat);
            bufLen += sizeof(npTcpStat);
            npStatHdr.tlmParamsMask |= TCP_STAT_FLAG;
            break;*/
         case NET_DEVICES_FLAG:
            memset (&npDevHdr, 0, sizeof(npDevHdr));
            if ( (npDevHdr.amount = getDevAmount()) > 0 ) {
               bufLen += sizeof(npDevHdr);
               bufLen += npDevHdr.amount * sizeof(npDevStat);
               npStatHdr.tlmParamsMask |= NET_DEVICES_FLAG;
            }
            break;
         default:
            break;
      }
   }

   /* Warnings mask initialization. */
/*   if (npConfigStat.modemUsage == (char) RCH_NO_TYPE) {
      npConfigStat.warningFlags |= WARNING_RCH_INIT_FAILED;
   }*/

/*   if (!(npConfigStat.flags & RCH_CATCH_ENABLE_FLAG) ||
       !(npConfigStat.flags & RCH_TX_SLOTS_ENABLE_FLAG)) {
      if ((npConfigStat.npType != RCH_NP_NES_DAMA_DUTY_TYPE) &&
          (npConfigStat.npWorkMode != STATUS_DAMA_HANDSHAKE)) {
         npConfigStat.warningFlags |= WARNING_CANT_TX_DATA_FLAG;
      }
   }*/

/*   if (npConfigStat.flags & BST_DEVICE_MGT_FLAG) {
      if (!(npBstStat.powerFlags & BST_TRANSMITTER_ON_FLAG) || 
          !(npBstStat.powerFlags & BST_RECEIVER_ON_FLAG)) {
         npConfigStat.warningFlags |= WARNING_BST_PARAMS_FLAG;
      }
   }*/

   if ((pBuf = (char *)malloc (bufLen)) == NULL) {
      printf("malloc failed\n");
      return ( ERROR );
   }
   memset(pBuf, 0, bufLen);
   currPos = pBuf;

   npStatHdr.command = htons(CMD_TLM_STATUS_OK);
   npStatHdr.tlmParamsMask = htonl(npStatHdr.tlmParamsMask);

   memcpy (currPos, &npStatHdr, sizeof(npStatHdr));
   currPos += sizeof(npStatHdr);

   npStatHdr.tlmParamsMask = ntohl (npStatHdr.tlmParamsMask);
   for ( i=0; i < FLAGS_AMOUNT; i++) {

      npCmd = npStatHdr.tlmParamsMask & (NP_CONFIG_FLAG << i);

      switch (npCmd)
	  {
         case NP_CONFIG_FLAG:
            memcpy (currPos, &npConfigStat, sizeof(npConfigStat));
            currPos += sizeof(npConfigStat);
            break;
         case MODEM_PARAMS_FLAG:
            memcpy (currPos, &npModemStat, sizeof(npModemStat));
            currPos += sizeof(npModemStat);
            break;
         case BST_PARAMETERS_FLAG:
            memcpy (currPos, &npBstStat, sizeof(npBstStat));
            currPos += sizeof(npBstStat);
            break;
         case BACKWARD_SLOTS_DISTR_FLAG:
            memcpy (currPos, &npSlotsStat, sizeof(npSlotsStat));
            currPos += sizeof(npSlotsStat);
            break;
         case IP_STAT_FLAG:
            memcpy (currPos, &npIpStat, sizeof(npIpStat));
            currPos += sizeof(npIpStat);
            break;
         case UDP_STAT_FLAG:
            memcpy (currPos, &npUdpStat, sizeof(npUdpStat));
            currPos += sizeof(npUdpStat);
            break;
         case TCP_STAT_FLAG:
            memcpy (currPos, &npTcpStat, sizeof(npTcpStat));
            currPos += sizeof(npTcpStat);
            break;
         case NET_DEVICES_FLAG:
            memcpy (currPos, &npDevHdr, sizeof(npDevHdr));
            currPos += sizeof(npDevHdr);
            for ( j = 0; j < npDevHdr.amount; j++ ) {
               memset (&npDevStat, 0, sizeof(npDevStat));
               getDevStat(j, &npDevStat);
               memcpy (currPos, &npDevStat, sizeof(npDevStat));
               currPos += sizeof(npDevStat);
            }
            break;
         default:
            break;
      }
   }

   *pBuffer = pBuf;
   return (bufLen);
} /* prepStat */

void getIpStat (ipStat* npIpStat)
{
	static int add=10;
   npIpStat->ips_total = htonl(1+add);
   npIpStat->ips_badsum = htonl(2+add);
   npIpStat->ips_tooshort = htonl(3+add);
   npIpStat->ips_toosmall = htonl(4+add);
   npIpStat->ips_badhlen = htonl(5+add);
   npIpStat->ips_badlen = htonl(6+add);
   npIpStat->ips_fragments = htonl(7+add);
   npIpStat->ips_fragdropped = htonl(8+add);
   npIpStat->ips_fragtimeout = htonl(9+add);
   npIpStat->ips_forward = htonl(10+add);
   npIpStat->ips_cantforward = htonl(11+add);
   npIpStat->ips_redirectsent = htonl(12+add);
   add+=5;
} /* getIpStat */

void getUdpStat (udpStat* npUdpStat)
{
 	static int add=100;
	  npUdpStat->udps_hdrops = htonl(1+add);
   npUdpStat->udps_badsum = htonl(2+add);
   npUdpStat->udps_badlen = htonl(3+add);
   npUdpStat->udps_ipackets = htonl(4+add);
   npUdpStat->udps_opackets = htonl(5+add);
   npUdpStat->udps_noportbcast = htonl(6+add);
   npUdpStat->udps_fullsock = htonl(7+add);
   add+=25;
} /* getUdpStat */

int getDevAmount (void)
{
   return (4);
} /* getDevAmount */

void getDevStat (int posValue, netDeviceData *npDevStat)
{
	static int add=20;
	switch (posValue)
	{
	case 0:
		strcpy(&npDevStat->if_params.data.if_name[0], "ln");
		break;
	case 1:
		sprintf(&npDevStat->if_params.data.if_name[0], "rch");
		break;
	case 2:
		sprintf(&npDevStat->if_params.data.if_name[0], "lo");
		break;
	default:
		sprintf(&npDevStat->if_params.data.if_name[0], "sss");
		break;
	};
   npDevStat->if_params.data.if_mtu = htons(256);
   npDevStat->if_ipackets = htonl(2+add+posValue);
   npDevStat->if_ierrors = htonl(3+add+posValue);
   npDevStat->if_opackets = htonl(4+add+posValue);
   npDevStat->if_oerrors = htonl(5+add+posValue);
   npDevStat->if_collisions = htonl(6+add+posValue);
   add+=2;
} /* getDevStat */

void getModemStat(modemStatus *npModemStat)
{
	static char add=1;
	npModemStat->ARUValue = add + 1;
	npModemStat->signalNoise = add + 2;
	npModemStat->flagBpch=add % 4;
	npModemStat->typeModem = add % 4;
	add++;
	add = add %250;
} /* getModemStat */

void getBstStat(bstStatus *npBstStat)
{
	static int add=10;
	npBstStat->ADCChannel = htonl(add + 1);
	npBstStat->ADCValueHex = htonl(add + 2);
	npBstStat->ADCValueMw = htonl(add + 4);
	npBstStat->powerFlags = htonl(add % 4);
	add += 39;
} /* getBstStat */

