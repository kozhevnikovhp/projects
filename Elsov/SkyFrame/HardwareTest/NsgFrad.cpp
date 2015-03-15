#include "stdafx.h"
#include "NsgFrad.h"

static char *pszFinalResult;
static char CR=0x0D;
static char LF=0x0A;
static char *pszUnknown="Unknown - some error occured";

CNsgFrad::CNsgFrad()
{
	m_bMakeScriptCommand = FALSE;
	m_bWaitForAsterisk = FALSE;
}

CNsgFrad::~CNsgFrad()
{
	Disconnect();
}

//virtual
CTerminal *CNsgFrad::GetTerminal()
{
	switch (m_ControlType)
	{
	case MC_UNKNOWN_TYPE:
	case MC_SERIAL_PORT:
		return &m_SerialPort;
	case MC_TELNET:
		return &m_TelnetSocket;
	}
	return NULL;
}

MC_ErrorCode CNsgFrad::Command(BOOL bWaitForReply/* = TRUE*/)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	GetTerminal()->ClearAllBuffers();
	for (unsigned int i=0; i<strlen(m_pszCommand); i++)
	{
		if (m_pszCommand[i] == CR ||
			m_pszCommand[i] == LF) m_pszCommand[i] = ' ';
	}
	strncat((char *)m_pszCommand, &CR, 1);
	strncat((char *)m_pszCommand, &LF, 1);
	GetTerminal()->Write(m_pszCommand, strlen(m_pszCommand));
	Sleep(100);

	if (bWaitForReply)
	{
		ReadReplyUntilPrompt();
		pszFinalResult=RemoveCRLFandPrompt(m_pszReply);
	}
//	Sleep(50);
	return MC_OK;
}

void CNsgFrad::ReadReplyUntilAsterisk()
{
	m_pszReply[0]=0;
	m_bBreakReading = FALSE;
	m_bWaitForAsterisk = TRUE;

	time_t	start, finish;
	time(&start);
	do
	{
		char cSymbol = 0;
		time(&finish);
		if (!GetTerminal()->Read(&cSymbol, 1)) 
			continue;
		if (cSymbol == '*')
			break;
		OnMcSymbolReceived(cSymbol);
		if (m_bBreakReading)
			break;
	} while (difftime(finish, start) < 5);
	m_bWaitForAsterisk = FALSE;
//	TRACE("%s", m_pszReply);
}

//virtual
MC_ErrorCode CNsgFrad::ReadReplyUntilPrompt()
{
	m_pszReply[0]=0;
	m_bBreakReading = FALSE;
	char cSymbol = 0;

	time_t	start, finish;
	time(&start);
	do
	{
		if (!GetTerminal()->Read(&cSymbol, 1)) break;
		OnMcSymbolReceived(cSymbol);
		TRACE("%c", cSymbol);
		if (m_bBreakReading)
			break;
		time(&finish);
	} while (difftime(finish, start) < 10);
//	TRACE("%s", m_pszReply);
	if (m_bBreakReading)
		return MC_OK;
	return MC_DEVICE_NOT_RESPONDING;
}

char *CNsgFrad::RemoveCRLFandPrompt(char *pszRawReply)
{
	char *pszStart=strchr(pszRawReply, CR);
	if (!pszStart)
		return pszUnknown;
	while (*pszStart==CR || *pszStart==LF)
		pszStart++;
		
	// rewrite this code! Prompt string is changeable now (not "Manager:" only)!!!
	const char *pszPrompt = "Manager:";
	char *pszEnd=strstr(pszRawReply, pszPrompt);
	if (!pszEnd)
		return pszUnknown;
	else
	{ // try to find second prompt substring (make script command)
		char *pszEnd2 = strstr(pszEnd+1, pszPrompt);
		if (pszEnd2)
			pszEnd = pszEnd2;
	}

	*pszEnd=0;
	return pszStart;
}

//virtual
void CNsgFrad::OnMcConnected()
{
	Command("77", FALSE);
	Command("\n");
	//GetTerminal()->PeekInputSymbols();
}

//virtual
void CNsgFrad::OnMcDisconnected()
{
	Command("q m", FALSE);
	ReadReplyUntilAsterisk();
}

// Common device commands

MC_ErrorCode CNsgFrad::WarmReset()
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	MC_ErrorCode EC=Command("W S PO:A", FALSE);
	Sleep(5000);
	Connect();
