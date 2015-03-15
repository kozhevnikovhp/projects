#ifndef __NSGFRAD_H_INCLUDED__
#define __NSGFRAD_H_INCLUDED__

#include "Frad.h"
#include "SerialPort.h"
#include "TelnetSockets.h"

class CSerialPort_NSG : public CSerialPort
{
	virtual char *GetControlString() { return "9600,n,8,1,p"; }
	virtual unsigned int GetReadTimeout() { return 1000; }
	virtual unsigned int GetWriteTimeout() { return 1000; }
};


class CNsgFrad : public CFrad
{
public:
// Construction-destruction
	CNsgFrad();
	virtual ~CNsgFrad();
	
// Public methods

// Overridable methods

//Common methods
	virtual MC_ErrorCode Command(BOOL bWaitForReply = TRUE);
	virtual MC_ErrorCode Command(char *pszCommand, BOOL bWaitForReply = TRUE) { strcpy(m_pszCommand, pszCommand); return Command(bWaitForReply); }
	virtual CTerminal *GetTerminal();
	virtual BOOL CanBeControlledByComPort() { return TRUE; }
	virtual BOOL CanBeControlledByTcpIp() { return TRUE; }
	virtual BOOL CanAsciiControl() { return TRUE; }
	virtual void OnMcConnected();
	virtual void OnMcDisconnected();
	virtual MC_ErrorCode ReadReplyUntilPrompt();
	virtual unsigned char *GetMCReplyData() { return (unsigned char *)m_pszReply; }
	virtual unsigned int GetMCReplyDataLength() { return strlen(m_pszReply); }
	virtual BOOL CanWarmReset() { return TRUE; }
	virtual MC_ErrorCode WarmReset();
	virtual MC_ErrorCode GetDeviceStatusString(char *&pszString);
	virtual MC_ErrorCode GetDeviceVersionString(char *&pszString);
	virtual BOOL CanWriteConfiguration() { return TRUE; }
	virtual MC_ErrorCode WriteConfiguration();
	virtual BOOL CanFactoryPreset() { return TRUE; }
	virtual MC_ErrorCode FactoryPreset();
	virtual MC_ErrorCode ConfigureForCommonTests();
// Monitor and Control methods
	virtual void OnMcSymbolReceived(char cSymbol);
// Physical port
	virtual int GetPhysicalPortNum() { return 0; }
	virtual MC_ErrorCode GetPhysicalPortConfigString(int PortNo, char *&pszConfigString);
	virtual MC_ErrorCode SetPhysicalPortConfigString(int PortNo, char *&pszConfigString);
	virtual MC_ErrorCode PhysicalPortWarmStart(int PortNo);
	virtual MC_ErrorCode GetPhysicalPortStatusString(int PortNo, char *&pszConfigString);
// Physical port type
	virtual MC_ErrorCode GetPhysicalPortType(int PortNo, char *pszType);
	virtual MC_ErrorCode SetPhysicalPortType(int PortNo, char *pszType);
// Physical port speed
	virtual MC_ErrorCode GetPhysicalPortSpeed(int PortNo, char *pszSpeed);
	virtual MC_ErrorCode SetPhysicalPortSpeed(int PortNo, char *pszSpeed);
// Frame Relay stations
	virtual int GetFrameRelayStationNum() { return 0; }
	virtual MC_ErrorCode GetFrameRelayStationConfigString(int StationNo, char *&pszConfigString);
	virtual MC_ErrorCode SetFrameRelayStationConfigString(int StationNo, char *&pszConfigString);
	virtual MC_ErrorCode GetFrameRelayStationStatusString(int StationNo, char *&pszStatusString);
// Ethernet stations
	virtual int GetEthernetStationNum() { return 0; }
	virtual MC_ErrorCode GetEthernetStationConfigString(int StationNo, char *&pszConfigString);
	virtual MC_ErrorCode SetEthernetStationConfigString(int StationNo, char *&pszConfigString);
	virtual MC_ErrorCode GetEthernetStationStatusString(int StationNo, char *&pszStatusString);
// Telnet stations
	virtual int GetTelnetStationNum() { return 0; }
	virtual MC_ErrorCode GetTelnetStationConfigString(int StationNo, char *&pszConfigString);
	virtual MC_ErrorCode SetTelnetStationConfigString(int StationNo, char *&pszConfigString);
	virtual MC_ErrorCode GetTelnetStationStatusString(int StationNo, char *&pszStatusString);
	virtual MC_ErrorCode WarmStartTelnetStation(int StationNo);
// Routing
	virtual int GetMaxIPInterfaces() { return 0; }
	virtual MC_ErrorCode GetIPInterfaceConfigString(int InterfaceNo, char *&pszConfigString, unsigned int &InterfaceCount, BOOL &bCanRIP);
	virtual MC_ErrorCode SetIPInterfaceConfigString(int InterfaceNo, char *&pszConfigString, unsigned int &InterfaceCount, BOOL &bCanRIP);
	virtual MC_ErrorCode GetIPInterfaceStatusString(int InterfaceNo, char *&pszConfigString);
	virtual MC_ErrorCode IPInterfaceWarmStart(int InterfaceNo);
	virtual MC_ErrorCode GetIPStaticRoutesString(char *&pszStaticRoutesString);
	virtual MC_ErrorCode AddIPStaticRoute(char *pszNet, char *pszMask, char *pszGateway, char *pszMetric);
	virtual MC_ErrorCode DeleteIPStaticRoute(char *pszNet, char *pszMask);
	virtual MC_ErrorCode SetIPDefaultGateway(char *pszDefaultGatewayString);
	virtual MC_ErrorCode GetIPRoutingTable(char *&pszTableString);
	virtual MC_ErrorCode GetPVCs(int PortNo, char *&pszTableString);
	virtual MC_ErrorCode AddPVC(char *pszPort1, char *pszChannel1, char *pszPort2, char *pszChannel2);
	virtual MC_ErrorCode DeletePVC(char *pszPort, char *pszChannel);
// Script related methods
	virtual MC_ErrorCode MakeScript(char *pszScriptBody);
	virtual MC_ErrorCode ApplyScript(char *pszScriptBody);

// Protected methods
protected:
	CSerialPort_NSG m_SerialPort;
	CTelnetSocket m_TelnetSocket;
	char *RemoveCRLFandPrompt(char *pszRawReply);
	void ReadReplyUntilAsterisk();
	char m_pszReply[4092];
	char m_pszCommand[256];
	BOOL m_bBreakReading;
	BOOL m_bMakeScriptCommand;
	BOOL m_bWaitForAsterisk;
};

