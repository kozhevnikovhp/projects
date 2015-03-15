#ifndef __DEVICE_H_INCLUDED__
#define __DEVICE_H_INCLUDED__
#include "Terminal.h"
#include <stdio.h>

enum MC_ErrorCode {
	MC_OK,							// all right
	MC_GENERIC_ERROR,				// Generic error
	MC_DEVICE_NOT_CONTROLLABLE,		// M&C port not opened
	MC_DEVICE_NOT_RESPONDING,		// M&C port opened, but device is not responding (for example, power off or not electrically connected)
	MC_INVALID_COMMAND,				// invalid command
	MC_INVALID_PARAMETERS,			// invalid parameter(s)
	MC_INVALID_ADDRESS,				// invalid address
	MC_BAD_CRC,						// bad CRC
	MC_COMMAND_NOT_SUPPORTED		// this command is not supported by given device
};

enum MC_ControlType {
	MC_UNKNOWN_TYPE,
	MC_SERIAL_PORT,
	MC_PARALLEL_PORT,
	MC_TELNET,
	MC_SNMP,
	MC_GPIB
};

class CSerialPort;

class CDevice //: public CGuiObject
{
public:
// Construction-destruction
	CDevice();
	virtual ~CDevice();
	
// Public methods
	static inline SetLogFile(FILE *pLogFile) { m_pLogFile = pLogFile; }
	void EnableLogFile(BOOL bEnabled) { m_bLogFileEnabled = bEnabled; }
	BOOL IsLogFileEnabled() { return m_bLogFileEnabled; }
	virtual char *GetName() = 0; // pure virtual - must be overriden

// Monitor and Control
	virtual CTerminal *GetTerminal() = 0; // pure virtual - must be overriden
	virtual CSerialPort *GetSerialPort() { return NULL; }
	virtual BOOL HasMCBusAddress() { return FALSE; }
	virtual unsigned int GetMCBusAddress() { return 0; }
	virtual void SetMCBusAddress(unsigned int Address) {}
	virtual unsigned int GetMinMCBusAddressScanning() { return 0; }
	virtual unsigned int GetMaxMCBusAddressScanning() { return 0; }
	virtual BOOL CheckMCBusAddress(unsigned int Address) { return FALSE; }
	virtual char *GetConnectionName() { return GetTerminal()->GetConnectionName(); }
	virtual MC_ErrorCode ReadReplyUntilPrompt() = 0; // pure virtual - must be overriden
	virtual unsigned char *GetMCReplyData() = 0; // pure virtual - must be overriden
	virtual unsigned int GetMCReplyDataLength() = 0; // pure virtual - must be overriden
	void SetConnectionName(const char *pszConnectionName) { GetTerminal()->SetConnectionName(pszConnectionName); }
	char *GetConnectionStatus() { return GetTerminal()->GetConnectionStatus(); }
	inline MC_ControlType GetControlType() { return m_ControlType; }
	inline void SetControlType(MC_ControlType Type) { m_ControlType = Type; OnControlTypeSet(); }
	inline BOOL IsControllable() { return m_bControllable; }
	virtual void SetLocality(unsigned long ConnectionTime);
	virtual BOOL IsLocallyControlled();
	virtual unsigned int GetMaxLocalConnectionTime() { return 1000; } // Max waiting time of connection establish to consider this connection as LOCAL
	BOOL Connect(const char *pszName);
	BOOL Connect();
	BOOL Connect(unsigned int nPort);
	BOOL Reconnect();
	BOOL Disconnect();

	virtual void OnMcSymbolReceived(char cSymbol) {}
	virtual void OnControlTypeSet() {}
	BOOL SendMC(char *pszBufferToSend, unsigned int nBytesToSend, unsigned int &nSentBytes);
	BOOL SendMC(char *pszBufferToSend, unsigned int nBytesToSend);
	virtual void OnBeforeSendMC(char *pszBufferToSend, unsigned int nBytesToSend) {} // if nesessary stores info from command
	unsigned int PumpMC(char *pszBuffer, unsigned int nBufferSize);

// Overridable methods

// Common device commands
	virtual MC_ErrorCode Identify(BOOL &bIdentified) { bIdentified = FALSE; return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanBeControlledByComPort() { return FALSE; }
	virtual BOOL CanBeControlledByTcpIp() { return FALSE; }
	virtual BOOL CanBeControlledBySNMP() { return FALSE; }
	virtual BOOL CanBeControlledByGPIB() { return FALSE; }
	virtual BOOL CanAsciiControl() { return FALSE; }
	virtual void OnMcConnected() {}
	virtual void OnMcDisconnected() {}
	virtual IsStation() { return FALSE; }
	virtual BOOL IsHuman() { return FALSE; }	// human-controlled device
	virtual BOOL IsModem() { return FALSE; }
	virtual BOOL IsTranceiver() { return FALSE; }
	virtual BOOL IsMeasureDevice() { return FALSE; }
	virtual BOOL IsUpConverter() { return FALSE; }
	virtual BOOL IsDownConverter() { return FALSE; }
	virtual BOOL IsDeviceSubstitutor() { return FALSE; }
	virtual BOOL CanWarmReset() { return FALSE; }
	virtual MC_ErrorCode WarmReset() { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanTotalReset() { return FALSE; }
	virtual MC_ErrorCode TotalReset() { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetUpTime(unsigned int &Time) { Time = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetDeviceStatusString(char *&pszString) { pszString=""; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString) { pszString=""; return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanWriteConfiguration() { return FALSE; }
	virtual MC_ErrorCode WriteConfiguration() { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanFactoryPreset() { return FALSE; }
	virtual MC_ErrorCode FactoryPreset() { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanGetUnitName() { return FALSE; }
	virtual MC_ErrorCode GetUnitName(char *&pszString) { pszString=""; return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanSetUnitName() { return FALSE; }
	virtual MC_ErrorCode SetUnitName(char *pszString) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetModelName(std::string &strModel) { strModel = ""; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetSerialNo(unsigned int &serialNo) { serialNo = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetSoftwareVersion(std::string &strVersion) { strVersion = ""; return MC_COMMAND_NOT_SUPPORTED; }

	virtual MC_ErrorCode ConfigureForCommonTests() { return MC_COMMAND_NOT_SUPPORTED; }

// Non Volatile RAM
	virtual BOOL CanControlNvRam() { return FALSE; }
	virtual MC_ErrorCode IsNvRamEnabled(BOOL &bEnabled) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode EnableNvRam(BOOL &bEnabled) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanControlParamsSavingInNvRamRightAfterSetting() { return FALSE; }
	virtual MC_ErrorCode IsNvRamSavingRightAfterSettingEnabled(BOOL &bEnabled) { bEnabled = m_bParamSavingInNvRamRightAfterSettingEnabled; return MC_OK; }
	virtual MC_ErrorCode EnableNvRamSavingRightAfterSetting(BOOL &bEnabled) { m_bParamSavingInNvRamRightAfterSettingEnabled = bEnabled; return MC_OK; }

// public members

// protected methods

// protected members
protected:
	static FILE *m_pLogFile;

	BOOL m_bControllable;
	MC_ControlType m_ControlType;
	BOOL m_bParamSavingInNvRamRightAfterSettingEnabled;
	unsigned long m_ConnectionTime;
	BOOL m_bLogFileEnabled;
};

#endif //__DEVICE_H_INCLUDED__