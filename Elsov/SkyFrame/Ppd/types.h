#ifndef __IP_STAT_H_INCLUDED__
#define __IP_STAT_H_INCLUDED__


#define NP_TELEMETRY_UDP_PORT_NUMBER   8201
/*#define ALIGNMENT_8*/

/* ====================================================================== */
/*  main struct for answer to client                                      */
/* ====================================================================== */
typedef
   struct {
      long               signature;       /* Dec = 31415926 */
      short              serialNumber;     /* serial number */
      short              command;         /* Status of reading the parameters (0-Ok, otherwise - code of error of reading the parameters) */
      long               tlmParamsMask;   /* Statistic mask */

#ifdef ALIGNMENT_8
      long               reserved;
#endif /* ALIGNMENT_8 */

   } tlmHeader;

/* ====================================================================== */
/*  Flags for tlmHeader.tlmParamsMask                                     */
/* ====================================================================== */
#define   FLAGS_AMOUNT               8
#define   SIGNATURE_FLAG             31415926

#define   NP_CONFIG_FLAG             0x00000001
#define   MODEM_PARAMS_FLAG          0x00000002
#define   BST_PARAMETERS_FLAG        0x00000004
#define   BACKWARD_SLOTS_DISTR_FLAG  0x00000008
#define   IP_STAT_FLAG               0x00000010
#define   UDP_STAT_FLAG              0x00000020
#define   TCP_STAT_FLAG              0x00000040
#define   NET_DEVICES_FLAG           0x00000080

/* ====================================================================== */
/*  NP_CONFIG_FLAG for tlmHeader.tlmParamsMask                            */
/* ====================================================================== */
typedef
   struct {
      char     npType;         /* NP type */
      char     modemUsage;     /* Radio modem type (SST TDMA, SST PAMA, NES TDMA) */
      short    npWorkMode;     /* reserved */
      long     flags;          /* flags */
      long     warningFlags;   /* if warningFlags !=0 - error */
      long     npRomVersion;   /* NP ROM version */
      long     workTime;       /* work time */

#ifdef ALIGNMENT_8
      long     reserved;
#endif /* ALIGNMENT_8 */

   } npConfigSetting;

/* ---------------------------------------------------------------------- */
/*  NP current work mode for npConfigStructure.npWorkMode                 */
/* ---------------------------------------------------------------------- */
#ifndef _INC_rchServer_h
enum {
   STATUS_DAMA_IDLE,                  /* DAMA: np is not used (idle) */
   STATUS_DAMA_ACTIVE,                /* DAMA: np is conntected to remote station */
   STATUS_DAMA_DUTY_ON,               /* DAMA: np requests recources */
   STATUS_DAMA_HANDSHAKE,             /* DAMA: np is making connection with remote station */
   STATUS_DAMA_DISCONNECTION,         /* !!! DAMA: np is disconnecting */
   STATUS_DAMA_WAIT_ON_TIMEOUT,       /* DAMA: np requests recources */
   STATUS_LEASED_LINE_HANDSHAKE,      /* !!! PAMA: np is making connection with remote station */
   STATUS_LEASED_LINE_ACTIVE,         /* PAMA: np is connected */
   STATUS_DAMA_DUTY,                  /* DAMA: CES duty channel */
   STATUS_TDMA_CES_NODE,              /* TDMA: TDMA hub active */
   STATUS_TDMA_NES_NODE,              /* TDMA: TDMA hub active */
   STATUS_TDMA_SST_NODE,              /* TDMA: TDMA sst */

   STATUS_TDMA_CES_NODE_INIT,         /* !!! TDMA: TDMA hub initialization */
   STATUS_TDMA_NES_NODE_INIT          /* !!! TDMA: TDMA hub initialization */
};
#endif


#ifndef _INC_nvram_h
/* ---------------------------------------------------------------------- */
/*  npType values                                                         */
/* ---------------------------------------------------------------------- */
#define NP_SST_PAMA_TYPE		1
#define NP_SST_DAMA_TYPE		2
#define NP_SST_TDMA_TYPE		3
#define NP_NES_PAMA_TYPE		4
#define NP_NES_DAMA_TYPE		5
#define NP_NES_DAMA_DUTY_TYPE		6
#define NP_NES_TDMA_TYPE		7
#define NP_CES_TDMA_TYPE		8

#define RCH_NP_SST_PAMA_TYPE            NP_SST_PAMA_TYPE
#define RCH_NP_SST_DAMA_TYPE            NP_SST_DAMA_TYPE
#define RCH_NP_SST_TDMA_TYPE            NP_SST_TDMA_TYPE
#define RCH_NP_NES_PAMA_TYPE            NP_NES_PAMA_TYPE
#define RCH_NP_NES_DAMA_TYPE            NP_NES_DAMA_TYPE
#define RCH_NP_NES_DAMA_DUTY_TYPE       NP_NES_DAMA_DUTY_TYPE
#define RCH_NP_NES_TDMA_TYPE            NP_NES_TDMA_TYPE
#define RCH_NP_CES_TDMA_TYPE            NP_CES_TDMA_TYPE