//	ReadReplyUntilAsterisk();
	return EC;
}

//virtual
MC_ErrorCode CNsgFrad::GetDeviceStatusString(char *&pszStatusString)
{
	pszStatusString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	strcpy(m_pszCommand, "D S SY:0");
	MC_ErrorCode EC=Command();
	pszStatusString=pszFinalResult;
	return EC;
}

//virtual
MC_ErrorCode CNsgFrad::GetDeviceVersionString(char *&pszString)
{
	pszString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	strcpy(m_pszCommand, "D V");
	MC_ErrorCode EC=Command();
	pszString=pszFinalResult;
	return EC;
}

//virtual
MC_ErrorCode CNsgFrad::WriteConfiguration()
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	strcpy(m_pszCommand, "W F");
	MC_ErrorCode EC=Command();
	Sleep(500);
	return EC;
}

//virtual
MC_ErrorCode CNsgFrad::FactoryPreset()
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	strcpy(m_pszCommand, "F S NW");
	MC_ErrorCode EC=Command();
	return EC;
}

//virtual
MC_ErrorCode CNsgFrad::ConfigureForCommonTests()
{
/*	static char pszOneCommand[128];
	Command("D P OFF", FALSE);

	// Unconfigure all physical ports
	for (int i = 0; i < GetPhysicalPortNum(); i++)
	{
		sprintf (pszOneCommand, "S P PO:%d TY:NOCONF", i);
		Command(pszOneCommand, FALSE);
	}

	// Unconfigure all frame relay stations
	for (i = 0; i < GetFrameRelayStationNum(); i++)
	{
		sprintf (pszOneCommand, "S P ST:%d TY:NOCONF", i);
		Command(pszOneCommand, FALSE);
	}

	// Unconfigure all ethernet stations
	for (i = 0; i < GetEthernetStationNum(); i++)
	{
		sprintf (pszOneCommand, "S P ET:%d TY:NOCONF", i);
		Command(pszOneCommand, FALSE);
	}

	// Unconfigure all telnet stations
	for (i = 0; i < GetTelnetStationNum(); i++)
	{
		sprintf (pszOneCommand, "S P TN:%d TY:NOCONF", i);
		Command(pszOneCommand, FALSE);
	}

	static char *pszCommand[] = {
	"C R PR:0",
	"S P PO:00 TY:ETH IF:TP MODE:HALF SP:10000000 ADDR:00.00.00.00.00.AA.",
	"S P PO:04 TY:PAD IF:V24 SP:9600 AF:8N1 CO:NO RP:NO AC:NO CM:NO NUI:NO LG:128 MB:NO CD:YES BI:0 CIDLE:0 RIDLE:0 TIDLE:0 AD:NO PT:\"*\" MS:\"\" 1:1 2:1 3:2 4:0 5:0 6:1 7:2 8:0 9:0 10:0 12:1 13:4 14:0 15:1 16:8 17:24 18:2 19:2",
	"S P ET:00 PO:0 TY:IP FRTY:Ethernet",
	"S P TN:00 TY:PAD CO:NO RP:NO AC:NO CM:NO NUI:NO LG:128 MB:NO CD:YES BI:0 CIDLE:300 RIDLE:0 TIDLE:0 AD:NO PT:\"*\" MS:\"\" 1:1 2:1 3:2 4:0 5:0 6:1 7:2 8:0 9:0 10:0 12:1 13:4 14:0 15:1 16:8 17:24 18:2 19:2 TCPPORT:23 IAC:YES",

	"S P IP:0 NUM:01  TTL:255 RIP:NO HTTP:NO DNS:NO",
	"S P IP:00 ADM:UP IADR:0.0.0.0 MASK:0.0.0.0 BRC:1 MTU:1500 TY:LOCAL",
	"S P IP:01 ADM:UP IADR:192.168.31.77 MASK:255.255.255.0 BRC:1 MTU:1500 ACCT:0 NAT:NO TY:ETHI ET:0",

	"S R PR:00 ID:D RT:77 TO:MN CONT:NO",
	"S F PF:0   1:1  2:1  3:2  4:0  5:0  6:1  7:2  8:0  9:0  10:0       12:1  13:4  14:0  15:1  16:8  17:24  18:2  19:2",
	"S F PF:1   1:0  2:0  3:127  4:0  5:0  6:1  7:8  8:0  9:0  10:0       12:0  13:0  14:0  15:0  16:0  17:0  18:0  19:0",
	"S F PF:2   1:0  2:0  3:0  4:1  5:0  6:1  7:8  8:0  9:0  10:0       12:0  13:0  14:0  15:0  16:0  17:0  18:0  19:0",
	"S F PF:3   1:0  2:0  3:0  4:1  5:0  6:0  7:0  8:0  9:0  10:0       12:0  13:0  14:0  15:0  16:0  17:0  18:0  19:0",
	"S F PF:4   1:0  2:0  3:0  4:0  5:0  6:0  7:0  8:0  9:0  10:0       12:0  13:0  14:0  15:0  16:0  17:0  18:0  19:0",
	"S F PF:5   1:0  2:0  3:0  4:0  5:0  6:0  7:0  8:0  9:0  10:0       12:0  13:0  14:0  15:0  16:0  17:0  18:0  19:0",
	"S F PF:6   1:0  2:0  3:0  4:0  5:0  6:0  7:0  8:0  9:0  10:0       12:0  13:0  14:0  15:0  16:0  17:0  18:0  19:0",
	"S F PF:7   1:0  2:0  3:0  4:0  5:0  6:0  7:0  8:0  9:0  10:0       12:0  13:0  14:0  15:0  16:0  17:0  18:0  19:0",

	"S U",
	"S P CO:00  NAME:\"public\"       WR:NO   TP:NO  IADR:0.0.0.0  MASK:0.0.0.0",
	"S P CO:01  NAME:\"\"       WR:NO   TP:NO  IADR:0.0.0.0  MASK:0.0.0.0",
	"S P CO:02  NAME:\"\"       WR:NO   TP:NO  IADR:0.0.0.0  MASK:0.0.0.0",
	"S P CO:03  NAME:\"\"       WR:NO   TP:NO  IADR:0.0.0.0  MASK:0.0.0.0",
	"S P CO:04  NAME:\"\"       WR:NO   TP:NO  IADR:0.0.0.0  MASK:0.0.0.0",
	
	"S P AU:01  TY:NO_AUTH",
	"S P AU:02  TY:NO_AUTH",
	"S P AU:03  TY:NO_AUTH",
	"S P AU:04  TY:NO_AUTH",

	"S P BI:01  TY:NO_BILL",
	"S P BI:02  TY:NO_BILL",
	"S P BI:03  TY:NO_BILL",
	"S P BI:04  TY:NO_BILL",

	"S P HX:00  NUM:0 LC:0",
	"S W PR:\"Manager: \" MNIT:0 MNTR:3 HS:2000000 SS:1572864 HELP:YES HNAM:\"HOSTNAME\" CONT:\"www.nsg.ru e-mail:info@nsg.ru tel:+7(095)918-3211 fax:+7(095)918-2739\" LOC:\"39 , Kirpichnaya st. Moscow 105187 Russia\"",

	"D P ON"};

	MC_ErrorCode EC = MC_OK;
	for (i = 0; i < sizeof(pszCommand)/sizeof(pszCommand[0]); i++)
	{
		EC = Command(pszCommand[i], FALSE);	
		if (EC != MC_OK)
			break;
	}
	EC = WriteConfiguration();
	EC = WarmReset();
	Connect();
	return EC;*/
	return MC_OK;
}
// Monitor and Control methods

