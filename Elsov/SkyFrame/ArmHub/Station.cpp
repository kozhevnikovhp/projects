#include "stdafx.h"
#include "Station.h"
#include "Thread.h"
#include "IpSockets.h"
#include "LogFile.h"

static char pszTempBuffer[1024];

// Class CListOfObjects - implementation
IMPLEMENT_SERIAL(CListOfObjects,CObList,1)

// Class CPpdTmi - implementation
IMPLEMENT_SERIAL(CPpdTmi, CObject, 1)

netDeviceData CPpdTmi::m_ZeroNetDeviceStatistics;

CPpdTmi::CPpdTmi()
{
	memset(&m_ZeroNetDeviceStatistics, 0, sizeof(m_ZeroNetDeviceStatistics));
	memset(&m_ModemStatus, 0, sizeof(m_ModemStatus));
	memset(&m_BstStatus, 0, sizeof(m_BstStatus));
	memset(&m_Header, 0, sizeof(m_Header));
	memset(&m_IpStat, 0, sizeof(m_IpStat));
	memset(&m_DeltaIpStat, 0, sizeof(m_DeltaIpStat));
	memset(&m_UdpStat, 0, sizeof(m_UdpStat));
	memset(&m_DeltaUdpStat, 0, sizeof(m_DeltaUdpStat));
	memset(&m_NetDeviceHeader, 0, sizeof(m_NetDeviceHeader));
	m_nNetDevices = 0;
	m_nMaxAllocatedDevices = 0;
	m_pNetDeviceData = m_pDeltaNetDeviceData = NULL;
}

CPpdTmi::~CPpdTmi()
{
	FreeAllocatedDevices();
}

void CPpdTmi::FreeAllocatedDevices()
{
	if (m_pNetDeviceData)
		delete[] m_pNetDeviceData;
	m_pNetDeviceData = NULL;
	if (m_pDeltaNetDeviceData)
		delete[] m_pDeltaNetDeviceData;
	m_pDeltaNetDeviceData = NULL;
}

BOOL CPpdTmi::HaveModemStatus()
{
	return (m_Header.tlmParamsMask & MODEM_PARAMS_FLAG);
}

BOOL CPpdTmi::HaveBstStatus()
{
	return (m_Header.tlmParamsMask & BST_PARAMETERS_FLAG);
}

BOOL CPpdTmi::HaveIpStatistics()
{
	return (m_Header.tlmParamsMask & IP_STAT_FLAG);
}

BOOL CPpdTmi::HaveUdpStatistics()
{
	return (m_Header.tlmParamsMask & UDP_STAT_FLAG);
}

void CPpdTmi::PrintModemStatus
(
	CFile *pFile
)
{
	pFile->Seek(0, CFile::end);
	CString str;
	str = m_Time.Format("%d.%m.%Y\t%H:%M:%S");
	sprintf(pszTempBuffer, "%s\t", str);
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));

	sprintf(pszTempBuffer, "%f\t%f\n",
      GetSignalNoise(),  /* signal to noise ratio */
      GetAruValue()     /* input signal level */
	  );
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));
}

void CPpdTmi::PrintBstStatus(CFile *pFile)
{
	pFile->Seek(0, CFile::end);
	CString str;
	str = m_Time.Format("%d.%m.%Y\t%H:%M:%S");
	sprintf(pszTempBuffer, "%s\t", str);
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));

	sprintf(pszTempBuffer, "%d\t%d\t%d\t%d\t%d\n",
      BstGetAdcValue(),	// power in mW
      BstGetAdcHexValue(),	// power (hex value)
	  BstGetAdcChannel(),
	  BstIsTransmitterPowerOn(),
	  BstIsReceiverPowerOn()
	  );
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));
}

void CPpdTmi::PrintIpStatistics(CFile *pFile)
{
	pFile->Seek(0, CFile::end);
	CString str;
	str = m_Time.Format("%d.%m.%Y\t%H:%M:%S");
	sprintf(pszTempBuffer, "%s\t", str);
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));

	sprintf(pszTempBuffer, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
      m_DeltaIpStat.ips_total,          /* total packets received */
      m_DeltaIpStat.ips_badsum,         /* checksum bad */
      m_DeltaIpStat.ips_tooshort,       /* packet too short */
      m_DeltaIpStat.ips_toosmall,       /* not enough data */
      m_DeltaIpStat.ips_badhlen,        /* ip header length < data size */
      m_DeltaIpStat.ips_badlen,         /* ip length < ip header length */
      m_DeltaIpStat.ips_fragments,      /* fragments received */
      m_DeltaIpStat.ips_fragdropped,    /* frags dropped (dups, out of space) */
      m_DeltaIpStat.ips_fragtimeout,    /* fragments timed out */
      m_DeltaIpStat.ips_forward,	 /* packets forwarded */
      m_DeltaIpStat.ips_cantforward,    /* packets rcvd for unreachable dest */
      m_DeltaIpStat.ips_redirectsent   /* packets forwarded on same net */
	  );
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));
}

void CPpdTmi::PrintIpRchStatistics(CFile *pFile)
{
	pFile->Seek(0, CFile::end);
	CString str;
	str = m_Time.Format("%d.%m.%Y\t%H:%M:%S");
	sprintf(pszTempBuffer, "%s\t", str);
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));

	int InterfaceNo;
	for (InterfaceNo = 0; InterfaceNo < GetNetDeviceCount(); InterfaceNo++)
	{
		if (!strcmp(m_pDeltaNetDeviceData[InterfaceNo].if_params.data.if_name, "rch"))
		{
			sprintf(pszTempBuffer, "%d\t%d\n",
				m_pDeltaNetDeviceData[InterfaceNo].if_ipackets,     /* packets received on radiochannel */
				m_DeltaIpStat.ips_badsum							/* checksum bad */
				);
			pFile->Write(pszTempBuffer, strlen(pszTempBuffer));
			break;
		}
	}

}

