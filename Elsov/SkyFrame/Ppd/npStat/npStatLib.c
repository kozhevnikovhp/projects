/* *************************************************************************** */
/*                                                                             */
/*   npStatLib.c                                                               */
/*                                                                             */
/* *************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "vxWorks.h"
#include "sockLib.h"
#include "inetLib.h"
#include "selectLib.h"

#include "net/if.h"
#include "netinet/in.h"
#include "netinet/udp.h"
#include "netinet/tcp.h"
#include "netinet/ip_var.h"
#include "netinet/udp_var.h"
#include "netinet/tcp_var.h"
#include "tickLib.h"

#include "np3041.h"
#include "h/nvram.h"
#include "h/sysLib.h"
#include "h/bst/bstLib.h"
#include "h/rchServer/rchServer.h"
#include "h/npStat/npStatLib.h"
#include "h/config-ISA/devConfig.h"
#include "h/npRomIdentity.h"

extern struct ifnet   *ifnet;
extern struct ipstat  ipstat;
IMPORT struct udpstat udpstat;
IMPORT struct tcpstat tcpstat;

/* =========================================================================== */
/* forward declarations                                                        */
/* =========================================================================== */
STATUS npStatServer (void);   
LOCAL int    prepStat(tlmHeader, char**);
LOCAL STATUS getConfigStat(npConfigSetting*);
LOCAL STATUS getModemStat(modemStatus*);
LOCAL STATUS getBstStat(bstStatus*);
LOCAL STATUS getSlotsStat(bckSlotsDistribution*);
LOCAL void   getIpStat(ipStat*);
LOCAL void   getUdpStat(udpStat*);
LOCAL void   getTcpStat(tcpStat*);
LOCAL int    getDevAmount(void);
LOCAL void   getDevStat(int, netDeviceData*);
LOCAL int    getRchParams(void);

LOCAL int    npType;

IMPORT BOOL  flgSetWorks;


extern STATUS rchStatInfoGet(char*, long*, modemStatus*);
extern void   mstProgVersionGet (char *buf);
/* =========================================================================== */
/*                                                                             */
/*    npStatServer -                                                           */
/*                                                                             */
/* =========================================================================== */
STATUS npStatServer (void)
{
   struct sockaddr_in    serverAddr;
   struct sockaddr_in    clientAddr;
   struct fd_set         rdReady;
   struct timeval        timeout;
   int                   clientAddrLen;
   int                   sockfd = ERROR;
   int                   optVal;
   int                   nfound;
   int                   len = 0;

   char*                 pNPStat = NULL;
   int                   npStatLen;
   tlmHeader             npStatCmd;

#if 1
   char                  ipAddr [18];
#endif

   npType = getRchParams();

   if ((sockfd = socket (AF_INET, SOCK_DGRAM, 0)) == ERROR) {
      sysPrintError ("socket", NULL);
      return ERROR;
   }

   optVal = 1;
   if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR,
                   (char *) &optVal, sizeof(optVal)) == ERROR) {
      sysPrintError ("setsockopt", "SO_REUSEADDR");
      return (ERROR);
   }

   memset ((char *) &serverAddr, 0, sizeof(serverAddr));
   serverAddr.sin_family       = AF_INET;
   serverAddr.sin_addr. s_addr = htonl (INADDR_ANY);
   serverAddr.sin_port         = htons (NP_TELEMETRY_UDP_PORT_NUMBER);

   if (bind (sockfd, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) == ERROR) {
      sysPrintError ("bind", NULL);
      closeSocket(&sockfd);
      return ERROR;
   }

   while(1) {
      	timeout.tv_sec  = 0;
        timeout.tv_usec = 500000;

        nfound = 0;
        FD_ZERO (&rdReady);
        FD_SET (sockfd, &rdReady);

        nfound = select (sockfd + 1, &rdReady, 0, 0, NULL);

        if ( nfound == ERROR ) {
            closeSocket(&sockfd);
            sysPrintError ("nfound", NULL);
            return ERROR;
        }

        if ( nfound > 0 ) {
           memset ((char *) &clientAddr, 0, sizeof(clientAddr));
           clientAddrLen = sizeof(clientAddr);
           if ((len = recvfrom (sockfd, (char*) &npStatCmd, sizeof (npStatCmd), 0,
                                (struct sockaddr *) &clientAddr,
                                &clientAddrLen)) == ERROR) {
              sysPrintError ("recvfrom", NULL);
              closeSocket(&sockfd);
              return (ERROR);
           }
        }

        npStatLen = prepStat(npStatCmd, &pNPStat);

#if 0
        printf("Outing from prepStat\n");
        printf("Sending signature %ld\n", ntohl ( ((tlmHeader*) pNPStat)->signature ) );
        printf("Sending serialNumber %d\n", ((tlmHeader*) pNPStat)->serialNumber);
        printf("Sending tlmParamsMask %ld\n", ntohl ( ((tlmHeader*) pNPStat)->tlmParamsMask ) );
        printf("\n");
#endif

        if ((len = sendto (sockfd, pNPStat, npStatLen, 0,
                           (struct sockaddr *) &clientAddr,
                           sizeof(struct sockaddr_in))) == ERROR) {
           sysPrintError ("sendto", NULL);
           closeSocket(&sockfd);
           return (ERROR);
        }

        if (pNPStat != NULL) {
           free(pNPStat);
           pNPStat = NULL;
        }
#if 1
        inet_ntoa_b (clientAddr.sin_addr, ipAddr);
        printf("send to client %s/%d %d bytes\n", ipAddr, ntohs(clientAddr.sin_port), len);
#endif
   }
} /* npStatServer */

