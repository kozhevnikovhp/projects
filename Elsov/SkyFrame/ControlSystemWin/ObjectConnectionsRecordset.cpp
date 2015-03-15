// ObjectConnectionsRecordset.cpp : implementation file
//

#include "stdafx.h"
#include "controlsystemwin.h"
#include "ObjectConnectionsRecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectConnectionsRecordset

IMPLEMENT_DYNAMIC(CObjectConnectionsRecordset, CRecordset)

CObjectConnectionsRecordset::CObjectConnectionsRecordset(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CObjectConnectionsRecordset)
	m_ID = 0;
	m_USE_PARENT = 0;
	m_TYPE = 0;
	m_SerialPortNo = 0;
	m_CONNECTED = 0;
	m_TelnetOptions = _T("");
	m_SerialPortOptions = _T("");
	m_SerialPortBusAddress = 0;
	m_nFields = 8;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams = 1;
}


CString CObjectConnectionsRecordset::GetDefaultSQL()
{
	return _T("[ObjectConnections]");
}

void CObjectConnectionsRecordset::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CObjectConnectionsRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Long(pFX, _T("[USE_PARENT]"), m_USE_PARENT);
	RFX_Long(pFX, _T("[TYPE]"), m_TYPE);
	RFX_Long(pFX, _T("[SerialPortNo]"), m_SerialPortNo);
	RFX_Long(pFX, _T("[CONNECTED]"), m_CONNECTED);
	RFX_Text(pFX, _T("[TelnetOptions]"), m_TelnetOptions);
	RFX_Text(pFX, _T("[SerialPortOptions]"), m_SerialPortOptions);
	RFX_Long(pFX, _T("[SerialPortBusAddress]"), m_SerialPortBusAddress);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
}

BOOL CObjectConnectionsRecordset::LoadConnectionParams(CMCParams &params, long ID)
{
	if (!FindByID(ID))
		return FALSE;

	params.m_bUSE_PARENT = m_USE_PARENT;
	params.m_CONNECTED = m_CONNECTED;
	//params.m_SerialPortBusAddress = m_SerialBusPortAddress;
	params.m_TYPE = (MC_Type)m_TYPE;
	params.m_SerialPortNo = m_SerialPortNo;
	params.m_SerialPortOptions = m_SerialPortOptions;
	params.m_TelnetOptions = m_TelnetOptions;
	return TRUE;
}

BOOL CObjectConnectionsRecordset::SaveConnectionParams(CMCParams &params, long ID)
{
	if (!FindByID(ID))
	{
		return FALSE;
	}
	SetFieldDirty(NULL);
	Edit();

	m_USE_PARENT = params.m_bUSE_PARENT;
	m_CONNECTED = params.m_CONNECTED ;
	m_SerialPortBusAddress = params.m_SerialPortBusAddress;
	m_TYPE = params.m_TYPE;
	m_SerialPortNo = params.m_SerialPortNo;
	m_SerialPortOptions = params.m_SerialPortOptions;
	m_TelnetOptions = params.m_TelnetOptions;
	return Update();
}

BOOL CObjectConnectionsRecordset::AddDefaultParams(long ID)
{
	AddNew();
	m_ID = ID;
	return Update();
}

BOOL CObjectConnectionsRecordset::FindByID(long ID)
{
	m_strFilter = "ID = ?";
	m_Param1 = ID;
	if (IsOpen())
		Requery();
	else
		Open();

	return (!IsEOF());
}

/////////////////////////////////////////////////////////////////////////////
// CObjectConnectionsRecordset diagnostics

#ifdef _DEBUG
void CObjectConnectionsRecordset::AssertValid() const
{
	CRecordset::AssertValid();
}

void CObjectConnectionsRecordset::Dump(CDumpContext &dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