void CPpdTmi::PrintUdpStatistics(CFile *pFile)
{
	pFile->Seek(0, CFile::end);
	CString str;
	str = m_Time.Format("%d.%m.%Y\t%H:%M:%S");
	sprintf(pszTempBuffer, "%s\t", str);
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));

	sprintf(pszTempBuffer, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
      m_DeltaUdpStat.udps_hdrops,        /* bad header length */
      m_DeltaUdpStat.udps_badsum,        /* bad header checksum */
      m_DeltaUdpStat.udps_badlen,        /* bad packet length */
      m_DeltaUdpStat.udps_ipackets,      /* total packets received, including errors */
      m_DeltaUdpStat.udps_opackets,      /* total packets sent */
      m_DeltaUdpStat.udps_noportbcast,   /* broadcast packets, no port */
      m_DeltaUdpStat.udps_fullsock       /* packets dropped socket recv side is full */
	  );
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));
}

void CPpdTmi::PrintNetDeviceStatistics
(
	CFile *pFile,
	int InterfaceNo
)
{
	pFile->Seek(0, CFile::end);
	CString str;
	str = m_Time.Format("%d.%m.%Y\t%H:%M:%S");
	sprintf(pszTempBuffer, "%s\t", str);
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));

	sprintf(pszTempBuffer, "%d\t%d\t%d\t%d\t%d\n",
		m_pDeltaNetDeviceData[InterfaceNo].if_ipackets,     /* packets received on interface */
		m_pDeltaNetDeviceData[InterfaceNo].if_ierrors,      /* input errors on interface */
		m_pDeltaNetDeviceData[InterfaceNo].if_opackets,     /* packets sent on interface */
		m_pDeltaNetDeviceData[InterfaceNo].if_oerrors,      /* output errors on interface */
		m_pDeltaNetDeviceData[InterfaceNo].if_collisions   /* collisions on csma interfaces */
		);
	pFile->Write(pszTempBuffer, strlen(pszTempBuffer));
}

CString CPpdTmi::GetNetDeviceName(int InterfaceNo)
{
	static char *pszRawNames[] = {"ln", "rch", "lo"};
	static char *pszNames[] = {"Ethernet", "RadioChannel", "Loopback"};
	CString RetString = "Slip";
	if (InterfaceNo < GetNetDeviceCount())
	{
		char *pszRawName = GetInterfaceRawName(InterfaceNo);
		for (int i=0; i<sizeof(pszRawNames)/sizeof(pszRawNames[0]); i++)
		{
			if (!strcmp(pszRawName, pszRawNames[i]))
			{
				RetString = pszNames[i];
				break;
			}
		}
	}
	return RetString;
}

BOOL CPpdTmi::HaveNetDeviceStatistics()
{
	return (m_Header.tlmParamsMask & NET_DEVICES_FLAG);
}


void CPpdTmi::Serialize(CArchive &archive)
{
	int Version;
	CObject::Serialize(archive);
	if (archive.IsStoring())
	{
		Version = 2;
		archive<<Version;
		archive.Write(&m_Header, sizeof(m_Header));
		archive.Write(&m_ModemStatus, sizeof(m_ModemStatus));
		archive.Write(&m_BstStatus, sizeof(m_BstStatus));
		archive.Write(&m_IpStat, sizeof(m_IpStat));
		archive.Write(&m_DeltaIpStat, sizeof(m_DeltaIpStat));
		archive.Write(&m_UdpStat, sizeof(m_UdpStat));
		archive.Write(&m_DeltaUdpStat, sizeof(m_DeltaUdpStat));
		archive<<m_nNetDevices;
		archive.Write(&m_NetDeviceHeader, sizeof(m_NetDeviceHeader));
		if (m_nNetDevices)
		{
			archive.Write(m_pNetDeviceData, m_nNetDevices * sizeof(netDeviceData));
			archive.Write(m_pDeltaNetDeviceData, m_nNetDevices * sizeof(netDeviceData));
		}
	}
	else
	{
		archive>>Version;
		archive.Read(&m_Header, sizeof(m_Header));
		archive.Read(&m_ModemStatus, sizeof(m_ModemStatus));
		if (Version >= 2)
			archive.Read(&m_BstStatus, sizeof(m_BstStatus));
		archive.Read(&m_IpStat, sizeof(m_IpStat));
		archive.Read(&m_DeltaIpStat, sizeof(m_DeltaIpStat));
		archive.Read(&m_UdpStat, sizeof(m_UdpStat));
		archive.Read(&m_DeltaUdpStat, sizeof(m_DeltaUdpStat));
		archive>>m_nNetDevices;
		archive.Read(&m_NetDeviceHeader, sizeof(m_NetDeviceHeader));
		ReallocNetDevicesData(m_nNetDevices);
		if (m_nNetDevices)
		{
			archive.Read(m_pNetDeviceData, m_nNetDevices * sizeof(netDeviceData));
			archive.Read(m_pDeltaNetDeviceData, m_nNetDevices * sizeof(netDeviceData));
		}
	}
}

void CPpdTmi::ReallocNetDevicesData
(
	int nNetDevices
)
{
	if (nNetDevices <= m_nMaxAllocatedDevices && m_pNetDeviceData)
	{ // nothing to do
	}
	else
	{
		FreeAllocatedDevices();
		if (nNetDevices)
		{
			m_pNetDeviceData = new netDeviceData[nNetDevices];
			memset(m_pNetDeviceData, 0, nNetDevices*sizeof(netDeviceData));
			m_pDeltaNetDeviceData = new netDeviceData[nNetDevices];
			memset(m_pDeltaNetDeviceData, 0, nNetDevices*sizeof(netDeviceData));
			m_nMaxAllocatedDevices = nNetDevices;
		}
	}
}

