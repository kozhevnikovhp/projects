#ifndef __STELA_M_H_INCLUDED__
#define __STELA_M_H_INCLUDED__

#include "SerialPort.h"
#include "TelnetSockets.h"
#include "Snmp_Stuff.h"
#include "Modem.h"

class CSerialPort_SDMS : public CSerialPort
{
	virtual char *GetControlString() { return "9600,n,8,1,x"; }
	virtual unsigned int GetReadTimeout() { return 5000; }
	virtual unsigned int GetWriteTimeout() { return 1000; }
};

typedef struct
{
	__int64 InputPackets;
	__int64 InputBytes;
	__int64 InputErrors;
	__int64 InputDrops;
	__int64 OutputPackets;
	__int64 OutputBytes;
	__int64 OutputErrors;
	__int64 OutputDrops;
} SInterfaceStatus;

typedef struct
{
	__int64 ReceiveShort;
	__int64 ReceiveGiant;
	__int64 ReceiveNonOctet;
	__int64 ReceiveCRC;
	__int64 ReceiveOverrun;
	__int64 ReceiveCollision;
	__int64 ReceiveDiscarded;
	__int64 SendUnderrun;
	__int64 SendLateCollisions;
	__int64 SendDeffered;
	__int64 SendLostCarrier;
	__int64 SendHeartBeat;
	__int64 SendRetries;
	__int64 SendRetransmitLimits;
} SEthernetErrors;

typedef struct
{
	__int64 ReceiveCRC;
	__int64 ReceiveOverrun;
	__int64 ReceiveLostCarrier;
	__int64 ReceiveFrameViolations;
	__int64 ReceiveNonOctet;
	__int64 ReceiveAbortSequence;
	__int64 ReceiveDpll;
	__int64 SendUnderrun;
	__int64 SendLostCTS;
	__int64 SendKeepaliveState;
	__int64 SendRxSeq;
	__int64 SendTxSeq;
	__int64 SendLastPoll;
} SSerialErrors;

typedef struct
{
	__int64 TotalPacketsReceived;
	__int64 BadCRC;
	__int64 PacketsTooShort;
	__int64 NotEnoughData;
	__int64 IpHeaderTooShort;
	__int64 IpPacketSmallerThanIpHeader;
	__int64 FragmentsReceived;
	__int64 FragmentsDropped;
	__int64 FragmentsTimeouted;
	__int64 PacketsForwarded;
	__int64 PacketsNotForwarded;
	__int64 RedirectsSent;
	__int64 UnknownProtocol;
	__int64 DeliveredToKernel;
	__int64 LocallyGeneratedOutput;
	__int64 LostPackets;
	__int64 Reassembled;
	__int64 Fragmented;
	__int64 OutputFragments;
	__int64 CannotFragment;
	__int64 BadIpOptions;
	__int64 NoRouteDrops;
	__int64 BadIpVersion;
	__int64 RawIpPacketsGenerated;
	__int64 TooLongPackets;
	__int64 AccessDeniedByFW;
	__int64 SwitchedToAnotherInterface;
	__int64 DroppedWhileSwitching;
} SIpStatistics;

typedef struct
{
	__int64 CallsToIcmpError;
	__int64 NoErrorBecauseOldIpTooShort;
	__int64 NoErrorBecauseOldPacketWasIcmp;
	__int64 IcmpCodeOutOfRangeReceived;
	__int64 TooShortPackets;
	__int64 BadChecksum;
	__int64 CalculatedBoundsMismatch;
	__int64 NumberOfResponses;
	__int64 BroadcastMulticastEchoRequestsDropped;
	__int64 BroadcastMulticastTimestampRequestsDropped;
	__int64 RcvdEchoReply, SentEchoReply;
	__int64 RcvdUnreach, SentUnreach;
	__int64 RcvdSourceQuench, SentSourceQuench;
	__int64 RcvdRedirect, SentRedirect;
	__int64 RcvdEcho, SentEcho;
	__int64 RcvdRouterAdvert, SentRouterAdvert;
	__int64 RcvdRouterSolicit, SentRouterSolicit;
	__int64 RcvdTtlExceed, SentTtlExceed;
	__int64 RcvdParamProbe, SentParamProbe;
	__int64 RcvdTimeStamp, SentTimeStamp;
	__int64 RcvdTimeStampReply, SentTimeStampReply;
	__int64 RcvdInformation, SentInformation;
	__int64 RcvdInformationReply, SentInformationReply;
	__int64 RcvdMask, SentMask;
	__int64 RcvdMaskReply, SentMaskReply;
} SIcmpStatistics;

