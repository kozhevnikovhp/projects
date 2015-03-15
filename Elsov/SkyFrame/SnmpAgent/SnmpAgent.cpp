// SnmpAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h> // getch() at the end
#include <time.h>
#include "../Common/SDMS_OIDs.h"
#include "DatumModem.h"
#include "PozharskyUpConverter.h"
#include "PozharskyDownConverter.h"


const char CR = 0x0D;
const char LF = 0x0A;

void ConnectDevice(CDevice *pDevice, MC_ControlType ControlType, const char *pszConnectionName, unsigned int nMCAddress)
{
	pDevice->SetControlType(ControlType);
	pDevice->SetConnectionName(pszConnectionName);
	pDevice->SetMCBusAddress(nMCAddress);
	pDevice->Connect();
}

void ListenRequests(const char *pszFileName, IPADDRESS_TYPE IP, unsigned short port)
{
	CDatumModem *pModem = NULL;
	CPozharskyUpDownConverter *pUpConverter = NULL;
	CPozharskyUpDownConverter *pDownConverter = NULL;

	FILE *pFile = fopen(pszFileName, "r");
	if (!pFile)
	{
		printf("Cannot read config file %s\n", pszFileName);
		return;
	}

	int nScanfed = 0;
	do
	{
		std::string strDeviceName;
		char pszConnectionName[128];
		MC_ControlType ControlType = MC_UNKNOWN_TYPE;
		unsigned int bConnected = 0;
		unsigned int nMCAddress = 0;
		unsigned int Param1 = 0;
		strDeviceName = "";
		char c;
		do
		{
			nScanfed = fscanf(pFile, "%c", &c);
			if (c != CR && c != LF)
				strDeviceName += c;
		} while (nScanfed != EOF && (c != CR) && (c != LF));
		nScanfed = fscanf(pFile, "%d %s %d %d %d\n",
			&ControlType, pszConnectionName, &bConnected, &nMCAddress, &Param1);
		if (nScanfed == 5)
		{
			if (!strcmp(strDeviceName.c_str(), CDatumPsm4900_70MHz::m_pszName))
			{
				pModem = new CDatumPsm4900_70MHz;
				ConnectDevice(pModem, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CDatumPsm4900_140MHz::m_pszName))
			{
				pModem = new CDatumPsm4900_140MHz;
				ConnectDevice(pModem, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CDatumPsm4900_Hybrid::m_pszName))
			{
				pModem = new CDatumPsm4900_Hybrid;
				ConnectDevice(pModem, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CDatumPsm4900_LBand::m_pszName))
			{
				pModem = new CDatumPsm4900_LBand;
				ConnectDevice(pModem, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CDatumPsm500_LBand::m_pszName))
			{
				pModem = new CDatumPsm500_LBand;
				ConnectDevice(pModem, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CPozharskyUpConverterKvv140Ver2002::m_pszName))
			{
				pUpConverter = new CPozharskyUpConverterKvv140Ver2002;
				ConnectDevice(pUpConverter, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CPozharskyUpConverterKvv140Ver2004::m_pszName))
			{
				pUpConverter = new CPozharskyUpConverterKvv140Ver2004;
				ConnectDevice(pUpConverter, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CPozharskyUpConverterKvv140Ver2011::m_pszName))
			{
				pUpConverter = new CPozharskyUpConverterKvv140Ver2011;
				ConnectDevice(pUpConverter, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CPozharskyDownConverterKvn140Ver2002::m_pszName))
			{
				pDownConverter = new CPozharskyDownConverterKvn140Ver2002;
				ConnectDevice(pDownConverter, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CPozharskyDownConverterKvn140Ver2004::m_pszName))
			{
				pDownConverter = new CPozharskyDownConverterKvn140Ver2004;
				ConnectDevice(pDownConverter, ControlType, pszConnectionName, nMCAddress);
			}
			else if (!strcmp(strDeviceName.c_str(), CPozharskyDownConverterKvn140Ver2011::m_pszName))
			{
				pDownConverter = new CPozharskyDownConverterKvn140Ver2011;
				ConnectDevice(pDownConverter, ControlType, pszConnectionName, nMCAddress);
			}
		}
	} while (nScanfed != EOF);
	if (pFile)
		fclose(pFile);

	char szAddressToListen[128];
	AddressToString(szAddressToListen, IP);
	printf("Listening port %d on address %s\n", port, szAddressToListen);

	CSnmpSocket sock;
	if (!sock.Bind(port, IP))
	{
		printf("Cannot listen port %d on address %s, error code = %d\n", port, szAddressToListen, sock.GetLastErrorCode());
		return;
	}

	if (pModem)
	{
		printf("'%s' is serviced", pModem->GetName());
		if (pModem->GetSerialPort())
			printf(" on	serial port '%s'", pModem->GetSerialPort()->GetName());
		printf("\n");
	}
	if (pDownConverter)
	{
		printf("'%s' is serviced", pDownConverter->GetName());
		if (pDownConverter->GetSerialPort())
			printf(" on	serial port '%s'", pDownConverter->GetSerialPort()->GetName());
		printf("\n");
	}
	if (pUpConverter)
	{
		printf("'%s' is serviced", pUpConverter->GetName());
		if (pUpConverter->GetSerialPort())
			printf(" on	serial port '%s'", pUpConverter->GetSerialPort()->GetName());
		printf("\n");
	}

	cSnmpDatagram requestDatagram;
	cSnmpVariable var;
	char szSrcIP[128];

	while (1)
	{
		if (!sock.waitForDatagram(requestDatagram))
		{
			printf("Cannot read from socket, error code = %d\n", sock.GetLastErrorCode());
			Sleep(10*1000);
			continue;
		}
					
		if (requestDatagram.isResponse())
			continue; // stranger response

		printf("==============================================================\n");
		time_t t; time(&t);
		printf( "%s", ctime(&t) );

		
		bool bGet = requestDatagram.isGetRequest();
		if (bGet)
			printf("GET ");
		else
			printf("SET ");

		bool bOK = false;
		var = requestDatagram.m_Variable;
		printf("%s\n", var.m_OID.getSymbolicName().c_str());

		IPADDRESS_TYPE srcIP = requestDatagram.getSourceAddress();
		AddressToString(szSrcIP, srcIP);
		printf("Source = %s\n", szSrcIP);

		if (var.m_OID.isPartOfOID(OidDatumModemMIB, OidDatumModemMIBLen))
		{
			if (pModem)
				bOK = (MC_OK == processDatumRequest(pModem, var, bGet));
			else
				printf("Modem object is not created, check CFG-file!\n");
		}
		else if (var.m_OID.isPartOfOID(OidPozharskyUpConverter, OidPozharskyUpConverterLen))
		{
			if (pUpConverter)
				bOK = (MC_OK == processPozharskyUpRequest(pUpConverter, var, bGet));
			else
				printf("UP-converter object is not created, check CFG-file!\n");
		}
		else if (var.m_OID.isPartOfOID(OidPozharskyDownConverter, OidPozharskyDownConverterLen))
		{
			if (pDownConverter)
				bOK = (MC_OK == processPozharskyDownRequest(pDownConverter, var, bGet));
			else
				printf("DOWN-converter object is not created, check CFG-file!\n");
		}
			
		if (bOK && bGet)
		{
			sock.SendGetResponse(requestDatagram.srcIpAddress_, requestDatagram.m_FromIpPort,
				requestDatagram.m_version, requestDatagram.m_Community,
				requestDatagram.m_RequestID, var);
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s cfg_file\n", argv[0]);
		return 0;
	}

	// real lines of cfg-file, one line per device
	char *pszFileName = argv[1];
	FILE *pCfgFile = fopen(pszFileName, "r");
	if (!pCfgFile)
	{
		printf("Cannot read config file %s, quitting\n", pszFileName);
		return 1;
	}
	char szLine[1024*8];
	char szDeviceCfgFileName[1024*8];
	char szIpAddress[1024*8];
	char szIpPort[1024*8];
	while (1)
	{
		szLine[0] = 0;
		char c = 0;
		while (c != 0x0D && c != 0x0A)
		{
			int nScanfed = fscanf(pCfgFile, "%c", &c);
			if (nScanfed != 0 && nScanfed != EOF)
			{
				if (c != 0x0A && c != 0x0A)
					strncat(szLine, &c, 1);
			}
			else
				break;
		}
		if (strlen(szLine) == 0)
			break;
		if (szLine[0] == '#')
			continue; // comment
		IPADDRESS_TYPE listenToAddress = INADDR_ANY;
		unsigned short listenToPort = SNMP_PORT;
		BOOL bSuccess = FALSE;
		if (sscanf(szLine, "%s %s %s", szDeviceCfgFileName, szIpAddress, szIpPort) == 3)
		{
			listenToAddress = StringToAddress(szIpAddress);
			unsigned int port = 0;
			if (sscanf(szIpPort, "%d", &port) == 1)
			{
				listenToPort = LOWORD(port);
				bSuccess = TRUE;
			}
			else
				printf("Cannot parse port number in line\n\t'%s', %d used\n", szLine, listenToPort);
		}
		else if (sscanf(szLine, "%s %s", szDeviceCfgFileName, szIpAddress, szIpPort) == 2)
		{
			listenToAddress = StringToAddress(szIpAddress);
			bSuccess = TRUE;
		}
		if (!bSuccess)
		{
			printf("Cannot parse line\n\t'%s'\n", szLine);
			continue; // next line
		}
		
		ListenRequests(szDeviceCfgFileName, listenToAddress, listenToPort);
	}
	fclose(pCfgFile);

	getch();

	return 0;
}