void CPpdTmi::ProcessPacket
(
	char *pBuffer,
	int length
)
{
	m_Time = CTime::GetCurrentTime();
	char *pCurrentPos = pBuffer;
	memcpy (&m_Header, pCurrentPos, sizeof(m_Header));
	m_Header.command = ntohs(m_Header.command);
	m_Header.tlmParamsMask = ntohl(m_Header.tlmParamsMask);
	m_Header.signature = ntohl(m_Header.signature);
	m_Header.serialNumber = ntohs(m_Header.serialNumber);
	pCurrentPos += sizeof(m_Header);

	if (HaveModemStatus())
	{
		memcpy(&m_ModemStatus, pCurrentPos, sizeof(m_ModemStatus));
		pCurrentPos += sizeof(m_ModemStatus);		
	}

	if (HaveBstStatus())
	{
		bstStatus *pData = (bstStatus *)pCurrentPos;
		m_BstStatus.powerFlags = ntohl(pData->powerFlags);     /* Transmitter, Receiver On/Off */
		m_BstStatus.ADCChannel = ntohl(pData->ADCChannel);     /* ADC Channel */
		m_BstStatus.ADCValueHex = ntohl(pData->ADCValueHex);    /* ADC Value Hex format */
		m_BstStatus.ADCValueMw = ntohl(pData->ADCValueMw);     /* ADC Value mW format */
		pCurrentPos += sizeof(m_BstStatus);		
	}

	if (HaveIpStatistics())
	{
		long *pFieldOld = (long *)&m_IpStat;
		long *pFieldDelta=(long *)&m_DeltaIpStat;
		long *pFieldNew = (long *)pCurrentPos;

		long *pNewValue=(long *)pCurrentPos;
		for (int i=0; i<sizeof(m_IpStat)/sizeof(long); i++)
		{
			*pNewValue = ntohl(*pNewValue);
			pNewValue++;
		}

		for (i=0; i<sizeof(m_IpStat)/sizeof(long); i++)
		{
			*pFieldDelta = *pFieldNew - *pFieldOld;
			*pFieldOld = *pFieldNew;
			++pFieldOld;
			++pFieldDelta;
			++pFieldNew;
		}
		pCurrentPos += sizeof(m_IpStat);
	}

	if (HaveUdpStatistics())
	{
		long *pFieldOld = (long *)&m_UdpStat;
		long *pFieldDelta=(long *)&m_DeltaUdpStat;
		long *pFieldNew = (long *)pCurrentPos;

		long *pNewValue=(long *)pCurrentPos;
		for (int i=0; i<sizeof(m_UdpStat)/sizeof(int); i++)
		{
			*pNewValue = ntohl(*pNewValue);
			pNewValue++;
		}

		for (i=0; i<sizeof(m_UdpStat)/sizeof(int); i++)
		{
			*pFieldDelta = *pFieldNew - *pFieldOld;
			*pFieldOld = *pFieldNew;
			++pFieldOld;
			++pFieldDelta;
			++pFieldNew;
		}
		pCurrentPos += sizeof(m_UdpStat);
	}

	if (HaveNetDeviceStatistics())
	{
		memcpy(&m_NetDeviceHeader, pCurrentPos, sizeof(m_NetDeviceHeader));
		pCurrentPos += sizeof(m_NetDeviceHeader);
		m_nNetDevices = m_NetDeviceHeader.amount;
		ReallocNetDevicesData(m_nNetDevices);
		m_bHaveRadiochannelStatistics = FALSE;
		for (int InterfaceNo = 0; InterfaceNo < GetNetDeviceCount(); InterfaceNo++)
		{
			memcpy(&m_pDeltaNetDeviceData[InterfaceNo], pCurrentPos, sizeof(*m_pDeltaNetDeviceData));

			// RadioChannel
			if (!strcmp(m_pDeltaNetDeviceData[InterfaceNo].if_params.data.if_name, "rch"))
				m_bHaveRadiochannelStatistics = TRUE;

			m_pDeltaNetDeviceData[InterfaceNo].if_ipackets = ntohl(m_pDeltaNetDeviceData[InterfaceNo].if_ipackets) - m_pNetDeviceData[InterfaceNo].if_ipackets;
			m_pDeltaNetDeviceData[InterfaceNo].if_ierrors = ntohl(m_pDeltaNetDeviceData[InterfaceNo].if_ierrors) - m_pNetDeviceData[InterfaceNo].if_ierrors;
			m_pDeltaNetDeviceData[InterfaceNo].if_opackets = ntohl(m_pDeltaNetDeviceData[InterfaceNo].if_opackets) - m_pNetDeviceData[InterfaceNo].if_opackets;
			m_pDeltaNetDeviceData[InterfaceNo].if_oerrors = ntohl(m_pDeltaNetDeviceData[InterfaceNo].if_oerrors) - m_pNetDeviceData[InterfaceNo].if_oerrors;
			m_pDeltaNetDeviceData[InterfaceNo].if_collisions = ntohl(m_pDeltaNetDeviceData[InterfaceNo].if_collisions) - m_pNetDeviceData[InterfaceNo].if_collisions;

			memcpy(&m_pNetDeviceData[InterfaceNo], pCurrentPos, sizeof(*m_pNetDeviceData));
			m_pNetDeviceData[InterfaceNo].if_ipackets = ntohl(m_pNetDeviceData[InterfaceNo].if_ipackets);
			m_pNetDeviceData[InterfaceNo].if_ierrors = ntohl(m_pNetDeviceData[InterfaceNo].if_ierrors);
			m_pNetDeviceData[InterfaceNo].if_opackets = ntohl(m_pNetDeviceData[InterfaceNo].if_opackets);
			m_pNetDeviceData[InterfaceNo].if_oerrors = ntohl(m_pNetDeviceData[InterfaceNo].if_oerrors);
			m_pNetDeviceData[InterfaceNo].if_collisions = ntohl(m_pNetDeviceData[InterfaceNo].if_collisions);
			pCurrentPos += sizeof(*m_pNetDeviceData);
		}
	}
}

char *CPpdTmi::GetInterfaceRawName
(
	int InterfaceNo
)
{
	char *pszReturn="";
	if (InterfaceNo < GetNetDeviceCount())
	{
		pszReturn = m_pDeltaNetDeviceData[InterfaceNo].if_params.data.if_name;
	}
	return pszReturn;
}

float CPpdTmi::GetSignalNoise()
{
	float fAdd05 = 0.0f;
	if (m_ModemStatus.signalNoise & 1)
		fAdd05 =0.5f;

	float SN = float(m_ModemStatus.signalNoise >> 1) + fAdd05;
   
	return SN;
}

float CPpdTmi::GetAruValue()
{
	double a = (-10.0+50.0)/(4.37-2.81);
	double b = -10 - a*4.37;
	float fAruValue = (float)(a*(float)(m_ModemStatus.ARUValue/51.f) +b);
	return fAruValue;
}

