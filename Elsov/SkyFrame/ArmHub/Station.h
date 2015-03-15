#ifndef __STATION_H_INCLUDED__
#define __STATION_H_INCLUDED__

#include "IpThreadedSockets.h"
#include "TelnetSockets.h"
#include "..\ppd\npStat\npStatLib.h"

class CStation;
class CTmiSocket;
class CPingSocket;

class CListOfObjects : public CObList
{
	DECLARE_SERIAL(CListOfObjects);
public :
	CListOfObjects() {}; 
	CListOfObjects(CListOfObjects &ListOfObjects) {}; 
	void Clear()
	{
		for (POSITION pos=GetHeadPosition(); pos; GetNext(pos))
			delete GetAt(pos);
		RemoveAll();
	};
	virtual ~CListOfObjects() { Clear(); }
};

class CPpdTmi : public CObject
{
	DECLARE_SERIAL(CPpdTmi);
public :
	// Данные-члены
	//Функции-члены
	CPpdTmi();
	~CPpdTmi();
	void Serialize(CArchive &);
	void ProcessPacket(char *pBuffer, int length);
	BOOL HaveModemStatus();
	BOOL HaveBstStatus();
	void PrintModemStatus(CFile *pFile);
	void PrintBstStatus(CFile *pFile);
	BOOL HaveIpStatistics();
	void PrintIpStatistics(CFile *pFile);
	void PrintIpRchStatistics(CFile *pFile);
	BOOL HaveUdpStatistics();
	void PrintUdpStatistics(CFile *pFile);
	inline int GetNetDeviceCount() { return m_NetDeviceHeader.amount; }
	CString GetNetDeviceName(int InterfaceNo);
	char *GetInterfaceRawName(int InterfaceNo);
	BOOL HaveNetDeviceStatistics();
	inline BOOL HaveRadiochannelStatistics() { return m_bHaveRadiochannelStatistics; }
	void PrintNetDeviceStatistics(CFile *pFile, int InterfaceNo);
	CString GetIpStatisticsText(BOOL bIncremental);
	CString GetUdpStatisticsText(BOOL bIncremental);
	CString GetNetDeviceStatisticsText(unsigned InterfaceNo, BOOL bIncremental);
	float GetSignalNoise();
	float GetAruValue();
	CString GetModemType();
	BOOL IsBPCHTransmitterLoaded();
	BOOL IsBPCHReceiverLoaded();

	// Bst
	int BstGetAdcValue() { return m_BstStatus.ADCValueMw; }
	int BstGetAdcHexValue() { return m_BstStatus.ADCValueHex; }
	int BstGetAdcChannel() { return m_BstStatus.ADCChannel; }
	BOOL BstIsTransmitterPowerOn() { return (m_BstStatus.powerFlags & BST_TRANSMITTER_ON_FLAG) != 0; }
	BOOL BstIsReceiverPowerOn() { return (m_BstStatus.powerFlags & BST_RECEIVER_ON_FLAG) != 0; }
protected:
	//Функции-члены
	void ReallocNetDevicesData(int nNetDevices);
	void FreeAllocatedDevices();
	// Данные-члены
	static netDeviceData m_ZeroNetDeviceStatistics;
	BOOL m_bHaveRadiochannelStatistics;
	CTime m_Time;
	tlmHeader m_Header;
	modemStatus m_ModemStatus;
	bstStatus m_BstStatus;
	ipStat m_IpStat, m_DeltaIpStat;
	udpStat m_UdpStat, m_DeltaUdpStat;
	int m_nNetDevices;
	netDeviceHdr m_NetDeviceHeader;
	netDeviceData *m_pNetDeviceData;
	netDeviceData *m_pDeltaNetDeviceData;
	int m_nMaxAllocatedDevices;
};

class CTmiSocket : public CUdpThreadedSocket
{
//Attributes
public:
	CTmiSocket(CStation *pStation) { m_pStation = pStation; Open(); }
	virtual ~CTmiSocket() {}
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
	virtual void OnDatagramReceived(unsigned long IpAddress, int DatagramLength);
// Protected members
	CStation *m_pStation;
};

class CStationTelnetSocket : public CAnsiTelnetSocket
{
//Attributes
public:
	CStationTelnetSocket() { m_pStation = NULL; }
	virtual ~CStationTelnetSocket() {}
// Public methods
	inline void SetStation(CStation *pStation) { m_pStation = pStation; }
// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
	virtual void OnSymbolToUser(char symbol);
// Protected members
	CStation *m_pStation;
private:
// Private methods
// Private overridables
// Private members
};