typedef struct
{
	__int64 TotalInputPackets;
	__int64 PacketsShorterThanHeader;
	__int64 ChecksumErrors;
	__int64 DataLengthLargerThanPacket;
	__int64 NoSocketOnPort;
	__int64 OfAboveArrivedAsBroadcast;
	__int64 NotDeliveredInputSocketFull;
	__int64 InputPacketsMissingPcbCache;
	__int64 InputPacketsNotForHashedPcb;
	__int64 TotalOutputPackets;
} SUdpStatistics;

typedef struct
{
	__int64 ConnectionsInitiated;
	__int64 ConnectionsAccepted;
	__int64 ConnectionsEstablished;
	__int64 ConnectionsDropped;
	__int64 EmbryonicConnectionsDropped;
	__int64 ConnectionsClosed;
	__int64 SegsWhereWeTriedToGetRTT;
	__int64 TimesWeSucceeded;
	__int64 DelayedAcksSent;
	__int64 ConnectionsDroppedInRxmtTimeout;
	__int64 RetransmitTimeouts;
	__int64 PersistTimeouts;
	__int64 KeepaliveTimeouts;
	__int64 KeepaliveProbesSent;
	__int64 ConnectionsDroppedInKeepalive;
	__int64 TotalPacketsSent;
	__int64 DataPacketsSent;
	__int64 DataBytesSent;
	__int64 DataPacketsRetransmitted;
	__int64 DataBytesRetransmitted;
	__int64 AckOnlyPacketsSent;
	__int64 WindowProbesSent;
	__int64 PacketsSentWithUrgOnly;
	__int64 WindowUpdateOnlyPacketsSent;
	__int64 ControlPacketsSent;
	__int64 TotalPacketsReceived;
	__int64 PacketsReceivedInSequence;
	__int64 BytesReceivedInSequence;
	__int64 PacketsReceivedWithChecksumError;
	__int64 PacketsReceivedWithBadOffset;
	__int64 PacketsReceivedTooShort;
	__int64 DuplicateOnlyPacketsReceived;
	__int64 DuplicateOnlyBytesReceived;
	__int64 PacketsWithSomeDuplicateData;
	__int64 DupBytesInPartDupPackets;
	__int64 OutOfOrderPacketsReceived;
	__int64 OutOfOrderBytesReceived;
	__int64 PacketsWithDataAfterWindow;
	__int64 BytesRcvdAfterWindow;
	__int64 PacketsRcvdAfterClose;
	__int64 RcvdWindowProbePackets;
	__int64 RcvdDuplicateAcks;
	__int64 RcvdAcksForUnsentData;
	__int64 RcvdAckPackets;
	__int64 BytesAckedByRcvdAcks;
	__int64 RcvdWindowUpdatePackets;
	__int64 SegmentsDroppedDueToPAWS;
	__int64 TimesHdrPredictOkForAcks;
	__int64 TimesHdrPredictOkForDataPkts;
	__int64 PcbCacheMisses;
	__int64 TimesCachedRTTInRouteUpdated;
	__int64 TimesCachedRTTVarUpdated;
	__int64 TimesCachedSsThreshUpdated;
	__int64 TimesRTTIninitializedFromRoute;
	__int64 TimesRTTVarInitializedFromRT;
	__int64 TimesSsThreshInitializedFromRT;
	__int64 TimeoutInPersistState;
	__int64 BogusSYNPrematureACK;
	__int64 ResendsDueToMTUDiscovery;
	__int64 ListenQueueOverflows;
} STcpStatistics;