CString CPpdTmi::GetModemType()
{
	static char *pszModemTypes[] = {"Standard", "Nonstandard1", "Nonstandard2", "Nonstandard3" };
	char ModemType = m_ModemStatus.typeModem;
	if (ModemType == 0 || ModemType >= sizeof(pszModemTypes)/sizeof(pszModemTypes[0]))
		return "Unknown";
	return pszModemTypes[ModemType-1];
}

BOOL CPpdTmi::IsBPCHTransmitterLoaded()
{
	return m_ModemStatus.flagBpch & FLAG_BPCH_TX;
}

BOOL CPpdTmi::IsBPCHReceiverLoaded()
{
	return m_ModemStatus.flagBpch & FLAG_BPCH_RX;
}

CString CPpdTmi::GetIpStatisticsText
(
	BOOL bIncremental
)
{
	char *pszFormatString="\
total packets received          %d\r\n\
checksum bad                    %d\r\n\
packet too short                %d\r\n\
not enough data                 %d\r\n\
ip header length < data size    %d\r\n\
ip length < ip header length    %d\r\n\
fragments received              %d\r\n\
fragments dropped               %d\r\n\
fragments timed out             %d\r\n\
packets forwarded               %d\r\n\
packets to unreachable dest     %d\r\n\
packets forwarded on same net   %d";
		
	ipStat *pIpStat = &m_IpStat;
	if (bIncremental)
		pIpStat = &m_DeltaIpStat;
	CString str;
	str.Format(pszFormatString,
		pIpStat->ips_total,       
		pIpStat->ips_badsum,
		pIpStat->ips_tooshort,
		pIpStat->ips_toosmall,
		pIpStat->ips_badhlen,
		pIpStat->ips_badlen,
		pIpStat->ips_fragments,
		pIpStat->ips_fragdropped,
		pIpStat->ips_fragtimeout,
		pIpStat->ips_forward,
		pIpStat->ips_cantforward,
		pIpStat->ips_redirectsent);
	return str;
}

CString CPpdTmi::GetUdpStatisticsText
(
	BOOL bIncremental
)
{
	char *pszFormatString="\
bad header length           %d\r\n\
bad header checksum         %d\r\n\
bad packet length           %d\r\n\
total packets received      %d\r\n\
total packets sent          %d\r\n\
broadcast packets, no port  %d\r\n\
socket overflow             %d";
		
	udpStat *pUdpStat = &m_UdpStat;
	if (bIncremental)
		pUdpStat = &m_DeltaUdpStat;
	CString str;
	str.Format(pszFormatString,
      pUdpStat->udps_hdrops,        /* bad header length */
      pUdpStat->udps_badsum,        /* bad header checksum */
      pUdpStat->udps_badlen,       /* bad packet length */
      pUdpStat->udps_ipackets,      /* total packets received, including errors */
      pUdpStat->udps_opackets,      /* total packets sent */
      pUdpStat->udps_noportbcast,   /* broadcast packets, no port */
      pUdpStat->udps_fullsock      /* packets dropped socket recv side is full */
		  );
	return str;
}

CString CPpdTmi::GetNetDeviceStatisticsText
(
	unsigned InterfaceNo,
	BOOL bIncremental
)
{
	char *pszFormatString="\
packets received   %d\r\n\
input errors       %d\r\n\
packets sent       %d\r\n\
output errors      %d\r\n\
collisions on csma %d";
		
	netDeviceData *pNetDeviceData = m_pNetDeviceData + InterfaceNo;
	if (bIncremental)
		pNetDeviceData = m_pDeltaNetDeviceData + InterfaceNo;
	if (!pNetDeviceData)
		pNetDeviceData = &m_ZeroNetDeviceStatistics;

	CString str;
	str.Format(pszFormatString,
		pNetDeviceData->if_ipackets,  	/* packets received on interface */
		pNetDeviceData->if_ierrors,   	/* input errors on interface */
		pNetDeviceData->if_opackets,  	/* packets sent on interface */
		pNetDeviceData->if_oerrors,   	/* output errors on interface */
		pNetDeviceData->if_collisions);	/* collisions on csma interfaces */
	return str;
}


// Class CStationTelnetSocket - implementation

//virtual
void CStationTelnetSocket::OnSymbolToUser(char symbol)
{
	m_pStation->TelnetSymbolReceived(symbol);
}

// Class CStation - implementation

IMPLEMENT_SERIAL(CStation, CObject, 1)

short CStation::m_TmiIpPort = 8201;
CIcmpThreadedSocket *CStation::m_pPingSocket=0;

CStation::CStation()
{
	Initialize();
}

CStation::CStation(CString name)
{
	m_Name = name;
	Initialize();
}

CStation::CStation(CStation *pStation)
{
	Initialize();
	m_Name = pStation->m_Name;

	m_LastQueryTime = pStation->m_LastQueryTime;
	m_LastReplyTime = pStation->m_LastReplyTime;

	m_LastStatQueryTime = pStation->m_LastStatQueryTime;
	m_LastStatReplyTime = pStation->m_LastStatReplyTime;

	m_QueryPeriod = pStation->m_QueryPeriod;
	m_RequeryPeriod = pStation->m_RequeryPeriod;
	m_bQueryEnabled = pStation->m_bQueryEnabled;

	m_IpAddressSP = pStation->m_IpAddressSP;
	m_IpAddressRCh = pStation->m_IpAddressRCh;
	m_LastPingTime = pStation->m_LastPingTime;
	m_LastPingTimeSP = pStation->m_LastPingTimeSP;
	m_LastPingTimeRCh = pStation->m_LastPingTimeRCh;
	m_LastPingTime = pStation->m_LastPingTime;
	m_LastPingReplySP = pStation->m_LastPingReplySP;
	m_LastPingReplyRCh = pStation->m_LastPingReplySP;
	m_PingDelaySP = m_PingDelayRCh = 0;
	m_bPingEnabled = pStation->m_bPingEnabled;
	m_PingPeriod = pStation->m_PingPeriod;

	m_PingDirectionFlag = 0;
}

