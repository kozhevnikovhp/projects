// TmiSet.cpp : implementation file
//

#include "stdafx.h"
#include "TmiDatabase.h"
#include "TmiSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmiSet

IMPLEMENT_DYNAMIC(CTmiSet, CRecordset)

CTmiSet::CTmiSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CTmiSet)
	m_StationID = 0;
	m_Azs = FALSE;
	m_SN = 0.0;
	m_Level = 0.0;
	m_Offset = 0;
	m_T = 0.0;
	m_Power = 0.0;
	m_nFields = 8;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CTmiSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=TMI_DB");
}

CString CTmiSet::GetDefaultSQL()
{
	return _T("[Tmi]");
}

void CTmiSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CTmiSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Date(pFX, _T("[Time]"), m_Time);
	RFX_Long(pFX, _T("[StationID]"), m_StationID);
	RFX_Bool(pFX, _T("[Azs]"), m_Azs);
	RFX_Double(pFX, _T("[SN]"), m_SN);
	RFX_Double(pFX, _T("[Level]"), m_Level);
	RFX_Long(pFX, _T("[Offset]"), m_Offset);
	RFX_Double(pFX, _T("[T]"), m_T);
	RFX_Double(pFX, _T("[Power]"), m_Power);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTmiSet diagnostics

#ifdef _DEBUG
void CTmiSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTmiSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