class CCheckControlability
{
public:
	CCheckControlability()
	{
		m_bEnabled = FALSE;
		m_bReported = FALSE;
		m_CurrentPass = 0;
		m_Period = 5;
	}
	virtual ~CCheckControlability() {}

// public methods
	void EnableControlabilityCheck(BOOL bEnable, unsigned int Period) { m_bEnabled = bEnable; m_Period = Period; if (m_Period <1) m_Period = 1; }
	BOOL IsControlabilityCheckEnabled() { return m_bEnabled; }
	void LostControlabilityReported(BOOL bReported) { m_bReported = bReported; }
	BOOL IsLostControlabilityReported() { return m_bReported; }
	BOOL Pass()
	{
		++m_CurrentPass;
		return ((m_CurrentPass % m_Period) == 0);
	}

// protected members
	BOOL m_bEnabled;
	BOOL m_bReported;
	unsigned int m_CurrentPass;
	unsigned int m_Period;
};

class CConnectionFailureReport
{
public:
	CConnectionFailureReport()
	{
		m_bReported = FALSE;
	}
	virtual ~CConnectionFailureReport() {}

// public methods
	BOOL IsConnectionFailureReported() { return m_bReported; }
	void ConnectionFailureReported() { m_bReported = TRUE; }
	void ConnectionFailureUnReported() { m_bReported = FALSE; }

// protected members
	BOOL m_bReported;
};


class CParamHolder
{
public:
	CParamHolder()
	{
		m_bUpTime = FALSE;
		m_bLNB = FALSE;
		m_bBUC = FALSE;
		m_bSerialIface = FALSE;
		m_bEthernetIface = FALSE;
		m_bPingEnabled = FALSE;
		m_bIpStatistics = FALSE;
		m_bMonoblock = FALSE;
	}
	virtual ~CParamHolder() {}

// public methods
	BOOL IsUpTimeTmiNeeded() { return m_bUpTime; }
	void NeedUpTimeTmi() { m_bUpTime = TRUE; }
	BOOL IsLnbStatusNeeded() { return m_bLNB; }
	void NeedLnbStatus() { m_bLNB = TRUE; }
	BOOL IsBucStatusNeeded() { return m_bBUC; }
	void NeedBucStatus() { m_bBUC = TRUE; }
	BOOL IsSerialTmiNeeded() { return m_bSerialIface; }
	void NeedSerialTmi() { m_bSerialIface = TRUE; }
	BOOL IsEthernetTmiNeeded() { return m_bEthernetIface; }
	void NeedEthernetTmi() { m_bEthernetIface = TRUE; }
	BOOL IsIpStatisticsNeeded() { return m_bIpStatistics; }
	void NeedIpStatistics() { m_bIpStatistics = TRUE; }
	BOOL IsPingEnabled() { return m_bPingEnabled; }
	void EnablePing() { m_bPingEnabled = TRUE; }
	BOOL IsMonoblockTMINeeded() { return m_bMonoblock; }
	void NeedMonoblockTMI() { m_bMonoblock = TRUE; }

// protected members
	BOOL m_bUpTime;
	BOOL m_bLNB;
	BOOL m_bBUC;
	BOOL m_bMonoblock;
	BOOL m_bSerialIface;
	BOOL m_bEthernetIface;
	BOOL m_bIpStatistics;
	BOOL m_bPingEnabled;
};

class CVsatSnmpSocket : public CTerminal, public CSnmpSocket
{
//Construction/destruction
public:
	CVsatSnmpSocket();
	virtual ~CVsatSnmpSocket();

// Public methods
	LOGICAL SendGetRequest(const std::string &strCommunity, const cSnmpOID &OID);
	LOGICAL SendSetIntegerRequest(const std::string &strCommunity, const cSnmpOID &OID, int iValue);
	LOGICAL SendSetUnsignedIntegerRequest(const std::string &strCommunity, const cSnmpOID &OID, unsigned int uiValue);

// Public overridables