void CStation::Initialize()
{
	InitializeCriticalSection(&m_Critical);
	EnterCritical();

	m_LastQueryTime =
	m_LastReplyTime = CTime::GetCurrentTime();

	m_LastStatQueryTime =
	m_LastStatReplyTime = CTime::GetCurrentTime();

	m_QueryPeriod = CTimeSpan(0, 0, 2, 0);
	m_RequeryPeriod = CTimeSpan(0, 0, 1, 0);
	m_bConnectedTMI = TRUE;
	m_bAlive = TRUE;
	m_bQueryEnabled = FALSE;
	m_bTmiPseudoQueried = FALSE;
	m_bTmiConnectionFailedReported = m_bTmiConnectionRestoredReported = FALSE;
	m_pTmiSocket = new CTmiSocket(this);

	m_TelnetSocket.SetStation(this);
	m_bTelnetTextChanged = FALSE;

	m_bConnectedSP = TRUE;
	m_bConnectedRCh = TRUE;
	m_IpAddressSP = m_IpAddressRCh = 0;
	m_LastPingTime = CTime::GetCurrentTime();
	m_LastPingTimeSP = m_LastPingTimeRCh = m_LastPingTime;
	m_LastPingReplySP = m_LastPingReplyRCh = m_LastPingTime;
	m_PingDelaySP = m_PingDelayRCh = 0;
	m_bPingEnabled = TRUE;
	m_PingPeriod = CTimeSpan(0, 0, 0, 10);

	m_PingDirectionFlag = 0;
	LeaveCritical();
}

CStation::~CStation()
{
	delete m_pTmiSocket;
	DeleteCriticalSection(&m_Critical);
	TRACE("Station %s is destructed\n", m_Name);
}

BOOL CStation::IsAlive()
{
	EnterCritical();
	BOOL bAlive = m_bAlive;
	LeaveCritical();
	return bAlive;
}

void CStation::Serialize(CArchive &archive)
{
	EnterCritical();
	CObject::Serialize(archive);
	m_PpdTmi.Serialize(archive);
	int Version;
	if (archive.IsStoring())
	{
		Version = 4;
		archive<<Version;
		archive<<m_Name<<m_IpAddressSP<<m_IpAddressRCh
			<<m_TmiIpPort
			<<m_QueryPeriod<<m_RequeryPeriod<<m_bQueryEnabled
			<<m_LastQueryTime<<m_LastReplyTime
			<<m_LastStatQueryTime<<m_LastStatReplyTime;
		archive<<m_bTmiConnectionFailedReported;
		archive<<m_bTmiConnectionRestoredReported;
		archive<<m_bPingEnabled<<m_PingPeriod;
	}
	else
	{
		archive>>Version;
		archive>>m_Name>>m_IpAddressSP>>m_IpAddressRCh
			>>m_TmiIpPort
			>>m_QueryPeriod>>m_RequeryPeriod>>m_bQueryEnabled
			>>m_LastQueryTime>>m_LastReplyTime
			>>m_LastStatQueryTime>>m_LastStatReplyTime;
		if (Version >= 2)
			archive>>m_bTmiConnectionFailedReported;
		if (Version >= 3)
			archive>>m_bTmiConnectionRestoredReported;
		if (Version >= 4)
			archive>>m_bPingEnabled>>m_PingPeriod;
	}
	LeaveCritical();
}

CString CStation::GetName()
{
	EnterCritical();
	CString name = m_Name;
	LeaveCritical();
	return name;
}

void CStation::SetName(CString name)
{
	EnterCritical();
	m_Name = name;
	LeaveCritical();
}

void CStation::EnterCritical()
{
	EnterCriticalSection(&m_Critical);
}

void CStation::LeaveCritical()
{
	LeaveCriticalSection(&m_Critical);
}

void CStation::QueryTmiSchedule()
{
	if (IsAlive() && m_IpAddressSP)
	{
		// Is it time to query statistics?
		CTime NextTime = GetNextStatQueryTime();
		if (NextTime <= CTime::GetCurrentTime())
		{
			if (IsQueryEnabled())
				QueryTmi();
			else
				PseudoQueryTmi();
		}
	}
}

void CStation::SendPingSchedule()
{
	if (!IsPingEnabled())
		return;
	if (!m_pPingSocket->CanSendRequest())
		return;
	CTime CurrentTime = CTime::GetCurrentTime();
	CTimeSpan DeltaTime = CurrentTime - m_LastPingTime;
	if (DeltaTime>GetPingPeriod())
	{
		EnterCritical();
		if (m_IpAddressSP)
		{
			if (!m_bConnectedSP)
				m_PingDelaySP = -1;
			m_pPingSocket->SendEchoRequest(10, m_IpAddressSP);
			m_LastPingTimeSP = CurrentTime;
		}
		else
		{
			m_bConnectedSP = FALSE;
			m_PingDelaySP = -1;
		}
		m_LastPingTime = CurrentTime;
		LeaveCritical();
	}
}

BOOL CStation::IsMyAddress
(
	unsigned long IpAddress
)
{
	EnterCritical();
	BOOL bMy=(IpAddress == m_IpAddressSP)/* || (IpAddress == m_IpAddressRCh)*/;
	LeaveCritical();
	return bMy;
}

BOOL CStation::IsMyPingEchoReply(unsigned long IpAddress)
{
	return IsMyAddress(IpAddress);
}

BOOL CStation::IsMyPingDestUnreachable(unsigned long IpAddress)
{
	return IsMyAddress(IpAddress);
}

void CStation::PrintModemStatus()
{
	CFile file;
	CString FileName = GetModemStatusFileName();
	BOOL bFileOpened = file.Open(FileName, CFile::modeWrite);
	if (!bFileOpened)
	{
		if (bFileOpened = file.Open(FileName, CFile::modeCreate | CFile::modeWrite))
		{
			strcpy(pszTempBuffer, "Date\tTime\tS/N\tARU\n");
			file.Write(pszTempBuffer, strlen(pszTempBuffer));
		}
	}
	if (bFileOpened)
		m_PpdTmi.PrintModemStatus(&file);
	file.Close();
}

void CStation::PrintBstStatus()
{
	CFile file;
	CString FileName = GetBstStatusFileName();
	BOOL bFileOpened = file.Open(FileName, CFile::modeWrite);
	if (!bFileOpened)
	{
		if (bFileOpened = file.Open(FileName, CFile::modeCreate | CFile::modeWrite))
		{
			strcpy(pszTempBuffer, "Date\tTime\tPower (mW)\tPower (hex)\tADC Channel\tTransmitter On\tReceiver On\n");
			file.Write(pszTempBuffer, strlen(pszTempBuffer));
		}
	}
	if (bFileOpened)
		m_PpdTmi.PrintBstStatus(&file);
	file.Close();
}