//virtual
void CNsgFrad::OnMcSymbolReceived(char cSymbol)
{
	if (cSymbol == LF)
	{
	}
	else if (cSymbol == CR)
	{
		strncat(m_pszReply, &CR, 1);
		strncat(m_pszReply, &LF, 1);
	}
	else if (cSymbol == 8)
	{
		strcat(m_pszReply, " ");
	}
	else
	{
		strncat(m_pszReply, &cSymbol, 1);
	}

	int i = strlen(m_pszReply);
	m_bBreakReading = FALSE;

	if (m_bWaitForAsterisk && cSymbol == '*')
	{
		m_bWaitForAsterisk = FALSE;
		m_bBreakReading = TRUE;
	}

	// rewrite this code! Prompt string is changeable now (not "Manager:" only)!!!
	if (i>9 &&
		m_pszReply[i-1]==' ' &&
		m_pszReply[i-2]==':' &&
		m_pszReply[i-3]=='r' &&
		m_pszReply[i-4]=='e' &&
		m_pszReply[i-5]=='g' &&
		m_pszReply[i-6]=='a' &&
		m_pszReply[i-7]=='n' &&
		m_pszReply[i-8]=='a' &&
		m_pszReply[i-9]=='M')
	{
		if (!m_bMakeScriptCommand)
			m_bBreakReading = TRUE;
		m_bMakeScriptCommand = FALSE;
	}
}