	// CTerminal* interface
	virtual LOGICAL Connect(const char *pszConnectionName);
	virtual void Disconnect() {} // stub (UDP - connectionless protocol)
	virtual char *GetConnectionStatus() { return ""; }
	virtual LOGICAL Write(void *pszBuffer, unsigned int BytesToWrite, unsigned int &nWrittenBytes);
	virtual LOGICAL Read(void *pszBuffer, unsigned int BytesToRead, unsigned int &nReadBytes);
	virtual LOGICAL PeekInputSymbols(void *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes);
	virtual LOGICAL SetReadTimeout(DWORD a_dwTimeout);
	virtual LOGICAL SetWriteTimeout(DWORD a_dwTimeout);

// Protected members
	IPADDRESS_TYPE m_IpAddress;
	int m_RequestID;
};

class CSdmsAPI : public CModem, public CCheckControlability, public CConnectionFailureReport,
					public CParamHolder
{
public:
// Construction-destruction
	CSdmsAPI();
	virtual ~CSdmsAPI();
	
// Public methods
// Overridable methods

// Common device commands
	virtual IsStation() { return TRUE; }
	virtual CTerminal *GetTerminal();
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual BOOL CanBeControlledByTcpIp() { return TRUE; }
	virtual BOOL CanAsciiControl() { return TRUE; }
	virtual void OnMcConnected();
	virtual MC_ErrorCode Command(char *pszCommand, int WaitTime = 0);
	virtual MC_ErrorCode GetUpTime(unsigned int &Time) = 0;
	virtual BOOL CanTotalReset() { return TRUE; }
	virtual MC_ErrorCode TotalReset();
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);

	virtual int GetModulatorsNumber() { return 1; }
	virtual int GetDemodulatorsNumber() { return 1; }

// Monitor and Control methods
	virtual MC_ErrorCode ModemPSM() = 0;
	virtual void OnMcSymbolReceived(char cSymbol);
	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned char *GetMCReplyData() { return (unsigned char *)m_pszReply; }
	virtual unsigned int GetMCReplyDataLength() { return strlen(m_pszReply); }
	char *GetMCReply();
	virtual MC_ErrorCode IsMCConnectionAlive();
	MC_ErrorCode Exit();
	MC_ErrorCode ConfigureTerminal();
	virtual MC_ErrorCode ConfigureEthernet();
	virtual MC_ErrorCode ConfigureSerial();
	MC_ErrorCode Enable() { return Command("enable"); }
	MC_ErrorCode Disable() { return Command("disable"); }
	MC_ErrorCode Reload();
	MC_ErrorCode GoTop();

// Modulation type
	virtual int GetRModulationTypeCount();
	virtual const char *GetRModulationTypeName(int Type);

	virtual int GetTModulationTypeCount();
	virtual const char *GetTModulationTypeName(int Type);

// FEC
	virtual int GetRFecModesCount();
	virtual const char *GetRFecModeName(int Mode);
	virtual MC_ErrorCode GetRFecMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRFecMode(int &Mode, int Demodulator);

	virtual int GetTFecModesCount();
	virtual const char *GetTFecModeName(int Mode);
	virtual MC_ErrorCode GetTFecMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTFecMode(int &Mode, int Modulator);

// Demodulator
// IF params and status
	virtual MC_ErrorCode GetRIfParams(CDemIfParams &Params, int Demodulator);
	virtual MC_ErrorCode GetRIfStatus(CDemIfStatus &Status, int Demodulator);
	virtual unsigned int GetMinRFrequency() { return 950000; } //  L-band
	virtual unsigned int GetMaxRFrequency() { return 1750000; } //  L-band
	virtual unsigned int GetMinTFrequency() { return 950000; } //  L-band
	virtual unsigned int GetMaxTFrequency() { return 1750000; } //  L-band
	virtual double GetMinComfortLevel() { return -80; }
	virtual double GetMaxComfortLevel() { return -30; }
	virtual BOOL CanGetInputLevel() { return TRUE; }
	virtual BOOL CanSetTFrequency() { return IsLocallyControlled(); }
	virtual BOOL CanSetRFrequency() { return IsLocallyControlled(); }
	virtual BOOL CanOutputLevel() { return IsLocallyControlled(); }
	virtual MC_ErrorCode GetRFrequency(unsigned int &Frequency, int Demodulator);
	virtual MC_ErrorCode SetRFrequency(unsigned int &Frequency, int Demodulator);