void CStation::PrintIpStatistics()
{
	CFile file;
	CString FileName = GetIpStatisticsFileName();
	BOOL bFileOpened = file.Open(FileName, CFile::modeWrite);
	if (!bFileOpened)
	{
		if (bFileOpened = file.Open(FileName, CFile::modeCreate | CFile::modeWrite))
		{
			strcpy(pszTempBuffer, "Date\tTime\ttotal\tbad crc\ttoo short\tnot enough data\tip header length < data size\tip length < ip header length\tfragments received\tfrags dropped\tfragments timed out\tforwarded\tunreachable dest\tpackets forwarded on same net\n");
			file.Write(pszTempBuffer, strlen(pszTempBuffer));
		}
	}
	if (bFileOpened)
		m_PpdTmi.PrintIpStatistics(&file);
	file.Close();
}

void CStation::PrintIpRchStatistics()
{
	CFile file;
	CString FileName = GetIpRchStatisticsFileName();
	BOOL bFileOpened = file.Open(FileName, CFile::modeWrite);
	if (!bFileOpened)
	{
		if (bFileOpened = file.Open(FileName, CFile::modeCreate | CFile::modeWrite))
		{
			strcpy(pszTempBuffer, "Date\tTime\trcvd\tbad crc\n");
			file.Write(pszTempBuffer, strlen(pszTempBuffer));
		}
	}
	if (bFileOpened)
		m_PpdTmi.PrintIpRchStatistics(&file);
	file.Close();
}

void CStation::PrintUdpStatistics()
{
	CFile file;
	CString FileName = GetUdpStatisticsFileName();
	BOOL bFileOpened = file.Open(FileName, CFile::modeWrite);
	if (!bFileOpened)
	{
		if (bFileOpened = file.Open(FileName, CFile::modeCreate | CFile::modeWrite))
		{
			strcpy(pszTempBuffer, "Date\tTime\tbad header length\tbad header checksum\tbad packet length\ttotal packets received\ttotal packets sent\tbroadcast packets, no port\tsocket overflow\n");
			file.Write(pszTempBuffer, strlen(pszTempBuffer));
		}
	}
	if (bFileOpened)
		m_PpdTmi.PrintUdpStatistics(&file);
	file.Close();
}

void CStation::PrintNetDeviceStatistics()
{
	for (int j = 0; j < m_PpdTmi.GetNetDeviceCount(); j++)
	{
		CFile file;
		CString FileName = GetNetDeviceStatisticsFileName(j);
		BOOL bFileOpened = file.Open(FileName, CFile::modeWrite);
		if (!bFileOpened)
		{
			if (bFileOpened = file.Open(FileName, CFile::modeCreate | CFile::modeWrite))
			{
				strcpy(pszTempBuffer, "Date\tTime\tpackets received\tinput errors\tpackets sent\toutput errors\tcollisions on csma\n");
				file.Write(pszTempBuffer, strlen(pszTempBuffer));
			}
		}
		if (bFileOpened)
			m_PpdTmi.PrintNetDeviceStatistics(&file, j);
		file.Close();
	}
}

void CStation::ReportTMIConnectionFailed()
{
	if (m_bTmiConnectionFailedReported)
		return; // recorded already
	m_bTmiConnectionRestoredReported = FALSE;
	m_bTmiConnectionFailedReported = TRUE;
	CString FileName = GetLogFileName();
	CLogFile log(FileName);
	log.WriteString("TMI connection failed!");
}

void CStation::ReportTMIConnectionRestored()
{
	if (m_bTmiConnectionRestoredReported)
		return; // recorded already
	m_bTmiConnectionRestoredReported = TRUE;
	m_bTmiConnectionFailedReported = FALSE;
	CString FileName = GetLogFileName();
	CLogFile log(FileName);
	log.WriteString("TMI connection established.");
}

CString CStation::GetLogFileName()
{
	EnterCritical();
	CString str;
	str.Format("%s_Log.txt", m_Name);
	LeaveCritical();
	return str;
}

void CStation::ProcessPpdTmiDatagram
(
	char *pBuffer,
	int length
)
{
	if (length <= sizeof(tlmHeader))
	{
		TRACE("Invalid TMI datagram! May be from the same computer.\n");
		return;
	}

	EnterCritical();
	m_LastReplyTime = CTime::GetCurrentTime();
	m_LastStatReplyTime = m_LastReplyTime;
	ReportTMIConnectionRestored();
	m_PpdTmi.ProcessPacket(pBuffer, length);
	
	CString FileName;
	CFile file;
	BOOL bFileOpened = FALSE;

	if (m_PpdTmi.HaveModemStatus())
		PrintModemStatus();

	if (m_PpdTmi.HaveBstStatus())
		PrintBstStatus();

	if (m_PpdTmi.HaveIpStatistics())
		PrintIpStatistics();

	if (m_PpdTmi.HaveUdpStatistics())
		PrintUdpStatistics();

	if (m_PpdTmi.HaveNetDeviceStatistics())
		PrintNetDeviceStatistics();

	if (m_PpdTmi.HaveIpStatistics() && m_PpdTmi.HaveNetDeviceStatistics() && m_PpdTmi.HaveRadiochannelStatistics())
		PrintIpRchStatistics();

	LeaveCritical();
}

void CStation::ProcessPingEchoReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned long SourceIP)
{
	EnterCritical();
//	TRACE("%s - echo reply received ", m_Name);
	if (SourceIP == m_IpAddressSP)
	{
//		TRACE("from communication processor");
		m_bConnectedSP = TRUE;
		m_LastPingReplySP = CTime::GetCurrentTime();
		m_PingDelaySP = m_pPingSocket->GetDelayTime();
	}
	else if (SourceIP == m_IpAddressRCh)
	{
//		TRACE("from radiochannel");
		m_bConnectedRCh = TRUE;
		m_LastPingReplyRCh = CTime::GetCurrentTime();
		m_PingDelayRCh = m_pPingSocket->GetDelayTime();
	}
	TRACE("\n");
	LeaveCritical();
}