// Physical port related methods

MC_ErrorCode CNsgFrad::GetPhysicalPortConfigString(int PortNo, char *&pszConfigString)
{
	pszConfigString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D P PO:%02d", PortNo);
	MC_ErrorCode EC=Command();
	pszConfigString=strstr(pszFinalResult, "TY:");
	if (!pszConfigString)
		pszConfigString=pszUnknown;
	return EC;
}

MC_ErrorCode CNsgFrad::SetPhysicalPortConfigString(int PortNo, char *&pszConfigString)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "S P PO:%02d %s", PortNo, pszConfigString);
	MC_ErrorCode EC=Command();
	GetPhysicalPortConfigString(PortNo, pszConfigString);
	return EC;
}

MC_ErrorCode CNsgFrad::PhysicalPortWarmStart(int PortNo)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "W S PO:%02d", PortNo);
	MC_ErrorCode EC=Command();
	return EC;
}

MC_ErrorCode CNsgFrad::GetPhysicalPortType(int PortNo, char *pszType)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D P PO:%02d", PortNo);
	MC_ErrorCode EC=Command();
	char *pszStart=strstr(pszFinalResult, "TY:");
	if (pszStart)
		sscanf(pszStart, "TY:%s", pszType);
	return EC;
}

MC_ErrorCode CNsgFrad::SetPhysicalPortType(int PortNo, char *pszType)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "S P PO:%02d TY:%s", PortNo, pszType);
	MC_ErrorCode EC=Command();
	return EC;
}

MC_ErrorCode CNsgFrad::GetPhysicalPortSpeed(int PortNo, char *pszSpeed)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D P PO:%02d", PortNo);
	MC_ErrorCode EC=Command();
	char *pszStart=strstr(pszFinalResult, "SP:");
	if (pszStart)
		sscanf(pszStart, "SP:%s", pszSpeed);
	else
		strcpy(pszSpeed, pszUnknown);
	return EC;
}

MC_ErrorCode CNsgFrad::SetPhysicalPortSpeed(int PortNo, char *pszSpeed)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "S P PO:%02d SP:%s", PortNo, pszSpeed);
	MC_ErrorCode EC=Command();
	return EC;
}

MC_ErrorCode CNsgFrad::GetPhysicalPortStatusString(int PortNo, char *&pszStatusString)
{
	pszStatusString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D S PO:%02d", PortNo);
	MC_ErrorCode EC=Command();
	pszStatusString=pszFinalResult;
	if (!pszStatusString)
		pszStatusString=pszUnknown;
	return EC;
}

	// Frame Relay stations related methods

MC_ErrorCode CNsgFrad::GetFrameRelayStationConfigString(int StationNo, char *&pszConfigString)
{
	pszConfigString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D P ST:%02d", StationNo);
	MC_ErrorCode EC=Command();
	pszConfigString=strstr(pszFinalResult, "PO:");
	if (!pszConfigString)
	{
		pszConfigString=strstr(pszFinalResult, "TY:");
		if (!pszConfigString)
			pszConfigString=pszUnknown;
	}
	return EC;
}

MC_ErrorCode CNsgFrad::SetFrameRelayStationConfigString(int StationNo, char *&pszConfigString)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "S P ST:%02d %s", StationNo, pszConfigString);
	MC_ErrorCode EC=Command();
	GetFrameRelayStationConfigString(StationNo, pszConfigString);
	return EC;
}

//virtual
MC_ErrorCode CNsgFrad::GetFrameRelayStationStatusString(int StationNo, char *&pszStatusString)
{
	pszStatusString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D S ST:%02d", StationNo);
	MC_ErrorCode EC=Command();
	pszStatusString=pszFinalResult;
	return EC;
}

	// Ethernet stations related methods

MC_ErrorCode CNsgFrad::GetEthernetStationConfigString(int StationNo, char *&pszConfigString)
{
	pszConfigString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D P ET:%02d", StationNo);
	MC_ErrorCode EC=Command();
	pszConfigString=strstr(pszFinalResult, "PO:");
	if (!pszConfigString)
	{
		pszConfigString=strstr(pszFinalResult, "TY:");
		if (!pszConfigString)
			pszConfigString=pszUnknown;
	}
	return EC;
}