// Modulator
	virtual MC_ErrorCode GetTIfParams(CModIfParams &Params, int Modulator);
	MC_ErrorCode IsModulatorOn(BOOL &bOn);
	virtual MC_ErrorCode SetTFrequency(unsigned int &Frequency, int Modulator);
	virtual MC_ErrorCode SetOutputLevel(double &Level, int Modulator);

// Data params
	virtual MC_ErrorCode GetRDataParams(CModemDataParams &Params, int Demodulator);
	virtual MC_ErrorCode GetTDataParams(CModemDataParams &Params, int Modulator);
	virtual void ParseRDataShow(CModemDataParams &Params);
	virtual void ParseTDataShow(CModemDataParams &Params);
	void ParseDataRate(unsigned int &DataRate);
	void ParseCodeRate(CModemDataParams &Params);
	void ParseRClockSource(int &source);
	void ParseTClockSource(int &source);

// Data rate
	virtual BOOL CanDataRate() { return TRUE; }
	virtual int GetMaxDataRateBPSK() { return 1650000; }
	virtual int GetMinDataRateBPSK() { return 9600; }
	virtual int GetMaxDataRateQPSK() { return 3850000; }
	virtual int GetMinDataRateQPSK() { return 19200; }
	virtual MC_ErrorCode SetTDataRate(unsigned int &DataRate, int Modulator);
	virtual MC_ErrorCode SetRDataRate(unsigned int &DataRate, int Demodulator);

// Data clock source
	virtual BOOL CanSetRDataClockSource() { return TRUE; }
	virtual BOOL CanSetTDataClockSource() { return TRUE; }
	virtual int GetRDataClockSourcesCount();
	virtual const char *GetRDataClockSourceName(int source);
	virtual int GetTDataClockSourcesCount();
	virtual const char *GetTDataClockSourceName(int source);
	virtual MC_ErrorCode SetTDataClockSource(int &Source, int Modulator);
	virtual MC_ErrorCode SetRDataClockSource(int &Source, int Modulator);

	// LNB status
	virtual BOOL CanGetLnbStatus() { return TRUE; }
	virtual MC_ErrorCode GetLnbStatus(CLnbStatus &status, int Demodulator);
	virtual void ParseDemodulatorLnbShow(CLnbStatus &status);

// Scrambling/Descrambling
	virtual int GetScramblerModesCount() { return 0; }
	virtual const char *GetScramblerModeName(int Mode) { return ""; }

	virtual int GetDescramblerModesCount() { return 0; }
	virtual const char *GetDescramblerModeName(int Mode) { return ""; }

// BUC status
	virtual BOOL CanGetBucStatus() { return TRUE; }
	virtual MC_ErrorCode GetBucStatus(CBucStatus &status, int Modulator);
	virtual void ParseModulatorBucShow(CBucStatus &status);

// Output
	virtual double GetMinOutputLevel() { return -35.; }
	virtual double GetMaxOutputLevel() { return +5.; }
	virtual MC_ErrorCode GetOutputLevel(double &Level, int Modulator);

// Tranceiver
	virtual MC_ErrorCode GetTranceiverStatus();
	MC_ErrorCode GetTranceiverTemperature(double &T) { T = m_TranceiverTemperature; return m_TranceiverMCErrorCode; }
	MC_ErrorCode GetTranceiverPower(double &P) { P = m_TranceiverPower; return m_TranceiverMCErrorCode; };