/*=============================================================================*/
/*                                                                             */
/*   prepStat -                                                                */
/*                                                                             */
/*=============================================================================*/
LOCAL int prepStat (tlmHeader npCommand, char** pBuffer)
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

   int                    i, j, bufLen;
   long                   npCmd;
   char*                  pBuf = NULL;
   char*                  currPos;

   memset (&npStatHdr, 0, sizeof(npStatHdr));
   npStatHdr.signature = htonl(SIGNATURE_FLAG);
   npStatHdr.serialNumber = npCommand.serialNumber;
   if ( (ntohl(npCommand.signature) != SIGNATURE_FLAG) &&
        (ntohs(npCommand.command) != CMD_TLM_GET) ) {

      npStatHdr.command = htons(CMD_TLM_STATUS_C_ERROR);

      if ( ( pBuf = malloc (sizeof (tlmHeader) ) ) == NULL) {
         sysPrintError ("malloc", NULL);
         return ( ERROR );
      }
      memset(pBuf, 0, sizeof (tlmHeader));
      memcpy (pBuf, &npStatHdr, sizeof(npStatHdr));

      return ( ERROR );
   }

#if 0
   printf("Recieved signature %ld/%ld\n", npCommand.signature, ntohl(npCommand.signature));
   printf("Recieved serialNumber %d/%d\n", npCommand.serialNumber, ntohl(npCommand.serialNumber));
   printf("Recieved tlmParamsMask %ld/%ld\n", npCommand.tlmParamsMask, ntohl(npCommand.tlmParamsMask));
   printf("\n");