MC_ErrorCode CNsgFrad::SetEthernetStationConfigString(int StationNo, char *&pszConfigString)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "S P ET:%02d %s", StationNo, pszConfigString);
	MC_ErrorCode EC=Command();
	GetEthernetStationConfigString(StationNo, pszConfigString);
	return EC;
}

//virtual
MC_ErrorCode CNsgFrad::GetEthernetStationStatusString(int StationNo, char *&pszStatusString)
{
	pszStatusString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D S ET:%02d", StationNo);
	MC_ErrorCode EC=Command();
	pszStatusString=pszFinalResult;
	return EC;
}

	// Telnet stations related methods

MC_ErrorCode CNsgFrad::GetTelnetStationConfigString(int StationNo, char *&pszConfigString)
{
	pszConfigString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D P TN:%02d", StationNo);
	MC_ErrorCode EC=Command();
	pszConfigString=strstr(pszFinalResult, "PO:");
	if (!pszConfigString)
	{
		pszConfigString=strstr(pszFinalResult, "TY:");
		if (!pszConfigString)
			pszConfigString=pszUnknown;
	}
	return EC;
}

MC_ErrorCode CNsgFrad::SetTelnetStationConfigString(int StationNo, char *&pszConfigString)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "S P TN:%02d %s", StationNo, pszConfigString);
	MC_ErrorCode EC=Command();
	GetTelnetStationConfigString(StationNo, pszConfigString);
	return EC;
}

//virtual
MC_ErrorCode CNsgFrad::GetTelnetStationStatusString(int StationNo, char *&pszStatusString)
{
	pszStatusString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D S TN:%02d", StationNo);
	MC_ErrorCode EC=Command();
	pszStatusString=pszFinalResult;
	return EC;
}

//virtual 
MC_ErrorCode CNsgFrad::WarmStartTelnetStation(int StationNo)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "W S TN:%02d", StationNo);
	MC_ErrorCode EC=Command();
	return EC;
}

	// Routing related methods

MC_ErrorCode CNsgFrad::GetIPInterfaceConfigString(int InterfaceNo, char *&pszConfigString, unsigned int &InterfaceCount, BOOL &bCanRIP)
{
	pszConfigString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D P IP:%02d", InterfaceNo);
	MC_ErrorCode EC=Command();
	pszConfigString=strstr(pszFinalResult, "ADM:");
	if (!pszConfigString)
		pszConfigString=pszUnknown;
	char *pszNum=strstr(pszFinalResult, "NUM:");
	if (pszNum)
		sscanf(pszNum, "NUM:%d", &InterfaceCount);
	else
		InterfaceCount=0;
	char pszCanRIP[16], *pszRIP=strstr(pszFinalResult, "RIP:");
	if (pszRIP)
	{
		sscanf(pszRIP, "RIP:%s", pszCanRIP);
		if (!strcmp(pszCanRIP, "NO"))
			bCanRIP=FALSE;
		if (!strcmp(pszCanRIP, "YES"))
			bCanRIP=TRUE;
	}
	else
		bCanRIP=FALSE;
	return EC;
}

MC_ErrorCode CNsgFrad::SetIPInterfaceConfigString(int InterfaceNo, char *&pszConfigString, unsigned int &InterfaceCount, BOOL &bCanRIP)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "S P IP:%02d %s", InterfaceNo, pszConfigString);
	if (InterfaceNo==0)
	{ // local interface - additional parameters need
		static char pszAdd[32];
		sprintf(pszAdd, " NUM:%02d", InterfaceCount);
		strcat(m_pszCommand, pszAdd);
		strcat(m_pszCommand, " RIP:");
		if (bCanRIP)
			strcat(m_pszCommand, "YES");
		else
			strcat(m_pszCommand, "NO");
	}
	MC_ErrorCode EC=Command();
	GetIPInterfaceConfigString(InterfaceNo, pszConfigString, InterfaceCount, bCanRIP);
	return EC;
}

//virtual
MC_ErrorCode CNsgFrad::GetIPInterfaceStatusString(int InterfaceNo, char *&pszConfigString)
{
	pszConfigString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "D S IP:%02d", InterfaceNo);
	MC_ErrorCode EC=Command();
	pszConfigString=pszFinalResult;
	return EC;
}

MC_ErrorCode CNsgFrad::IPInterfaceWarmStart(int InterfaceNo)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "W S IP:%02d", InterfaceNo);
	MC_ErrorCode EC=Command();
