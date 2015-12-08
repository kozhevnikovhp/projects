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
#include "ThreadObject.h"


const char CR = 0x0D;
const char LF = 0x0A;
const char *PSZ_SEPARATOR = "=============================================================";

void ConnectDevice(CDevice *pDevice, MC_ControlType ControlType, const char *pszConnectionName, unsigned int nMCAddress)
{
	pDevice->SetControlType(ControlType);
	pDevice->SetConnectionName(pszConnectionName);
	pDevice->SetMCBusAddress(nMCAddress);
	pDevice->Connect();
}

class ListeningThread : public CThreadObject
{
public:
	ListeningThread(const std::string &strChannelName,
					IPADDRESS_TYPE IP, unsigned int port, int nMCAddress,
					CDatumModem *pModem,
					CPozharskyUpDownConverter *pUpConverter,
					CPozharskyUpDownConverter *pDownConverter)
	{
		strChannelName_ = strChannelName;
		IP_ = IP;
		port_ = port;
		nMCAddress_ = nMCAddress;
		pModem_ = pModem;
		pUpConverter_ = pUpConverter;
		pDownConverter_ = pDownConverter;
	}

protected:
	virtual void OnThreadStarted()
	{
		char szAddressToListen[128];
		AddressToString(szAddressToListen, IP_);
		strIPandPort_ = szAddressToListen;
		strIPandPort_ += ":";
		char szTmp[128];
		strIPandPort_ += itoa(port_, szTmp, 10);
		printf("Listening %s\n", strIPandPort_.c_str());

		if (pModem_)
		{
			printf("'%s' is serviced", pModem_->GetName());
			if (pModem_->GetSerialPort())
				printf(" on	serial port '%s:%d'", pModem_->GetSerialPort()->GetName(), nMCAddress_);
			printf("\n");
		}

		if (pDownConverter_)
		{
			printf("'%s' is serviced", pDownConverter_->GetName());
			if (pDownConverter_->GetSerialPort())
				printf(" on	serial port '%s:d'", pDownConverter_->GetSerialPort()->GetName(), nMCAddress_);
			printf("\n");
		}
		if (pUpConverter_)
		{
			printf("'%s' is serviced", pUpConverter_->GetName());
			if (pUpConverter_->GetSerialPort())
				printf(" on	serial port '%s:d'", pUpConverter_->GetSerialPort()->GetName(), nMCAddress_);
			printf("\n");
		}

		bSuccesfullySet_ = true;
		if (!sock_.Bind(port_, IP_))
		{
			printf("Cannot listen %s, error code = %d\n", strIPandPort_.c_str(), sock_.GetLastErrorCode());
			bSuccesfullySet_ = false;
		}
		printf("%s\n", PSZ_SEPARATOR);
	}

	virtual LOGICAL ThreadBody()
	{
		if (!bSuccesfullySet_)
			return LOGICAL_FALSE;

		if (!sock_.waitForRequest(requestDatagram_))
		{
			printf("Cannot read from socket, error code = %d\n", sock_.GetLastErrorCode());
			Sleep(1000);
			return LOGICAL_TRUE;
		}
					
		if (!requestDatagram_.isRequest())
			return LOGICAL_TRUE; // stranger response

		printf("%s\n", PSZ_SEPARATOR);
		time_t t; time(&t);
		printf( "%s", ctime(&t) );

		printf("Channel %s: ", strChannelName_.c_str());
		bool bGet = requestDatagram_.isGetRequest() || requestDatagram_.isGetNextRequest();
		if (bGet)
			printf("GET ");
		else
			printf("SET ");

		bool bOK = false;
		var_ = requestDatagram_.m_Variable;
		printf("%s\n", var_.m_OID.getSymbolicName().c_str());

		IPADDRESS_TYPE srcIP = requestDatagram_.getSourceAddress();
		AddressToString(szSrcIP_, srcIP);
		printf("Source = %s\n", szSrcIP_);

		if (var_.m_OID.isPartOfOID(OidDatumModemMIB, OidDatumModemMIBLen))
		{
			if (pModem_)
				bOK = (MC_OK == processDatumRequest(pModem_, var_, bGet));
			else
				printf("Modem object is not created, check CFG-file!\n");
		}
		else if (var_.m_OID.isPartOfOID(OidPozharskyUpConverter, OidPozharskyUpConverterLen))
		{
			if (pUpConverter_)
				bOK = (MC_OK == processPozharskyUpRequest(pUpConverter_, var_, bGet));
			else
				printf("UP-converter object is not created, check CFG-file!\n");
		}
		else if (var_.m_OID.isPartOfOID(OidPozharskyDownConverter, OidPozharskyDownConverterLen))
		{
			if (pDownConverter_)
				bOK = (MC_OK == processPozharskyDownRequest(pDownConverter_, var_, bGet));
			else
				printf("DOWN-converter object is not created, check CFG-file!\n");
		}
			
		if (bOK && bGet)
		{
			sock_.SendGetResponse(requestDatagram_.srcIpAddress_, requestDatagram_.m_FromIpPort,
				requestDatagram_.m_version, requestDatagram_.m_Community,
				requestDatagram_.m_RequestID, var_);
		}
		return LOGICAL_TRUE;
	}

	virtual void OnThreadStopping()
	{
	}

	virtual void OnThreadEnded()
	{
	}

protected:
	CSnmpSocket sock_;
	std::string strChannelName_;
	IPADDRESS_TYPE IP_;
	unsigned int port_;
	int nMCAddress_;
	std::string strIPandPort_;
	bool bSuccesfullySet_;

	CDatumModem *pModem_;
	CPozharskyUpDownConverter *pUpConverter_;
	CPozharskyUpDownConverter *pDownConverter_;