#endif

   bufLen = sizeof (tlmHeader);
   npCommand.tlmParamsMask = ntohl (npCommand.tlmParamsMask);

   for ( i=0; i < FLAGS_AMOUNT; i++ ) {

      npCmd = npCommand.tlmParamsMask & (NP_CONFIG_FLAG << i);

      switch ( npCmd ) {

         case NP_CONFIG_FLAG:
            memset (&npConfigStat, 0, sizeof(npConfigStat));
            getConfigStat(&npConfigStat);
            bufLen += sizeof(npConfigSetting);
            npStatHdr.tlmParamsMask |= NP_CONFIG_FLAG;
            break;
         case MODEM_PARAMS_FLAG:
            memset (&npModemStat, 0, sizeof(npModemStat));
            if ( getModemStat(&npModemStat) == OK ) {
               bufLen += sizeof(modemStatus);

#if 0
        printf("Sending signalNoise %i\n", npModemStat.signalNoise );
        printf("Sending ARUValue %i\n", npModemStat.ARUValue );
        printf("Sending flagBpch %i\n", npModemStat.flagBpch );
        printf("Sending typeModem %i\n", npModemStat.typeModem );
        printf("\n");
#endif

               npStatHdr.tlmParamsMask |= MODEM_PARAMS_FLAG;
            }
            break;
         case BST_PARAMETERS_FLAG:
            memset (&npBstStat, 0, sizeof(npBstStat));
            if ( getBstStat(&npBstStat) == OK ) {
               bufLen += sizeof(bstStatus);
               npStatHdr.tlmParamsMask |= BST_PARAMETERS_FLAG;
            }
            break;
         case BACKWARD_SLOTS_DISTR_FLAG:
            memset (&npSlotsStat, 0, sizeof(npSlotsStat));
            if ( getSlotsStat(&npSlotsStat) == OK ) {
               bufLen += sizeof(bckSlotsDistribution);
               npStatHdr.tlmParamsMask |= BACKWARD_SLOTS_DISTR_FLAG;
            }
            break;
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
         case TCP_STAT_FLAG:
            memset (&npTcpStat, 0, sizeof(npTcpStat));
            getTcpStat (&npTcpStat);
            bufLen += sizeof(npTcpStat);
            npStatHdr.tlmParamsMask |= TCP_STAT_FLAG;
            break;
         case NET_DEVICES_FLAG:
            memset (&npDevHdr, 0, sizeof(npDevHdr));
            if ( ( npDevHdr.amount = getDevAmount() ) > 0 ) {
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
   if (npConfigStat.modemUsage == (char) RCH_NO_TYPE) {
      npConfigStat.warningFlags |= WARNING_RCH_INIT_FAILED;
   }

   if (!(npConfigStat.flags & RCH_CATCH_ENABLE_FLAG) ||
       !(npConfigStat.flags & RCH_TX_SLOTS_ENABLE_FLAG)) {
      if ((npConfigStat.npType != RCH_NP_NES_DAMA_DUTY_TYPE) &&
          (npConfigStat.npWorkMode != STATUS_DAMA_HANDSHAKE)) {
         npConfigStat.warningFlags |= WARNING_CANT_TX_DATA_FLAG;
      }
   }

   if (npConfigStat.flags & BST_DEVICE_MGT_FLAG) {
      if (!(npBstStat.powerFlags & BST_TRANSMITTER_ON_FLAG) || 
          !(npBstStat.powerFlags & BST_RECEIVER_ON_FLAG)) {
         npConfigStat.warningFlags |= WARNING_BST_PARAMS_FLAG;
      }
   }

   if ((pBuf = malloc (bufLen)) == NULL) {
      sysPrintError ("malloc", NULL);
      return ( ERROR );
   }
   memset(pBuf, 0, bufLen);
   currPos = pBuf;

   npStatHdr.command = htons(CMD_TLM_STATUS_OK);
   npStatHdr.tlmParamsMask = htonl (npStatHdr.tlmParamsMask);

   memcpy (currPos, &npStatHdr, sizeof(npStatHdr));
   currPos += sizeof(npStatHdr);

#if 0
   printf("Sending signature %ld\n", ntohl ( ((tlmHeader*) currPos)->signature ) );
   printf("Sending serialNumber %d\n", ((tlmHeader*) currPos)->serialNumber);
   printf("Sending tlmParamsMask %ld\n", ntohl ( ((tlmHeader*) currPos)->tlmParamsMask ) );
   printf("\n");
#endif

   npStatHdr.tlmParamsMask = ntohl (npStatHdr.tlmParamsMask);
   for ( i=0; i < FLAGS_AMOUNT; i++) {

      npCmd = npStatHdr.tlmParamsMask & (NP_CONFIG_FLAG << i);

      switch (npCmd) {

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
   return ( bufLen );
} /* prepStat */

/*=============================================================================*/
/*                                                                             */
/*   getConfigStat -                                                           */
/*                                                                             */
/*=============================================================================*/
LOCAL STATUS getConfigStat (npConfigSetting* npConfigStat)
{
   npRomVersion  version;

   npConfigStat->npType = (char) npType;
   npConfigStat->npWorkMode = (short) getRchServerState(NULL, NULL, NULL);
   npConfigStat->workTime = tickGet();
   
   mstProgVersionGet ((char *) &version);
   npConfigStat->npRomVersion = version.progVersion;
 
   if (flgSetWorks == FALSE) {
      npConfigStat->flags |= NP_IS_IN_TEST_MODE_FLAG;
   }
 
   /* Additional rch parameters and flags. */
   if ( rchStatInfoGet(&npConfigStat->modemUsage, &npConfigStat->flags, NULL) == ERROR ) {
      npConfigStat->modemUsage = -1;
   }

   return ( OK );
} /* getConfigStat */

/*=============================================================================*/
/*                                                                             */
/*   getModemStat -                                                            */
/*                                                                             */
/*=============================================================================*/
LOCAL STATUS getModemStat (modemStatus* npModemStat)
{
   if ( rchStatInfoGet(NULL, NULL, npModemStat) == OK ) {
      return ( OK );
   }

   return ( ERROR );
} /* getModemStat */

/*=============================================================================*/
/*                                                                             */
/*   getBstStat -                                                              */
/*                                                                             */
/*=============================================================================*/
LOCAL STATUS getBstStat (bstStatus* npBstStat)
{
   if (bstInit () == OK) {

      if ( bstCheckTransmitPowerOn() == OK ) {
         npBstStat->powerFlags |= BST_TRANSMITTER_ON_FLAG;
      }

      if ( bstCheckReceivPowerOn() == OK ) {
         npBstStat->powerFlags |= BST_RECEIVER_ON_FLAG;
      }

      if ( bstGetPowerTlmData() == OK ) {
         npBstStat->ADCChannel = bstPowerTlmCh;
         npBstStat->ADCValueHex = bstPowerTlmData;
         if ( bstGetPowerTlmDataMvt() == OK ) {
            npBstStat->ADCValueMw = bstPowerTlmDataMvt;
         }
      }

      return ( OK );
   }

   return ( ERROR );
} /* getBstStat */

/*=============================================================================*/
/*                                                                             */
/*   getSlotsStat -                                                            */
/*                                                                             */
/*=============================================================================*/
LOCAL STATUS getSlotsStat (bckSlotsDistribution* npSlotsStat)
{

   return ( ERROR );
} /* getSlotsStat */

/*=============================================================================*/
/*                                                                             */
/*   getIpStat -                                                               */
/*                                                                             */
/*=============================================================================*/
LOCAL void getIpStat (ipStat* npIpStat)
{
   npIpStat->ips_total = htonl (ipstat.ips_total);
   npIpStat->ips_badsum = htonl (ipstat.ips_badsum);
   npIpStat->ips_tooshort = htonl (ipstat.ips_tooshort);
   npIpStat->ips_toosmall = htonl (ipstat.ips_toosmall);
   npIpStat->ips_badhlen = htonl (ipstat.ips_badhlen);
   npIpStat->ips_badlen = htonl (ipstat.ips_badlen);
   npIpStat->ips_fragments = htonl (ipstat.ips_fragments);
   npIpStat->ips_fragdropped = htonl (ipstat.ips_fragdropped);
   npIpStat->ips_fragtimeout = htonl (ipstat.ips_fragtimeout);
   npIpStat->ips_forward = htonl (ipstat.ips_forward);
   npIpStat->ips_cantforward = htonl (ipstat.ips_cantforward);
   npIpStat->ips_redirectsent = htonl (ipstat.ips_redirectsent);
} /* getIpStat */

/*=============================================================================*/
/*                                                                             */
/*   getUdpStat -                                                              */
/*                                                                             */
/*=============================================================================*/
LOCAL void getUdpStat (udpStat* npUdpStat)
{
   npUdpStat->udps_hdrops = udpstat.udps_hdrops;
   npUdpStat->udps_badsum = udpstat.udps_badsum;
   npUdpStat->udps_badlen = udpstat.udps_badlen;
   npUdpStat->udps_ipackets = udpstat.udps_ipackets;
   npUdpStat->udps_opackets = udpstat.udps_opackets;
   npUdpStat->udps_noportbcast = udpstat.udps_noportbcast;
   npUdpStat->udps_fullsock = udpstat.udps_fullsock;
} /* getUdpStat */

/*=============================================================================*/
/*                                                                             */
/*   getTcpStat -                                                              */
/*                                                                             */
/*=============================================================================*/
LOCAL void getTcpStat (tcpStat* npTcpStat)
{
   npTcpStat->tcps_connattempt = htonl (tcpstat.tcps_connattempt);
   npTcpStat->tcps_accepts = htonl (tcpstat.tcps_accepts);
   npTcpStat->tcps_connects = htonl (tcpstat.tcps_connects);
   npTcpStat->tcps_drops = htonl (tcpstat.tcps_drops);
   npTcpStat->tcps_conndrops = htonl (tcpstat.tcps_conndrops);
   npTcpStat->tcps_closed = htonl (tcpstat.tcps_closed);
   npTcpStat->tcps_segstimed = htonl (tcpstat.tcps_segstimed);
   npTcpStat->tcps_rttupdated = htonl (tcpstat.tcps_rttupdated);
   npTcpStat->tcps_delack = htonl (tcpstat.tcps_delack);
   npTcpStat->tcps_timeoutdrop = htonl (tcpstat.tcps_timeoutdrop);
   npTcpStat->tcps_rexmttimeo = htonl (tcpstat.tcps_rexmttimeo);
   npTcpStat->tcps_persisttimeo = htonl (tcpstat.tcps_persisttimeo);
   npTcpStat->tcps_keeptimeo = htonl (tcpstat.tcps_keeptimeo);
   npTcpStat->tcps_keepprobe = htonl (tcpstat.tcps_keepprobe);
   npTcpStat->tcps_keepdrops = htonl (tcpstat.tcps_keepdrops);

   npTcpStat->tcps_sndtotal = htonl (tcpstat.tcps_sndtotal);
   npTcpStat->tcps_sndpack = htonl (tcpstat.tcps_sndpack);
   npTcpStat->tcps_sndbyte = htonl (tcpstat.tcps_sndbyte);
   npTcpStat->tcps_sndrexmitpack = htonl (tcpstat.tcps_sndrexmitpack);
   npTcpStat->tcps_sndrexmitbyte = htonl (tcpstat.tcps_sndrexmitbyte);
   npTcpStat->tcps_sndacks = htonl (tcpstat.tcps_sndacks);
   npTcpStat->tcps_sndprobe = htonl (tcpstat.tcps_sndprobe);
   npTcpStat->tcps_sndurg = htonl (tcpstat.tcps_sndurg);
   npTcpStat->tcps_sndwinup = htonl (tcpstat.tcps_sndwinup);
   npTcpStat->tcps_sndctrl = htonl (tcpstat.tcps_sndctrl);
   npTcpStat->tcps_rcvtotal = htonl (tcpstat.tcps_rcvtotal);
   npTcpStat->tcps_rcvpack = htonl (tcpstat.tcps_rcvpack);
   npTcpStat->tcps_rcvbyte = htonl (tcpstat.tcps_rcvbyte);

   npTcpStat->tcps_rcvbadsum = htonl (tcpstat.tcps_rcvbadsum);
   npTcpStat->tcps_rcvbadoff = htonl (tcpstat.tcps_rcvbadoff);
   npTcpStat->tcps_rcvshort = htonl (tcpstat.tcps_rcvshort);

   npTcpStat->tcps_rcvduppack = htonl (tcpstat.tcps_rcvduppack);
   npTcpStat->tcps_rcvdupbyte = htonl (tcpstat.tcps_rcvdupbyte);
   npTcpStat->tcps_rcvpartduppack = htonl (tcpstat.tcps_rcvpartduppack);
   npTcpStat->tcps_rcvpartdupbyte = htonl (tcpstat.tcps_rcvpartdupbyte);
   npTcpStat->tcps_rcvoopack = htonl (tcpstat.tcps_rcvoopack);
   npTcpStat->tcps_rcvoobyte = htonl (tcpstat.tcps_rcvoobyte);
   npTcpStat->tcps_rcvpackafterwin = htonl (tcpstat.tcps_rcvpackafterwin);
   npTcpStat->tcps_rcvbyteafterwin = htonl (tcpstat.tcps_rcvbyteafterwin);
   npTcpStat->tcps_rcvafterclose = htonl (tcpstat.tcps_rcvafterclose);
   npTcpStat->tcps_rcvwinprobe = htonl (tcpstat.tcps_rcvwinprobe);
   npTcpStat->tcps_rcvdupack = htonl (tcpstat.tcps_rcvdupack);
   npTcpStat->tcps_rcvacktoomuch = htonl (tcpstat.tcps_rcvacktoomuch);
   npTcpStat->tcps_rcvackpack = htonl (tcpstat.tcps_rcvackpack);
   npTcpStat->tcps_rcvackbyte = htonl (tcpstat.tcps_rcvackbyte);
   npTcpStat->tcps_rcvwinupd = htonl (tcpstat.tcps_rcvwinupd);
} /* getTcpStat */

/*=============================================================================*/
/*                                                                             */
/*   getDevHdr -                                                               */
/*                                                                             */
/*=============================================================================*/
LOCAL int getDevAmount (void)
{
   int amount;
   struct ifnet *ptr_next;

   ptr_next = ifnet;
   amount = 0;
   while ( ptr_next != NULL ) {
      amount ++;
      ptr_next = ptr_next->if_next;
   }

   return ( amount );
} /* getDevAmount */

/*=============================================================================*/
/*                                                                             */
/*   getDevStat -                                                              */
/*                                                                             */
/*=============================================================================*/
LOCAL void getDevStat (int posValue, netDeviceData* npDevStat)
{
   int i;
   struct ifnet *ptr_next;

   ptr_next = ifnet;
   for (i = 0; i < posValue; i++) {
      ptr_next = ptr_next->if_next;
   }

   strcpy(&npDevStat->if_params.data.if_name[0], ptr_next->if_name);
   npDevStat->if_params.data.if_mtu = ptr_next->if_mtu;
   npDevStat->if_ipackets = ptr_next->if_ipackets;
   npDevStat->if_ierrors = ptr_next->if_ierrors;
   npDevStat->if_opackets = ptr_next->if_opackets;
   npDevStat->if_oerrors = ptr_next->if_oerrors;
   npDevStat->if_collisions = ptr_next->if_collisions;
} /* getDevStat */

/*=============================================================================*/
/*                                                                             */
/*   getRchParams -                                                            */
/*                                                                             */
/*=============================================================================*/
LOCAL int getRchParams(void)
{
   int            i;
   int            chUsage;
   devConfig      rmConfig;
#if (NVRAM_H_VER >= 50)
   rchParameters  rchParms;
#else
   rchParameters  rchParms;
#endif /* NVRAM_H_VER */
   
   for (i = 0; i < ISA_NUMBER_OF_SLOTS; i ++) {
      if (getDevConfig (i, DEV_RM, &rmConfig) == OK) {
#if (NVRAM_H_VER >= 50)
         if ((sysNvRamGet ((char*) &rchParms, sizeof(rchParms), NV_RCH_PARMS_OFFSET + (i * sizeof(rchParms))) == OK)) {
            chUsage = rchParms.usage;
            return chUsage;
         }
#else
         if (sysNvRamGet ((char*) &rchParms, NV_RCH_PARMS_SIZE, NV_RCH_PARMS_OFFSET) == OK) {
            chUsage = npIdent.type & 0x0000007F;
            return chUsage;
         }
#endif /* NVRAM_H_VER */
      }
   }

   return -1;   
} /* getRchParams */