//	Sleep(50);
	return EC;
}

MC_ErrorCode CNsgFrad::GetIPStaticRoutesString(char *&pszStaticRoutesString)
{
	pszStaticRoutesString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	strcpy(m_pszCommand, "D I STATIC");
	MC_ErrorCode EC=Command();
	pszStaticRoutesString=pszFinalResult;
	return EC;
}

MC_ErrorCode CNsgFrad::AddIPStaticRoute(char *pszNet, char *pszMask, char *pszGateway, char *pszMetric)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "S I NET:%s MASK:%s GW:%s MET:%s", pszNet, pszMask, pszGateway, pszMetric);
	MC_ErrorCode EC=Command();
	return EC;
}

MC_ErrorCode CNsgFrad::DeleteIPStaticRoute(char *pszNet, char *pszMask)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "C I NET:%s MASK:%s", pszNet, pszMask);
	MC_ErrorCode EC=Command();
	return EC;
}

MC_ErrorCode CNsgFrad::GetIPRoutingTable(char *&pszTableString)
{
	pszTableString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	strcpy(m_pszCommand, "D I");
	MC_ErrorCode EC=Command();
	pszTableString=pszFinalResult;
	return EC;
}

MC_ErrorCode CNsgFrad::SetIPDefaultGateway(char *pszDefaultGatewayString)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "S I DEFAULT GW:%s", pszDefaultGatewayString);
	MC_ErrorCode EC=Command();
	return EC;
}

MC_ErrorCode CNsgFrad::GetPVCs(int PortNo, char *&pszTableString)
{
	pszTableString=pszUnknown;
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	static char pszPortNo[8];
	if (PortNo==-1)
		strcpy(pszPortNo, "A");
	else
		sprintf(pszPortNo, "%02d", PortNo);
	sprintf(m_pszCommand, "D C PO:%s", pszPortNo);
	
	MC_ErrorCode EC=Command();
	pszTableString=pszFinalResult;
	return EC;
}

MC_ErrorCode CNsgFrad::AddPVC(char *pszPort1, char *pszChannel1, char *pszPort2, char *pszChannel2)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "A P PO:%s CH:%s PO:%s CH:%s", pszPort1, pszChannel1, pszPort2, pszChannel2);
	MC_ErrorCode EC=Command();
	return EC;
}

MC_ErrorCode CNsgFrad::DeletePVC(char *pszPort, char *pszChannel)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	sprintf(m_pszCommand, "R P PO:%s CH:%s", pszPort, pszChannel);
	MC_ErrorCode EC=Command();
	return EC;
}

	// Script related methods

MC_ErrorCode CNsgFrad::MakeScript(char *pszScriptBody)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	m_bMakeScriptCommand = TRUE;
	strcpy(m_pszCommand, "M S");
	MC_ErrorCode EC=Command();
	strcpy(pszScriptBody, pszFinalResult);
	return EC;
}

MC_ErrorCode CNsgFrad::ApplyScript(char *pszScriptBody)
{
	if (!IsControllable()) return MC_DEVICE_NOT_CONTROLLABLE;
	
	// turn echo off
	strcpy(m_pszCommand, "D P OFF");
	MC_ErrorCode EC = Command(FALSE);
	int i = 0;

	// each string as separate command
	while (*pszScriptBody)
	{
		char cSymbol = *pszScriptBody;
		if (cSymbol == CR || cSymbol == LF)
		{
			m_pszCommand[i] = 0;
			Command(FALSE);
			i = 0;
		}
		else
		{
			m_pszCommand[i] = cSymbol;
			i++;
		}
		pszScriptBody++;
	}
	// last string (not empty if last string of the script does not contain tail CRLF)
	m_pszCommand[i] = 0;
	Command(FALSE);

	// turn echo on back
	strcpy(m_pszCommand, "D P ON");
	EC = Command();
	return EC;
}

/////////////////////////////////////////////////////////////////////////////
// class CNps3e implementation

char *CNps3e::m_pszName="Nps-3e (FRAD)";

CNps3e::CNps3e()
{
}

CNps3e::~CNps3e()
{
}


/////////////////////////////////////////////////////////////////////////////
// class CNx300 implementation

char *CNx300::m_pszName="NX-300 (FRAD)";

CNx300::CNx300()
{
}

CNx300::~CNx300()
{
}