void CStation::ProcessPingDestUnreachable(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader)
{
	EnterCritical();
//	m_bConnected=TRUE;
	TRACE("%s - dest unreachable\n", m_Name);
/*	long dest=pIpHeader->destIP;
	long source=pIpHeader->sourceIP;
	TRACE("IpHeader:\ndestIP=%d.%d.%d.%d\nsource=%d.%d.%d.%d\n",
		LOBYTE(LOWORD(dest)), HIBYTE(LOWORD(dest)), LOBYTE(HIWORD(dest)), HIBYTE(HIWORD(dest)), 
		LOBYTE(LOWORD(source)), HIBYTE(LOWORD(source)), LOBYTE(HIWORD(source)), HIBYTE(HIWORD(source))); 
*/
	LeaveCritical();
}

void CStation::QueryTmi()
{
	CTime CurrentTime = CTime::GetCurrentTime();
	m_LastStatQueryTime = CurrentTime;
	tlmHeader Command;
	Command.command = htons(CMD_TLM_GET);
	Command.signature = htonl(SIGNATURE_FLAG);
	Command.tlmParamsMask = htonl(MODEM_PARAMS_FLAG | BST_PARAMETERS_FLAG | IP_STAT_FLAG | UDP_STAT_FLAG | NET_DEVICES_FLAG);
	Command.serialNumber = htons(3);

	SendTmiQuery((char *)&Command, sizeof(Command));
}

void CStation::PseudoQueryTmi()
{
	EnterCritical();
	m_bTmiPseudoQueried = TRUE;
	m_LastStatQueryTime = CTime::GetCurrentTime();
	LeaveCritical();
}

void CStation::SendTmiQuery
(
	char *pBuffer,
	int length
)
{
	unsigned int nSentBytes = 0;
	EnterCritical();
	m_LastQueryTime = CTime::GetCurrentTime();
	m_pTmiSocket->WriteTo(pBuffer, length, nSentBytes, m_IpAddressSP, m_TmiIpPort);
//	m_TmiSocket.ReadOverlapped(m_szBufferForTMI, sizeof(m_szBufferForTMI), m_TmiIpPort, this, TmiPacketReceived);

	m_bTmiPseudoQueried = FALSE;
	LeaveCritical();
}

CTime CStation::GetNextStatQueryTime()
{
	CTime NextTime, CurrentTime = CTime::GetCurrentTime();
	EnterCritical();
	if (m_LastStatQueryTime == CurrentTime)
	{
		if (m_LastStatReplyTime < m_LastStatQueryTime)
			NextTime = CurrentTime + m_RequeryPeriod;
		else
			NextTime = CurrentTime + m_QueryPeriod;
	}
	else if (m_LastStatReplyTime < m_LastStatQueryTime && CurrentTime - m_LastStatQueryTime > m_RequeryPeriod)
		NextTime = CurrentTime;
	else if (m_LastStatReplyTime < m_LastStatQueryTime)
	{
		NextTime = m_LastQueryTime;
		while (NextTime < CurrentTime)
			NextTime += m_RequeryPeriod;
	}
	else
	{
		NextTime = m_LastQueryTime;
		while (NextTime < CurrentTime)
			NextTime += m_QueryPeriod;
	}
	LeaveCritical();
	return NextTime;
}

CTimeSpan CStation::GetTimeTillNextStatQuery()
{
	CTime NextTime = GetNextStatQueryTime();
	CTimeSpan TimeTill = NextTime - CTime::GetCurrentTime();
	return TimeTill;
}

BOOL CStation::IsTmiChangedSince(CTime Time)
{
	return (m_LastReplyTime >= Time);
}


CString CStation::FormatLastStatReplyTime()
{
	EnterCritical();
	CString string = m_LastStatReplyTime.Format("%d.%m.%Y %H:%M:%S");
	LeaveCritical();
	return string;
}

CString CStation::FormatNextStatQueryTime()
{
	CTime NextQueryTime = GetNextStatQueryTime();
	CString string = NextQueryTime.Format("%d.%m.%Y %H:%M:%S");
	return string;
}

CString CStation::FormatTimeElapsedSinceLastStatReply()
{
	EnterCritical();
	CTimeSpan ElapsedTime = CTime::GetCurrentTime() - m_LastStatReplyTime;
	LeaveCritical();

	CString string = ElapsedTime.Format("%D-%H:%M:%S");
	return string;
}

CString CStation::FormatTimeTillNextStatQuery()
{
	CTimeSpan TimeTill = GetTimeTillNextStatQuery();
	CString string = TimeTill.Format("%H:%M:%S");
	return string;
}

void CStation::SetIpAddressOfSP(unsigned long IpAddress)
{
	EnterCritical();
	m_IpAddressSP = IpAddress;
	LeaveCritical();
}

void CStation::SetIpAddressOfRCh(unsigned long IpAddress)
{
	EnterCritical();
	m_IpAddressRCh = IpAddress;
	LeaveCritical();
}

unsigned long CStation::GetIpAddressOfSP()
{
	EnterCritical();
	unsigned long IpAddress = m_IpAddressSP;
	LeaveCritical();
	return IpAddress;
}

unsigned long CStation::GetIpAddressOfRCh()
{
	EnterCritical();
	unsigned long IpAddress = m_IpAddressRCh;
	LeaveCritical();
	return IpAddress;
}

int CStation::GetPingDelaySP()
{
	EnterCritical();
	int delay = m_PingDelaySP;
	LeaveCritical();
	return delay;
}

int CStation::GetPingDelayRCh()
{
	EnterCritical();
	int delay = m_PingDelayRCh;
	LeaveCritical();
	return delay;
}

BOOL CStation::IsSpConnected()
{
	EnterCritical();
	m_bConnectedSP = !m_bPingEnabled || (m_LastPingReplySP >= m_LastPingTimeSP) ||
		(((CTime::GetCurrentTime() <= m_LastPingTimeSP + CTimeSpan(0,0,0,5)) && m_bConnectedSP));
	BOOL bConnected = m_bConnectedSP;
	if (!m_bConnectedSP)
		m_PingDelaySP = -1;
	LeaveCritical();
	return bConnected;
}