// Interfaces
	MC_ErrorCode EnableInterface(BOOL bEnable);

// Ethernet interface
	virtual MC_ErrorCode RemoveEthernetIpAddress(const char *pszIpAddress, const char *pszIpMask = "");
	virtual MC_ErrorCode GetEthernetInterfaceStatus(SInterfaceStatus &status, SEthernetErrors &errors);
	virtual MC_ErrorCode GetEthernetIpAddress(char *pszIpAddress, char *pszIpSubnetMask);
	MC_ErrorCode GetEthernetIpAddress(unsigned long &IpAddress, unsigned long &IpSubnetMask);
	MC_ErrorCode SetEthernetIpAddress(const char *pszIpAddress, const char *pszIpMask = "");
	MC_ErrorCode SetEthernetIpAddress(unsigned long IpAddress, unsigned long IpSubnetMask);
	MC_ErrorCode RemoveAllEthernetIpAddresses();
	virtual MC_ErrorCode GetEthernetMacAddress(char *pszMacAddress);
	virtual MC_ErrorCode SetEthernetMacAddress(const char *pszMacAddress);
	MC_ErrorCode EnableEthernetInterface(BOOL &bEnable);

// Serial interface
	virtual MC_ErrorCode GetSerialInterfaceStatus(SInterfaceStatus &status, SSerialErrors &errors);
	MC_ErrorCode GetSerialIpAddress(char *pszIpAddress, char *pszIpSubnetMask);
	MC_ErrorCode GetSerialIpAddress(unsigned long &IpAddress, unsigned long &IpSubnetMask);
	MC_ErrorCode SetSerialIpAddress(const char *pszIpAddress, const char *pszIpMask = "");
	MC_ErrorCode SetSerialIpAddress(unsigned long IpAddress, unsigned long IpSubnetMask);
	MC_ErrorCode RemoveSerialIpAddress(const char *pszIpAddress, const char *pszIpMask = "");
	MC_ErrorCode EnableSerialInterface(BOOL &bEnable);

// IP - statistics
	MC_ErrorCode GetIpStatistics(SIpStatistics &statistics);

// ICMP - statistics
	MC_ErrorCode GetIcmpStatistics(SIcmpStatistics &statistics);

// UDP - statistics
	MC_ErrorCode GetUdpStatistics(SUdpStatistics &statistics);

// TCP - statistics
	MC_ErrorCode GetTcpStatistics(STcpStatistics &statistics);

// Software
	MC_ErrorCode SetupSoftware(const char *pszFtpServerIpAddress, const char *pszFileName, const char *pszFtpUserName, const char *pszFtpPassword);

// Startup-config
	MC_ErrorCode LoadStartupConfig(const char *pszFtpServerIpAddress, const char *pszFileName, const char *pszFtpUserName, const char *pszFtpPassword);

// Public members
public:

// Protected methods
protected:
	void CutPrompt();
	void DumpMCReply();
	virtual void ParseEthernetInterfaceStatus(SInterfaceStatus &status);
	virtual void ParseSerialInterfaceStatus(SInterfaceStatus &status);
	virtual void ParseEthernetInterfaceReceiveErrors(SEthernetErrors &errors);
	virtual void ParseEthernetInterfaceSendErrors(SEthernetErrors &errors);
	virtual void ParseSerialInterfaceReceiveErrors(SSerialErrors &errors);
	virtual void ParseSerialInterfaceSendErrors(SSerialErrors &errors);

// Protected members
	CTelnetSocket m_TelnetSocket;
	CSerialPort_SDMS m_SerialPort;
	double m_TranceiverTemperature;
	double m_TranceiverPower;
	MC_ErrorCode m_DemodulatorMCErrorCode;
	MC_ErrorCode m_ModulatorMCErrorCode;
	MC_ErrorCode m_TranceiverMCErrorCode;
	char m_pszReply[4096];
	char m_pszCommand[256];
	char m_szVersion[1024];
	char m_szStatus[1024];
	BOOL m_bReady;
	char m_cCurrentPrompt; // can be '>' or '#' depending on user mode
};