#endif /* _INC_nvram_h */

/* ---------------------------------------------------------------------- */
/*  modemUsage                                                            */
/* ---------------------------------------------------------------------- */
#define RCH_SST_TYPE           1 /* Small Satellite Terminal */
#define RCH_CES_TYPE           2 /* Central Earthen Station */
#define RCH_SST2SST_TYPE       3 /* SST<->SST point-to-point */
#define RCH_NO_TYPE           -1


/* ---------------------------------------------------------------------- */
/* flags for npConfigStructure.flags                                      */
/* ---------------------------------------------------------------------- */
#define BST_DEVICE_MGT_FLAG         0x00001  /* BST device management */
#define RCH_CATCH_ENABLE_FLAG       0x00002  /* Catch detected */
#define RCH_TX_SLOTS_ENABLE_FLAG    0x00004  /* Tx slots enabled */
#define RCH_TRANSMITTER_IS_ON_FLAG  0x00008  /* Radio modem transmitter is ON */
#define NP_CONSOLE_CONNECTED_FLAG   0x00010  /* NP console connected */
#define NP_IS_IN_TEST_MODE_FLAG     0x00020  /* NP is in test mode */

/* ---------------------------------------------------------------------- */
/* flags for npConfigStructure.warningFlags                               */
/* ---------------------------------------------------------------------- */
#define WARNING_CANT_TX_DATA_FLAG   0x00000001     /* Can't transmit data */
#define WARNING_BST_PARAMS_FLAG     0x00000002     /* Error in BST device */
#define WARNING_NON_ACTIVE_FLAG     0x00000004     /* NP status */
#define WARNING_RCH_INIT_FAILED     0x00000008     /* Radio Modem initialization failed */

/* ====================================================================== */
/* MODEM_PARAMS_FLAG for tlmHeader.tlmParamsMask                          */
/* ====================================================================== */
#define FLAG_BPCH_RX   0x01
#define FLAG_BPCH_TX   0x02

typedef
   struct {
      char  signalNoise;  /* signal to noise ratio */
      char  ARUValue;     /* input signal level */
      char  flagBpch;     /*  */
      char  typeModem;    /*  */

#ifdef ALIGNMENT_8
      long    reserved;
#endif /* ALIGNMENT_8 */

   } modemStatus;

/* ====================================================================== */
/* BST_PARAMETERS_FLAG for tlmHeader.tlmParamsMask                        */
/* ====================================================================== */
#define BST_TRANSMITTER_ON_FLAG   0x00000001
#define BST_RECEIVER_ON_FLAG      0x00000002

typedef
   struct {
      int   powerFlags;     /* Transmitter, Receiver On/Off */
      int   ADCChannel;     /* ADC Channel */
      int   ADCValueHex;    /* ADC Value Hex format */
      int   ADCValueMw;     /* ADC Value mW format */
   } bstStatus;

/* ====================================================================== */
/* BACKWARD_SLOTS_DISTR_FLAG for tlmHeader.tlmParamsMask                  */
/* Backward channel slots ditribution (for DAMA dut and TDMA hub only)    */
/* ====================================================================== */
typedef 
   struct {
      USHORT  slots[36];

#ifdef ALIGNMENT_8
      long    reserved;
#endif /* ALIGNMENT_8 */

} bckSlotsDistribution;

/* ====================================================================== */
/* IP_STAT_FLAG for tlmHeader.tlmParamsMask                               */
/* ====================================================================== */
typedef
   struct {
      long   ips_total;          /* total packets received */
      long   ips_badsum;         /* checksum bad */
      long   ips_tooshort;       /* packet too short */
      long   ips_toosmall;       /* not enough data */
      long   ips_badhlen;        /* ip header length < data size */
      long   ips_badlen;         /* ip length < ip header length */
      long   ips_fragments;      /* fragments received */
      long   ips_fragdropped;    /* frags dropped (dups, out of space) */
      long   ips_fragtimeout;    /* fragments timed out */
      long   ips_forward;	 /* packets forwarded */
      long   ips_cantforward;    /* packets rcvd for unreachable dest */
      long   ips_redirectsent;   /* packets forwarded on same net */

#ifdef ALIGNMENT_8
      long   reserved;
#endif /* ALIGNMENT_8 */

   } ipStat;

/* ====================================================================== */
/* UDP_STAT_FLAG for tlmHeader.tlmParamsMask                              */
/* ====================================================================== */
typedef
   struct {
      int	udps_hdrops;        /* bad header length */
      int	udps_badsum;        /* bad header checksum */
      int	udps_badlen;        /* bad packet length */
      int	udps_ipackets;      /* total packets received, including errors */
      int	udps_opackets;      /* total packets sent */
      int	udps_noportbcast;   /* broadcast packets, no port */
      int	udps_fullsock;      /* packets dropped socket recv side is full */
   } udpStat;

