// ChannelsRecordset.cpp : implementation file
//

#include "stdafx.h"
#include "ChannelsRecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChannelsRecordset

IMPLEMENT_DYNAMIC(CChannelsRecordset, CRecordset)

CChannelsRecordset::CChannelsRecordset(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CChannelsRecordset)
	m_N_CHANNEL = 0;
	m_TRANSMITTER = 0;
	m_RECEIVER = 0;
	m_bAutoSN = FALSE;
	m_MaxSN = 0.0;
	m_MinSN = 0.0;
	m_nFields = 6;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_Param1 = 0;
	m_Param2 = 0;
	m_nParams = 2;
}


CString CChannelsRecordset::GetDefaultConnect()
{
	return _T("ODBC;DSN=CUVS_DB");
}

CString CChannelsRecordset::GetDefaultSQL()
{
	return _T("[Channels]");
}

void CChannelsRecordset::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CChannelsRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[N_CHANNEL]"), m_N_CHANNEL);
	RFX_Long(pFX, _T("[TRANSMITTER]"), m_TRANSMITTER);
	RFX_Long(pFX, _T("[RECEIVER]"), m_RECEIVER);
	RFX_Bool(pFX, _T("[AutoSN]"), m_bAutoSN);
	RFX_Double(pFX, _T("[MaxSN]"), m_MaxSN);
	RFX_Double(pFX, _T("[MinSN]"), m_MinSN);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
	RFX_Long(pFX,"", m_Param2);
}

/////////////////////////////////////////////////////////////////////////////
// CChannelsRecordset diagnostics

#ifdef _DEBUG
void CChannelsRecordset::AssertValid() const
{
	CRecordset::AssertValid();
}

void CChannelsRecordset::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