class CSdmsVsat : public CSdmsAPI
{
public:
// Common device commands
	virtual void OnMcConnected();
	virtual CTerminal *GetTerminal();
	virtual MC_ErrorCode GetUpTime(unsigned int &Time);
	virtual MC_ErrorCode GetDeviceStatusString(char *&pszString);
	virtual BOOL CanBeControlledBySNMP() { return TRUE; }
// Monitor and Control methods
	virtual MC_ErrorCode IsMCConnectionAlive();
// Output
	virtual MC_ErrorCode GetTIfParams(CModIfParams &Params, int Modulator);
//	MC_ErrorCode IsModulatorOn(BOOL &bOn);
	virtual MC_ErrorCode SetTFrequency(unsigned int &Frequency, int Modulator);
	virtual MC_ErrorCode GetTFrequency(unsigned int &Frequency, int Modulator);
	virtual MC_ErrorCode SetOutputLevel(double &Level, int Modulator);
	virtual MC_ErrorCode GetOutputLevel(double &Level, int Modulator);
	virtual BOOL CanOutputOnOff() { return (m_ControlType == MC_SNMP); }
	virtual MC_ErrorCode IsOutputOn(BOOL &bOn, int Modulator);
	virtual MC_ErrorCode TurnOutputOn(BOOL &bOn, int Modulator);
	// Modulator shift
	virtual BOOL CanSetModulatorShift() { return (m_ControlType == MC_SNMP); }
	virtual MC_ErrorCode GetModulatorShift(int &Shift, int Modulator);
	virtual MC_ErrorCode SetModulatorShift(int &Shift, int Modulator);

// Modulation type
	virtual MC_ErrorCode GetRModulationType(int &Type, int Demodulator);
	virtual MC_ErrorCode SetRModulationType(int &Type, int Demodulator);

	virtual MC_ErrorCode GetTModulationType(int &Type, int Modulator);
	virtual MC_ErrorCode SetTModulationType(int &Type, int Modulator);

	// Data params
	virtual MC_ErrorCode GetRDataParams(CModemDataParams &Params, int Demodulator);
	virtual MC_ErrorCode GetTDataParams(CModemDataParams &Params, int Modulator);


// Ethernet interface

// Public members
public:
	CVsatSnmpSocket m_SnmpSocket;
};

class CSdmsVsat4900L : public CSdmsVsat
{
public:
// Common device commands
	virtual char *GetName() { return m_pszName; }
// Monitor and Control methods
	virtual MC_ErrorCode ModemPSM() { return Command("device modem PSM-4900L 0"); }

// Reed-Solomon
	virtual const char *GetReedSolomonModeName(int Mode);
	virtual int GetReedSolomonModesCount();

	virtual MC_ErrorCode GetRReedSolomonMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRReedSolomonMode(int &Mode, int Demodulator);

	virtual MC_ErrorCode GetTReedSolomonMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTReedSolomonMode(int &Mode, int Modulator);

// Public members
public:
	static char *m_pszName;
};

class CSdmsVsatM500L : public CSdmsVsat
{
public:
// Common device commands
	virtual char *GetName() { return m_pszName; }
// Monitor and Control methods
	virtual MC_ErrorCode ModemPSM() { return Command("device modem PSM-500L 0"); }

// Reed-Solomon
	virtual const char *GetReedSolomonModeName(int Mode);
	virtual int GetReedSolomonModesCount();

	virtual MC_ErrorCode GetRReedSolomonMode(int &Mode, int Demodulator);
	virtual MC_ErrorCode SetRReedSolomonMode(int &Mode, int Demodulator);

	virtual MC_ErrorCode GetTReedSolomonMode(int &Mode, int Modulator);
	virtual MC_ErrorCode SetTReedSolomonMode(int &Mode, int Modulator);

// Public members
public:
	static char *m_pszName;
};

#endif // __STELA_M_H_INCLUDED__