	cSnmpDatagram requestDatagram_;
	cSnmpVariable var_;
	char szSrcIP_[128];	
};

void ListenRequests(IPADDRESS_TYPE IP, unsigned short port,
					const std::string &strChannelName,
					const std::string strDeviceName,
					const std::string strConnectionName, int nMCAddress)
{
	printf("%s\n%s\n", PSZ_SEPARATOR, strChannelName.c_str());
	CDatumModem *pModem = NULL;
	CPozharskyUpDownConverter *pUpConverter = NULL;
	CPozharskyUpDownConverter *pDownConverter = NULL;

	if (!strcmp(strDeviceName.c_str(), CDatumPsm4900_70MHz::m_pszName))
	{
		pModem = new CDatumPsm4900_70MHz;
		ConnectDevice(pModem, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CDatumPsm4900_140MHz::m_pszName))
	{
		pModem = new CDatumPsm4900_140MHz;
		ConnectDevice(pModem, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CDatumPsm4900_Hybrid::m_pszName))
	{
		pModem = new CDatumPsm4900_Hybrid;
		ConnectDevice(pModem, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CDatumPsm4900_LBand::m_pszName))
	{
		pModem = new CDatumPsm4900_LBand;
		ConnectDevice(pModem, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CDatumPsm500_LBand::m_pszName))
	{
		pModem = new CDatumPsm500_LBand;
		ConnectDevice(pModem, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CDatum_M7_LMod_LDem::m_pszName))
	{
		pModem = new CDatum_M7_LMod_LDem;
		ConnectDevice(pModem, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CPozharskyUpConverterKvv140Ver2002::m_pszName))
	{
		pUpConverter = new CPozharskyUpConverterKvv140Ver2002;
		ConnectDevice(pUpConverter, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CPozharskyUpConverterKvv140Ver2004::m_pszName))
	{
		pUpConverter = new CPozharskyUpConverterKvv140Ver2004;
		ConnectDevice(pUpConverter, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CPozharskyUpConverterKvv140Ver2011::m_pszName))
	{
		pUpConverter = new CPozharskyUpConverterKvv140Ver2011;
		ConnectDevice(pUpConverter, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CPozharskyDownConverterKvn140Ver2002::m_pszName))
	{
		pDownConverter = new CPozharskyDownConverterKvn140Ver2002;
		ConnectDevice(pDownConverter, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CPozharskyDownConverterKvn140Ver2004::m_pszName))
	{
		pDownConverter = new CPozharskyDownConverterKvn140Ver2004;
		ConnectDevice(pDownConverter, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}
	else if (!strcmp(strDeviceName.c_str(), CPozharskyDownConverterKvn140Ver2011::m_pszName))
	{
		pDownConverter = new CPozharskyDownConverterKvn140Ver2011;
		ConnectDevice(pDownConverter, MC_SERIAL_PORT, strConnectionName.c_str(), nMCAddress);
	}

	ListeningThread *pThreadObject = new ListeningThread(strChannelName, IP, port, nMCAddress, pModem, pUpConverter, pDownConverter);
	pThreadObject->Run();
	Sleep(1000);
}

std::string getLine(FILE *pFile)
{
	while (1)
	{
		std::string strLine;
		char c = 0;
		while (c != 0x0D && c != 0x0A)
		{
			int nScanfed = fscanf(pFile, "%c", &c);
			if (nScanfed == 0 || nScanfed == EOF)
				return strLine;
			if (c != 0x0A && c != 0x0A)
				strLine += c;
		}
		if (strLine.empty())
			continue; //next line
		if (strLine[0] == '#')
			continue; //commented, next line
		return strLine;
	}
	return "";
}

int main(int argc, char* argv[])
{
	/*IPADDRESS_TYPE thisIP = StringToAddress("10.192.1.219");
	IPADDRESS_TYPE thatIP = StringToAddress("10.192.1.3");
	CSnmpSocket testSock;
	if (!testSock.Bind(161, thisIP))
	{
		printf("Cannot bind, EC = %d\n", testSock.GetLastErrorCode());
		return 1;
	}

	unsigned int id[] = {1,3,6,1,4,1,7840,3,1,2,2,2,1,1,16,1};
	cSnmpOID oid(id, ELEMENT_COUNT(id));
	testSock.SendSnmpGetNextRequest(thatIP, "public", oid, 46211);
	cSnmpDatagram dgm;
	testSock.GetSnmpReply(dgm);*/

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

	int nListened = 0;
	while (1)
	{
		std::string strChannelName = getLine(pCfgFile);
		if (strChannelName.empty())
			break;
		std::string strListenIP = getLine(pCfgFile);
		if (strListenIP.empty())
			break;
		std::string strListenPort = getLine(pCfgFile);
		if (strListenPort.empty())
			break;
		std::string strDeviceName = getLine(pCfgFile);
		if (strDeviceName.empty())
			break;
		std::string strConnection = getLine(pCfgFile);
		if (strConnection.empty())
			break;
		std::string strMCAddress = getLine(pCfgFile);
		if (strMCAddress.empty())
			break;
		IPADDRESS_TYPE listenToAddress = StringToAddress(strListenIP.c_str());
		int iPort = SNMP_PORT;
		sscanf(strListenPort.c_str(), "%d", &iPort);
		unsigned short listenToPort = LOWORD(iPort);
		
		int nMCAddress = 0;
		sscanf(strMCAddress.c_str(), "%d", &nMCAddress);

		ListenRequests(listenToAddress, listenToPort, strChannelName, strDeviceName, strConnection, nMCAddress);
		++nListened;
	}
	fclose(pCfgFile);
	printf("Totally %d listened IP-Addresses\n", nListened);

	getch();

	return 0;
}