class CStation : public CObject
{
	DECLARE_SERIAL(CStation);
public :
	//Функции-члены
	CStation();
	CStation(CString name);
	CStation(CStation *pStation);
	~CStation();
	void QueryTmiSchedule();
	void SendPingSchedule();
	void Serialize(CArchive &);
	CString GetName();
	void SetName(CString name);
	inline CTelnetSocket *GetTelnetSocket() { return &m_TelnetSocket; }
	BOOL IsMyAddress(unsigned long IpAddress);
	BOOL IsMyPingEchoReply(unsigned long IpAddress);
	BOOL IsMyPingDestUnreachable(unsigned long IpAddress);
	BOOL IsPingEnabled();
	CTimeSpan GetPingPeriod();
	void ProcessPpdTmiDatagram(char *pBuffer, int length);
	void ProcessPingEchoReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned long SourceIP);
	void ProcessPingDestUnreachable(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader);
	void QueryTmi();
	void PseudoQueryTmi();
	BOOL IsAlive();
	BOOL IsQueryEnabled();
	CTime GetNextStatQueryTime();
	CTimeSpan GetTimeTillNextStatQuery();
	BOOL IsTmiChangedSince(CTime Time);
	CString FormatLastStatReplyTime();
	CString FormatNextStatQueryTime();
	CString FormatTimeElapsedSinceLastStatReply();
	CString FormatTimeTillNextStatQuery();
	CString GetIpStatisticsText(BOOL bIncremental);
	CString GetUdpStatisticsText(BOOL bIncremental);
	CString GetNetDeviceStatisticsText(unsigned InterfaceNo, BOOL bIncremental);
	void TelnetSymbolReceived(char symbol);
	CString &GetTextFromTelnet() {  m_bTelnetTextChanged = FALSE; return m_FromTelnet;}
	inline BOOL TelnetTextChanged() { return m_bTelnetTextChanged; }
	void SetTmiQueryPeriod(unsigned Period);
	void SetTmiRequeryPeriod(unsigned Period);
	void SetPingPeriod(unsigned Period);
	void EnableStatQuery(BOOL bEnabled);
	void EnablePing(BOOL bEnabled);
	inline int GetNetDeviceCount() { return m_PpdTmi.GetNetDeviceCount(); }
	inline CString GetNetDeviceName(int InterfaceNo) { return m_PpdTmi.GetNetDeviceName(InterfaceNo); }
	inline float GetSignalNoise() { return m_PpdTmi.GetSignalNoise(); }
	inline float GetAruValue() { return m_PpdTmi.GetAruValue(); }
	inline CString GetModemType() { return m_PpdTmi.GetModemType(); }
	inline BOOL IsBPCHTransmitterLoaded() { return m_PpdTmi.IsBPCHTransmitterLoaded(); }
	inline BOOL IsBPCHReceiverLoaded() { return m_PpdTmi.IsBPCHReceiverLoaded(); }
	void SetIpAddressOfSP(unsigned long IpAddress);
	void SetIpAddressOfRCh(unsigned long IpAddress);
	CTimeSpan GetQueryPeriod();
	CTimeSpan GetRequeryPeriod();
	unsigned long GetIpAddressOfSP();
	unsigned long GetIpAddressOfRCh();
	int GetPingDelaySP();
	int GetPingDelayRCh();

	// Bst
	int BstGetAdcValue();
	int BstGetAdcHexValue();
	int BstGetAdcChannel();
	BOOL BstIsTransmitterPowerOn();
	BOOL BstIsReceiverPowerOn();

	BOOL IsSpConnected();
	BOOL IsRChConnected();
	BOOL IsTmiConnected();
	static void CALLBACK TmiPacketReceived(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED pOverlapped, DWORD dwFlags);
	// Данные-члены
	static CIcmpThreadedSocket *m_pPingSocket;
	static short m_TmiIpPort;
protected:
	void Initialize();
	void EnterCritical();
	void LeaveCritical();
	void SendTmiQuery(char *pBuffer, int length);
	CString GetModemStatusFileName();
	CString GetBstStatusFileName();
	CString GetIpStatisticsFileName();
	void PrintModemStatus();
	void PrintBstStatus();
	void PrintIpStatistics();
	void PrintIpRchStatistics();
	CString GetIpRchStatisticsFileName();
	CString GetUdpStatisticsFileName();
	void PrintUdpStatistics();
	CString GetNetDeviceStatisticsFileName(int InterfaceNo);
	void PrintNetDeviceStatistics();
	void ReportTMIConnectionFailed();
	void ReportTMIConnectionRestored();
	CString GetLogFileName();

protected:
	CString m_Name;
	unsigned long m_IpAddressSP, m_IpAddressRCh; 
	BOOL m_bConnectedSP, m_bConnectedRCh, m_bConnectedTMI;
	BOOL m_bTmiPseudoQueried;
	BOOL m_bAlive;
	CRITICAL_SECTION m_Critical;
	unsigned int m_ConnectionThreadID;

	// TMI
	CUdpThreadedSocket *m_pTmiSocket;
	CTime m_LastQueryTime;
	CTime m_LastReplyTime;
	CTimeSpan m_QueryPeriod;
	CTimeSpan m_RequeryPeriod;
	BOOL m_bQueryEnabled;
	BOOL m_bTmiConnectionFailedReported, m_bTmiConnectionRestoredReported;

	CTime m_LastStatReplyTime;
	CTime m_LastStatQueryTime;

	CPpdTmi m_PpdTmi;

	// Ping
	CTime m_LastPingTime, m_LastPingTimeSP, m_LastPingTimeRCh;
	CTime m_LastPingReplySP, m_LastPingReplyRCh;
	int m_PingDelaySP, m_PingDelayRCh; 
	unsigned int m_PingDirectionFlag;
	BOOL m_bPingEnabled;
	CTimeSpan m_PingPeriod;

	CStationTelnetSocket m_TelnetSocket;
	CString m_FromTelnet;
	BOOL m_bTelnetTextChanged;
};

#endif //__STATION_H_INCLUDED__

