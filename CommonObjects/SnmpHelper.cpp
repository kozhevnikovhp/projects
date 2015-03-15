// Snmp.cpp - implementation file for SNMP stuff
// Created:	September 20, 2004
// Author:	Evgeny Kozhevnikov evgeny@kozhevnikov.org
//
// Revision history
// September 20, 2004 - initial creation
//
// Remarks:


#include "stdafx.h"
#include "SnmpHelper.h"

#pragma comment (lib, "WSnmp32.lib")

const char *pszUnknown = "<UNKNOWN>";

/////////////////////////////////////////////////////////////////////////////
// Class CSnmpEntity implementation

CSnmpEntity::CSnmpEntity(HSNMP_ENTITY hEntity)
{
	m_hEntity = hEntity;
}

CSnmpEntity::CSnmpEntity(CSnmpSession *pSession, const char *pszEntity)
{
	Create(pSession, pszEntity);
}

//virtual
CSnmpEntity::~CSnmpEntity()
{
	Kill();
}

BOOL CSnmpEntity::Create(CSnmpSession *pSession, const char *pszEntity)
{
	m_hEntity = SnmpStrToEntity(pSession->Handle(), pszEntity);
	if (m_hEntity == SNMPAPI_FAILURE)
		return FALSE;
	return TRUE;
}

BOOL CSnmpEntity::Kill()
{
	return (SNMPAPI_SUCCESS == SnmpFreeEntity(m_hEntity));
}

