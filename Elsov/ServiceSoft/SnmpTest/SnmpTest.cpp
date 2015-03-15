// SnmpTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include "SnmpHelper.h"

class CMySession : public CSnmpSession
{
	// Construction/destruction
public:
	CMySession() {}
	virtual ~CMySession() {}

	// Public methods
public:
	// Protected methods
protected:
	virtual void OnTimeout() { printf("Timeout\n"); }
	virtual void OnNotificationEvent(CSnmpEntity *pSrc, CSnmpEntity *pDst, CSnmpContext *pContext, CSnmpPDU *pPDU);
	// Protected members
protected:
};

//virtual
void CMySession::OnNotificationEvent(CSnmpEntity *pSrc, CSnmpEntity *pDst, CSnmpContext *pContext, CSnmpPDU *pPDU)
{
	char szSrcName[128];
	BOOL bSuccess = pSrc->GetName(szSrcName, sizeof(szSrcName)/sizeof(szSrcName[0]));
	DWORD EC = SnmpGetLastError(Handle());
	printf("From %s\n", szSrcName);
	char szDstName[128];
	bSuccess = pDst->GetName(szDstName, sizeof(szDstName)/sizeof(szDstName[0]));
	printf("To %s\n", szDstName);
	char szCommunity[128];
	bSuccess = pContext->GetName(szCommunity, sizeof(szCommunity)/sizeof(szCommunity[0]));
	printf("Community %s\n", szCommunity);

	char szVarName[1500];
	HSNMP_PDU hPDU = pPDU->Handle();
	HSNMP_VBL hVbl;
	SnmpGetPduData(hPDU, NULL, NULL, NULL, NULL, &hVbl);
	EC = SnmpGetLastError(Handle());
	int count = SnmpCountVbl(hVbl);
	for (int i = 1; i <= count; i++)
	{
		smiOID name;
		smiVALUE value;
		SnmpGetVb(hVbl, i, &name, &value);
		EC = SnmpGetLastError(Handle());
		SnmpOidToStr(&name, sizeof(szVarName)/sizeof(szVarName[0]), szVarName);
		switch (value.syntax)
		{
		case SNMP_SYNTAX_INT32:
			printf("%s = %d\n", szVarName, value.value.uNumber);
			break;
		case SNMP_SYNTAX_CNTR32:
			printf("%s = %d\n", szVarName, value.value.uNumber);
			break;
		case SNMP_SYNTAX_TIMETICKS:
			printf("%s = %d\n", szVarName, value.value.uNumber);
			break;
		case SNMP_SYNTAX_OCTETS:
			char szStr[1024];
			memset(szStr, 0, sizeof(szStr));
			strncpy(szStr, (const char *)value.value.string.ptr, value.value.string.len);
			printf("%s = %s\n", szVarName, szStr);
			break;
		}
	}

	printf("\n");
}

void getUpStatus(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.6840.1.1");
	session.SendMessage(&src, &dest, &context, &pdu);

}

void getDownStatus(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.6840.1.2");
	session.SendMessage(&src, &dest, &context, &pdu);
}

void getUnitStatus(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.1.1.1"); // name
	session.SendMessage(&src, &dest, &context, &pdu);

	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.1.1.2"); // model
	session.SendMessage(&src, &dest, &context, &pdu);

	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.1.1.3"); // serial No
	session.SendMessage(&src, &dest, &context, &pdu);

	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.1.1.4"); // version
	session.SendMessage(&src, &dest, &context, &pdu);
}

void getModIf(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.1.1.1"); // freq
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.1.1.2"); //offset
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.1.1.3"); //level
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.1.1.4"); //on/off
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.1.1.5"); //modulation
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.1.1.6"); //spectrum
	session.SendMessage(&src, &dest, &context, &pdu);
}

void getModData(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.2.1.1"); // bit rate
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.2.1.3"); // FEC type
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.2.1.4"); // FEC code rate
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.2.1.5"); // diff encoder
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.2.2.2.1.9"); // FEC option
	session.SendMessage(&src, &dest, &context, &pdu);
}

void getDemodIf(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.2.1.1.1"); // freq
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.2.1.1.2"); // sweep range
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.2.1.1.7"); // modulation
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.2.1.1.8"); // spectrum
	session.SendMessage(&src, &dest, &context, &pdu);
}

void getDemodStatus(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.1.2.1.2"); // offset
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.1.2.1.3"); // lvl
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.1.2.1.4"); // S/N
	session.SendMessage(&src, &dest, &context, &pdu);
}

void getDemodData(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.2.2.1.1"); // bit rate
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.2.2.1.5"); // FEC type
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.2.2.1.6"); // FEC code rate
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.2.2.1.7"); // diff decoder
	session.SendMessage(&src, &dest, &context, &pdu);
	
	pdu.SetOID(".1.3.6.1.4.1.7840.3.1.2.3.2.2.1.11"); // FEC option //????
	session.SendMessage(&src, &dest, &context, &pdu);
}

void getUpConverterData(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.6840.1.1.1"); // ON/OFF
	session.SendMessage(&src, &dest, &context, &pdu);

	pdu.SetOID(".1.3.6.1.4.1.6840.1.1.2"); // Freq
	session.SendMessage(&src, &dest, &context, &pdu);

	pdu.SetOID(".1.3.6.1.4.1.6840.1.1.3"); // att
	session.SendMessage(&src, &dest, &context, &pdu);
}

void getDownConverterData(CMySession &session, CSnmpEntity &src, CSnmpEntity &dest, CSnmpContext &context)
{
	CSnmpPDU pdu(&session, SNMP_PDU_GET);

	pdu.SetOID(".1.3.6.1.4.1.6840.1.2.1"); // ON/OFF
	session.SendMessage(&src, &dest, &context, &pdu);

	pdu.SetOID(".1.3.6.1.4.1.6840.1.2.2"); // Freq
	session.SendMessage(&src, &dest, &context, &pdu);

	pdu.SetOID(".1.3.6.1.4.1.6840.1.2.3"); // att
	session.SendMessage(&src, &dest, &context, &pdu);
}

int main(int argc, char* argv[])
{
	CMySession session;
	BOOL bOpened = session.Create();

//	SNMPAPI_STATUS status;
	DWORD EC;

	CSnmpEntity dest(&session, "127.0.0.1");
	CSnmpEntity src(&session, "127.0.0.1");
	CSnmpContext context(&session, "public");

	EC = SnmpGetLastError(session.Handle());
	int nRequest = 0;
	while (1)
	{
		printf("Request %d\n", ++nRequest);

		//getUpStatus(session, src, dest, context);
		//getDownStatus(session, src, dest, context);
		//getUnitStatus(session, src, dest, context);

		//getModIf(session, src, dest, context);
		//getModData(session, src, dest, context);

		//getDemodIf(session, src, dest, context);
		//getDemodStatus(session, src, dest, context);
		//getDemodData(session, src, dest, context);

		getUpConverterData(session, src, dest, context);
		getDownConverterData(session, src, dest, context);

		Sleep(5000);
	}

	return 0;
}