class CNps3e : public CNsgFrad
{
public:
	// Construction-destruction
	CNps3e();
	virtual ~CNps3e();
	
	// Public methods

	// Overridable methods
	virtual char *GetName() { return m_pszName; }

	// Common device commands
	// Physical port
	virtual int GetPhysicalPortNum() { return 4; }
	// Frame Relay stations
	virtual int GetFrameRelayStationNum() { return 64; }
	// Ethernet stations
	virtual int GetEthernetStationNum() { return 4; }
	// Telnet stations
	virtual int GetTelnetStationNum() { return 8; }
	// Routing
	virtual int GetMaxIPInterfaces() { return 20; }

	static char *m_pszName;

protected:
};


class CNx300 : public CNsgFrad
{
public:
	// Construction-destruction
	CNx300();
	virtual ~CNx300();
	
	// Public methods

	// Overridable methods

	// Common device commands
	virtual char *GetName() { return m_pszName; }
	// Physical port
	virtual int GetPhysicalPortNum() { return 4; }
	// Frame Relay stations
	virtual int GetFrameRelayStationNum() { return 64; }
	// Ethernet stations
	virtual int GetEthernetStationNum() { return 4; }
	// Telnet stations
	virtual int GetTelnetStationNum() { return 8; }
	// Routing
	virtual int GetMaxIPInterfaces() { return 20; }

	static char *m_pszName;

protected:
};

#endif //__NSGFRAD_H_INCLUDED__