BOOL CStation::IsRChConnected()
{
	return TRUE;
	/*EnterCritical();
	m_bConnectedRCh = (m_LastPingReplyRCh >= m_LastPingTimeRCh) ||
		(((CTime::GetCurrentTime() <= m_LastPingTimeRCh + CTimeSpan(0,0,0,5)) && m_bConnectedRCh));
	BOOL bConnected = m_bConnectedRCh;
	if (!m_bConnectedRCh)
		m_PingDelayRCh = -1;
	LeaveCritical();
	return bConnected;*/
}

BOOL CStation::IsTmiConnected()
{
	EnterCritical();
	m_bConnectedTMI = (m_LastStatReplyTime >= m_LastStatQueryTime) ||
		((CTime::GetCurrentTime() <= m_LastStatQueryTime + CTimeSpan(0,0,0,5)) && m_bConnectedTMI);
	BOOL bConnected = m_bConnectedTMI || !m_bQueryEnabled || m_bTmiPseudoQueried;
	if (!bConnected && !m_bTmiConnectionFailedReported)
		ReportTMIConnectionFailed();
	LeaveCritical();
	return bConnected;
}

void CStation::SetTmiQueryPeriod(unsigned Period)
{
	EnterCritical();
	m_QueryPeriod = CTimeSpan(0, 0, Period, 0);
	LeaveCritical();
}

void CStation::SetTmiRequeryPeriod(unsigned Period)
{
	EnterCritical();
	m_RequeryPeriod = CTimeSpan(0, 0, Period, 0);
	LeaveCritical();
}

CTimeSpan CStation::GetQueryPeriod()
{
	EnterCritical();
	CTimeSpan Period = m_QueryPeriod;
	LeaveCritical();
	return Period;
}

CTimeSpan CStation::GetRequeryPeriod()
{
	EnterCritical();
	CTimeSpan Period = m_RequeryPeriod;
	LeaveCritical();
	return Period;
}

void CStation::EnableStatQuery(BOOL bEnabled)
{
	EnterCritical();
	m_bQueryEnabled = bEnabled;
	LeaveCritical();
}

void CStation::EnablePing(BOOL bEnabled)
{
	EnterCritical();
	m_bPingEnabled = bEnabled;
	LeaveCritical();
}

CString CStation::GetModemStatusFileName()
{
	CString str;
	str.Format("%s_Modem_Status.txt", GetName());
	return str;
}

CString CStation::GetBstStatusFileName()
{
	CString str;
	str.Format("%s_Bst_Status.txt", GetName());
	return str;
}

CString CStation::GetIpStatisticsFileName()
{
	CString str;
	str.Format("%s_IP_Stat.txt", GetName());
	return str;
}

CString CStation::GetIpRchStatisticsFileName()
{
	CString str;
	str.Format("%s_IP_Rch_Stat.txt", GetName());
	return str;
}

CString CStation::GetUdpStatisticsFileName()
{
	CString str;
	str.Format("%s_UDP_Stat.txt", GetName());
	return str;
}

CString CStation::GetNetDeviceStatisticsFileName
(
	int InterfaceNo
)
{
	CString str;
	EnterCritical();
	str.Format("%s_%s.txt", m_Name, m_PpdTmi.GetNetDeviceName(InterfaceNo));
	LeaveCritical();
	return str;
}

CString CStation::GetIpStatisticsText
(
	BOOL bIncremental
)
{
	CString str;
	EnterCritical();
	str = m_PpdTmi.GetIpStatisticsText(bIncremental);
	LeaveCritical();
	return str;
}

CString CStation::GetUdpStatisticsText
(
	BOOL bIncremental
)
{
	CString str;
	EnterCritical();
	str = m_PpdTmi.GetUdpStatisticsText(bIncremental);
	LeaveCritical();
	return str;
}

CString CStation::GetNetDeviceStatisticsText
(
	unsigned InterfaceNo,
	BOOL bIncremental
)
{
	CString str;
	EnterCritical();
	str = m_PpdTmi.GetNetDeviceStatisticsText(InterfaceNo, bIncremental);
	LeaveCritical();
	return str;
}

void CStation::TelnetSymbolReceived(char symbol)
{
	switch (symbol)
	{
		case '\r': m_FromTelnet += "\r\n"; break;
		case '\n': break;
		default: m_FromTelnet += symbol; break;
	}
	m_bTelnetTextChanged = TRUE;
}

BOOL CStation::IsPingEnabled()
{
	EnterCritical();
	BOOL bEnabled = m_bPingEnabled;
	LeaveCritical();
	return bEnabled;
}

BOOL CStation::IsQueryEnabled()
{
	EnterCritical();
	BOOL bEnabled = m_bQueryEnabled;
	LeaveCritical();
	return bEnabled;
}

CTimeSpan CStation::GetPingPeriod()
{
	EnterCritical();
	CTimeSpan Period = m_PingPeriod;
	LeaveCritical();
	return Period;
}

void CStation::SetPingPeriod(unsigned Period)
{
	EnterCritical();
	m_PingPeriod = CTimeSpan(0, 0, 0, Period);
	LeaveCritical();
}

int CStation::BstGetAdcValue()
{
	EnterCritical();
	int ret = m_PpdTmi.BstGetAdcValue();
	LeaveCritical();
	return ret;
}

int CStation::BstGetAdcHexValue()
{
	EnterCritical();
	int ret = m_PpdTmi.BstGetAdcHexValue();
	LeaveCritical();
	return ret;
}

int CStation::BstGetAdcChannel()
{
	EnterCritical();
	int ret = m_PpdTmi.BstGetAdcChannel();
	LeaveCritical();
	return ret;
}

BOOL CStation::BstIsTransmitterPowerOn()
{
	EnterCritical();
	BOOL ret = m_PpdTmi.BstIsTransmitterPowerOn();
	LeaveCritical();
	return ret;
}

BOOL CStation::BstIsReceiverPowerOn()
{
	EnterCritical();
	BOOL ret = m_PpdTmi.BstIsReceiverPowerOn();
	LeaveCritical();
	return ret;
}

//static
void CALLBACK CStation::TmiPacketReceived(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED pOverlapped, DWORD dwFlags)
{
}