/* ====================================================================== */
/* TCP_STAT_FLAG for tlmHeader.tlmParamsMask                              */
/* ====================================================================== */
typedef
   struct {
      u_long  tcps_connattempt;       /* connections initiated */
      u_long  tcps_accepts;           /* connections accepted */
      u_long  tcps_connects;          /* connections established */
      u_long  tcps_drops;             /* connections dropped */
      u_long  tcps_conndrops;         /* embryonic connections dropped */
      u_long  tcps_closed;            /* conn. closed (includes drops) */
      u_long  tcps_segstimed;         /* segs where we tried to get rtt */
      u_long  tcps_rttupdated;        /* times we succeeded */
      u_long  tcps_delack;            /* delayed acks sent */
      u_long  tcps_timeoutdrop;       /* conn. dropped in rxmt timeout */
      u_long  tcps_rexmttimeo;        /* retransmit timeouts */
      u_long  tcps_persisttimeo;      /* persist timeouts */
      u_long  tcps_keeptimeo;         /* keepalive timeouts */
      u_long  tcps_keepprobe;         /* keepalive probes sent */
      u_long  tcps_keepdrops;         /* connections dropped in keepalive */

      u_long  tcps_sndtotal;          /* total packets sent */
      u_long  tcps_sndpack;           /* data packets sent */
      u_long  tcps_sndbyte;           /* data bytes sent */
      u_long  tcps_sndrexmitpack;     /* data packets retransmitted */
      u_long  tcps_sndrexmitbyte;     /* data bytes retransmitted */
      u_long  tcps_sndacks;           /* ack-only packets sent */
      u_long  tcps_sndprobe;          /* window probes sent */
      u_long  tcps_sndurg;            /* packets sent with URG only */
      u_long  tcps_sndwinup;          /* window update-only packets sent */
      u_long  tcps_sndctrl;           /* control (SYN|FIN|RST) packets sent */
      u_long  tcps_rcvtotal;          /* total packets received */
      u_long  tcps_rcvpack;           /* packets received in sequence */
      u_long  tcps_rcvbyte;           /* bytes received in sequence */

      u_long  tcps_rcvbadsum;         /* packets received with ccksum errs */
      u_long  tcps_rcvbadoff;         /* packets received with bad offset */
      u_long  tcps_rcvshort;          /* packets received too short */

      u_long  tcps_rcvduppack;        /* duplicate-only packets received */
      u_long  tcps_rcvdupbyte;        /* duplicate-only bytes received */
      u_long  tcps_rcvpartduppack;    /* packets with some duplicate data */
      u_long  tcps_rcvpartdupbyte;    /* dup. bytes in part-dup. packets */
      u_long  tcps_rcvoopack;         /* out-of-order packets received */
      u_long  tcps_rcvoobyte;         /* out-of-order bytes received */
      u_long  tcps_rcvpackafterwin;   /* packets with data after window */
      u_long  tcps_rcvbyteafterwin;   /* bytes rcvd after window */
      u_long  tcps_rcvafterclose;     /* packets rcvd after "close" */
      u_long  tcps_rcvwinprobe;       /* rcvd window probe packets */
      u_long  tcps_rcvdupack;         /* rcvd duplicate acks */
      u_long  tcps_rcvacktoomuch;     /* rcvd acks for unsent data */
      u_long  tcps_rcvackpack;        /* rcvd ack packets */
      u_long  tcps_rcvackbyte;        /* bytes acked by rcvd acks */
      u_long  tcps_rcvwinupd;         /* rcvd window update packets */
   } tcpStat;

/* ====================================================================== */
/* NET_DEVICES_FLAG for tlmHeader.tlmParamsMask                          */
/* ====================================================================== */
typedef
   struct {
      char  amount;
      char  reserved_1;
      short reserved_2;

#ifdef ALIGNMENT_8
      long  reserved_long;
#endif /* ALIGNMENT_8 */

   } netDeviceHdr;

typedef 
   struct {
      union {
         struct {
            char    if_name[10];     /* name, e.g. "ln" (Ethernet), "rch" (Radio channel) or
                                        "lo" (internal loopback) else Slip */
            short   if_mtu;          /* maximum transmission unit */
         } data;
         long  array[3];
      }       if_params;
/* generic interface statistics */
      int     if_ipackets;     /* packets received on interface */
      int     if_ierrors;      /* input errors on interface */
      int     if_opackets;     /* packets sent on interface */
      int     if_oerrors;      /* output errors on interface */
      int     if_collisions;   /* collisions on csma interfaces */
} netDeviceData;

/* ====================================================================== */
/* Values of status of reading the parameters (npTlmParams.prmStatus)     */
/* ====================================================================== */
enum {
   CMD_TLM_STATUS_OK,        /* Successful reading the parameters */
   CMD_TLM_STATUS_D_ERROR,   /* Error of reading the parameters */
   CMD_TLM_STATUS_C_ERROR,   /* Error of command */
   CMD_TLM_GET               /* Telemetry request */
};

#endif //__IP_STAT_H_INCLUDED__

