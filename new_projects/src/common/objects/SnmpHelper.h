// Snmp.h - header file for SNMP stuff
// Created:	September 20, 2004
// Author:	Evgeny Kozhevnikov evgeny@kozhevnikov.org
//
// Revision history
// September 20, 2004 - initial creation
//
// Remarks:

#ifndef __SNMP_H_INCLUDED__
#define __SNMP_H_INCLUDED__

#include <WinSNMP.h>

// Forward declarations
class CSnmpSession;

class CSnmpEntity
{
	// Construction/destruction
public:
	CSnmpEntity(HSNMP_ENTITY hEntity = INVALID_HANDLE_VALUE);
	CSnmpEntity(CSnmpSession *pSession, const char *pszEntity);
	virtual ~CSnmpEntity();

	// Public methods
public:
	BOOL Create(CSnmpSession *pSession, const char *pszEntity);
	BOOL Kill();
	HSNMP_ENTITY Handle() { return m_hEntity; }
	BOOL GetName(char *pszBufferForName, unsigned int NameLen);
	// Protected methods
protected:
	// Protected members
protected:
	HSNMP_ENTITY m_hEntity;
};

class CSnmpContext
{
	// Construction/destruction
public:
	CSnmpContext(HSNMP_CONTEXT hContext = INVALID_HANDLE_VALUE);
	CSnmpContext(CSnmpSession *pSession, char *pszContext);
	virtual ~CSnmpContext();

	// Public methods
public:
	BOOL Create(CSnmpSession *pSession, char *pszContext);
	BOOL Kill();
	HSNMP_CONTEXT Handle() { return m_hContext; }
	BOOL GetName(char *pszBufferForName, unsigned int NameLen);
	// Protected methods
protected:
	// Protected members
protected:
	HSNMP_CONTEXT m_hContext;
};

class CSnmpPDU
{
	// Construction/destruction
public:
	CSnmpPDU(HSNMP_SESSION hSession, HSNMP_PDU hPDU = INVALID_HANDLE_VALUE);
	CSnmpPDU(CSnmpSession *pSession, smiINT PduType);
	virtual ~CSnmpPDU();

	// Public methods
public:
	BOOL Create(CSnmpSession *pSession, smiINT PduType);
	BOOL Kill();
	HSNMP_PDU Handle() { return m_hPDU; }
	BOOL SetOID(const char *pszOID);
	// Protected methods
protected:
	// Protected members
protected:
	HSNMP_PDU m_hPDU;
	HSNMP_SESSION m_hSession;
};

class CSnmpSession
{
	// Construction/destruction
public:
	CSnmpSession();
	virtual ~CSnmpSession();

	// Public methods
public:
	BOOL Create(HWND hWnd = NULL, UINT uMsg = 0);
	BOOL Kill();
	BOOL SendMessage(CSnmpEntity *pSrcEntity, CSnmpEntity *pDestEntity, CSnmpContext *pContext, CSnmpPDU *pPDU);
	HSNMP_SESSION Handle() { return m_hSession; }
	// Protected methods
protected:
	virtual void OnTimeout() {}
	virtual void OnNotificationEvent(CSnmpEntity *pSrc, CSnmpEntity *pDst, CSnmpContext *pContext, CSnmpPDU *pPDU) {}
	static SNMPAPI_STATUS InitSNMP();
	static SNMPAPI_STATUS FinitSNMP();
	static SNMPAPI_STATUS CALLBACK NotificationHandler(HSNMP_SESSION hSession, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID lpClientData);
	// Protected members
protected:
	HSNMP_SESSION m_hSession;
	static unsigned int m_SessionCount;
	static SNMPAPI_STATUS m_SnmpStatus;
	static smiUINT32 m_MayorVersion, m_MinorVersion;
	static smiUINT32 m_SupportLevel;
	static smiUINT32 m_TranslateMode;
	static smiUINT32 m_RetransmitMode;
};


#endif // __SNMP_H_INCLUDED__