BOOL CSnmpEntity::GetName(char *pszBufferForName, unsigned int NameLen)
{
	SNMPAPI_STATUS status = SnmpEntityToStr(Handle(), NameLen, pszBufferForName);
	if (SNMPAPI_FAILURE == status)
	{
		strcpy(pszBufferForName, pszUnknown);
		return FALSE;
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Class CSnmpContext implementation

CSnmpContext::CSnmpContext(HSNMP_CONTEXT hContext)
{
	m_hContext = hContext;
}

CSnmpContext::CSnmpContext(CSnmpSession *pSession, char *pszContext)
{
	Create(pSession, pszContext);
}

//virtual
CSnmpContext::~CSnmpContext()
{
	Kill();
}

BOOL CSnmpContext::Create(CSnmpSession *pSession, char *pszContext)
{
	smiOCTETS Context;
	Context.ptr = (smiLPBYTE)pszContext;
	Context.len = lstrlen(pszContext);
	m_hContext = SnmpStrToContext(pSession->Handle(), &Context);
	if (m_hContext == SNMPAPI_FAILURE)
		return FALSE;
	return TRUE;
}

BOOL CSnmpContext::Kill()
{
	return (SNMPAPI_SUCCESS == SnmpFreeContext(m_hContext));
}

BOOL CSnmpContext::GetName(char *pszBufferForName, unsigned int NameLen)
{
	smiOCTETS Context;
	SNMPAPI_STATUS status = SnmpContextToStr(Handle(), &Context);
	if (SNMPAPI_FAILURE == status)
	{
		strcpy(pszBufferForName, pszUnknown);
		return FALSE;
	}
	strncpy(pszBufferForName, (char *)Context.ptr, Context.len);
	pszBufferForName[Context.len] = 0;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Class CSnmpPDU implementation

CSnmpPDU::CSnmpPDU(HSNMP_SESSION hSession, HSNMP_PDU hPDU)
{
	m_hSession = hSession;
	m_hPDU = hPDU;
}

CSnmpPDU::CSnmpPDU(CSnmpSession *pSession, smiINT PduType)
{
	Create(pSession, PduType);
}

//virtual
CSnmpPDU::~CSnmpPDU()
{
	Kill();
}

BOOL CSnmpPDU::Create(CSnmpSession *pSession, smiINT PduType)
{
	m_hSession = pSession->Handle();
	m_hPDU = SnmpCreatePdu(
		pSession->Handle(),
		PduType,
		NULL,	// requestID (NULL - WinSNMP will assign value)
		NULL,	// error status
		NULL,	// error index
		NULL	// VarBindList
		);
	if (m_hPDU == SNMPAPI_FAILURE)
		return FALSE;
	return TRUE;
}

BOOL CSnmpPDU::Kill()
{
	return (SNMPAPI_SUCCESS == SnmpFreePdu(m_hPDU));
}

BOOL CSnmpPDU::SetOID(const char *pszOID)
{
	smiOID name;
	SnmpStrToOid(pszOID, &name);
	HSNMP_VBL hVbl = SnmpCreateVbl(m_hSession, &name, NULL);
	SnmpSetPduData(Handle(), NULL, NULL, NULL, NULL, &hVbl);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// Class CSnmpSession implementation

//static
SNMPAPI_STATUS CSnmpSession::m_SnmpStatus = SNMPAPI_NOT_INITIALIZED;
//static
unsigned int CSnmpSession::m_SessionCount = 0;
//static
smiUINT32 CSnmpSession::m_MayorVersion = 0;
//static
smiUINT32 CSnmpSession::m_MinorVersion = 0;
//static
smiUINT32 CSnmpSession::m_SupportLevel = 0;
//static
smiUINT32 CSnmpSession::m_TranslateMode = 0;
//static
smiUINT32 CSnmpSession::m_RetransmitMode = 0;


CSnmpSession::CSnmpSession()
{
	if (!m_SessionCount)
		InitSNMP();
	++m_SessionCount;
	m_hSession = INVALID_HANDLE_VALUE;
}

//virtual
CSnmpSession::~CSnmpSession()
{
	Kill();
	--m_SessionCount;
	if (!m_SessionCount)
		FinitSNMP();
}

// methods to initialize and deinitialize SNMP subsystem

//static
SNMPAPI_STATUS CSnmpSession::InitSNMP()
{
	m_SnmpStatus = SnmpStartup(
		&m_MayorVersion, &m_MinorVersion,	// version
		&m_SupportLevel,					// Support level
		&m_TranslateMode,
		&m_RetransmitMode		
		);
	return m_SnmpStatus;
} //  InitSNMP


//static
SNMPAPI_STATUS CSnmpSession::FinitSNMP()
{
	m_SnmpStatus = SnmpCleanup();
	return m_SnmpStatus; // success
} //  FinitSNMP

//static
SNMPAPI_STATUS CALLBACK CSnmpSession::NotificationHandler(HSNMP_SESSION hSession, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID lpClientData)
{
	CSnmpSession *pThis = (CSnmpSession *)lpClientData;
	if (wParam == SNMPAPI_TL_TIMEOUT)
	{
		pThis->OnTimeout();
		return SNMPAPI_SUCCESS;
	}

	HSNMP_ENTITY hSrc, hDst;
	HSNMP_CONTEXT hContext;
	HSNMP_PDU hPdu;
	SNMPAPI_STATUS status = SnmpRecvMsg(hSession, &hSrc, &hDst, &hContext, &hPdu);
	DWORD EC = SnmpGetLastError(hSession);
	if (status != SNMPAPI_SUCCESS && EC == SNMPAPI_NOOP)
		return SNMPAPI_SUCCESS;

	CSnmpEntity Src(hSrc);
	CSnmpEntity Dst(hDst);
	CSnmpContext Context(hContext);
	CSnmpPDU PDU(hSession, hPdu);

	pThis->OnNotificationEvent(&Src, &Dst, &Context, &PDU);
	return SNMPAPI_SUCCESS;
}

BOOL CSnmpSession::Create(HWND hWnd, UINT uMsg)
{
	m_hSession = SnmpCreateSession(hWnd, uMsg, NotificationHandler, this);
	if (m_hSession == SNMPAPI_FAILURE)
		return FALSE;
	return TRUE;
}

BOOL CSnmpSession::Kill()
{
	return (SNMPAPI_SUCCESS == SnmpClose(m_hSession));
}

BOOL CSnmpSession::SendMessage(CSnmpEntity *pSrcEntity, CSnmpEntity *pDestEntity, CSnmpContext *pContext, CSnmpPDU *pPDU)
{
	SNMPAPI_STATUS Status = SnmpSendMsg(Handle(),
		pSrcEntity ? pSrcEntity->Handle() : NULL,
		pDestEntity ? pDestEntity->Handle() : NULL,
		pContext ? pContext->Handle() : NULL,
		pPDU ? pPDU->Handle() : NULL
		);
	return (Status == SNMPAPI_SUCCESS);
}
