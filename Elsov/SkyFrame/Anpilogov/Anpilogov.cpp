// Anpilogov.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Anpilogov.h"
#include "../HardwareTest/Datum.h"
#include "../HardwareTest/Radyne.h"
#include "SNMP_stuff.h"
#include "../Common/SDMS_OIDs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;
typedef struct
{
	CModem *pModem;
	IPADDRESS_TYPE IpAddress;
} SModem;

int Anpilogov(int argc, TCHAR* argv[])
{
	unsigned int TraOID[] = { 1,3,6,1,4,1,7840,3,1,2,2,2,2,1,1,1 };
	int TraOidLen = sizeof(TraOID)/sizeof(TraOID[0]);
	unsigned int RecOID[] = { 1,3,6,1,4,1,7840,3,1,2,3,2,2,1,1,1 };
	int RecOidLen = sizeof(RecOID)/sizeof(RecOID[0]);

	CModem *pModem;
	int nPort;
	int nModems = 0;
	SModem Modems[2];
	memset(Modems, 0, sizeof(Modems));
	int BusAddress;
	unsigned int DataRate = 32000;
	unsigned int TimeDelay = 500;
	char szIpAddress[128]; szIpAddress[0] = 0;
	int i1, i2, i3, i4;
	BYTE b1=0, b2=0, b3=0, b4=0;
	CSnmpSocket snmp;
	
	for (int i = 1; i<argc; i++)
	{
		char c = 0;
		char *pszArg = argv[i];
		if (*pszArg != '-')
			continue;
		switch (*(pszArg+1))
		{
		case '7': // Datum modem 70 MHz
			pModem = new CDatumPsm4900_70MHz();
			Modems[nModems].pModem = pModem;
			sscanf(argv[i]+2, "%d", &nPort);
			pModem->Connect(nPort);
			nModems++;
			break;
		case 'D':
		case 'd': // Datum modem
			pModem = new CDatumPsm4900_LBand();
			Modems[nModems].pModem = pModem;
			sscanf(argv[i]+2, "%d", &nPort);
			pModem->Connect(nPort);
			nModems++;
			break;
		case 'R':
		case 'r': // Radyne modem
			pModem = new CRadyneDMD2401();
			Modems[nModems].pModem = pModem;
			sscanf(argv[i]+2, "%d", &nPort);
			pModem->Connect(nPort);
			BusAddress = 0;
			while (BusAddress < 32 || BusAddress > 255)
			{
				printf("Please enter remote address for Radyne DMD-2401 (32...255) :");
				scanf("%d", &BusAddress);
			}
			pModem->SetMCBusAddress(BusAddress);
			nModems++;
			break;
		case 'W':
		case 'w': // data rate
			sscanf(argv[i]+2, "%d", &DataRate);
			break;
		case 'T':
		case 't': // TimeDelay
			sscanf(argv[i]+2, "%d", &TimeDelay);
			break;
		case 'V':
		case 'v':
			sscanf(argv[i]+2, "%s", szIpAddress);

			if (sscanf(szIpAddress, "%d.%d.%d.%d", &i1, &i2, &i3, &i4) != 4)
			{
				printf("Invalid address\n");
				return 0;
			}

			b1=LOBYTE(LOWORD(i1));
			b2=LOBYTE(LOWORD(i2));
			b3=LOBYTE(LOWORD(i3));
			b4=LOBYTE(LOWORD(i4));
			Modems[nModems].IpAddress = MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4));
			++nModems;
			break;
		default:
			printf("Error: - Unrecognized command-line option '%c'\n", *(pszArg+1));
			break;
		};
	}

	if (nModems < 2)
	{
		printf("Please give me 2 modems in command line\n");
		return 0;
	}

	CModem *pTransmitter = Modems[0].pModem;
	IPADDRESS_TYPE TransmitterIP = Modems[0].IpAddress;
	CModem *pReceiver = Modems[1].pModem;
	IPADDRESS_TYPE ReceiverIP = Modems[1].IpAddress;

	printf("Setting modulator freq.  %d...", DataRate);	
	if (pTransmitter)
		pTransmitter->SetTDataRate(DataRate, 1);
	if (TransmitterIP)
	{
		for (int i = 0; i < 5; i++)
		{
			Sleep(0);
			snmp.SendSnmpSetUnsignedIntegerRequest(TransmitterIP, "private", OidModulatorIfFrequency, OidModulatorIfFrequencyLen, 1, DataRate);
		}
	}
	printf("done\n");
	DWORD start = GetTickCount();

	printf("Sleeping %d msec...", TimeDelay);
	Sleep(TimeDelay);
	printf("done\n");

	printf("Setting demodulator freq. %d...", DataRate);
	if (pReceiver)
		pReceiver->SetRDataRate(DataRate, 1);
	if (ReceiverIP)
	{
		for (i = 0; i < 5; i++)
		{
			Sleep(0);
			snmp.SendSnmpSetUnsignedIntegerRequest(ReceiverIP, "private", OidDemodulatorIfFrequency, OidDemodulatorIfFrequencyLen, 1, DataRate-50000000);
		}
	}
	printf("done\n");
/*	CDemIfStatus status;
	do
	{
		pReceiver->GetRIfStatus(status, 1);
	} while (!status.m_bCarrier);
	DWORD stop = GetTickCount();
	printf("%d msec\n", stop-start);*/

	return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		nRetCode = Anpilogov(argc, argv);
	}

	return nRetCode;
}


