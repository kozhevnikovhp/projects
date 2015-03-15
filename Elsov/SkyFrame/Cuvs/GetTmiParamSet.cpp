// GetTmiParamSet.cpp : implementation file
//

#include "stdafx.h"
#include "GetTmiParamSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetTmiParamSet

IMPLEMENT_DYNAMIC(CGetTmiParamSet, CRecordset)

CGetTmiParamSet::CGetTmiParamSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CGetTmiParamSet)
	m_DeviceID = 0;
	m_TmiType = 0;
	m_Period = 0;
	m_Enabled = FALSE;
	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_Param1 = 0;
	m_Param2 = 0;
	m_nParams = 2;
}


CString CGetTmiParamSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=CUVS_DB");
}

CString CGetTmiParamSet::GetDefaultSQL()
{
	return _T("[TmiQueryParameters]");
}

void CGetTmiParamSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CGetTmiParamSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[DeviceID]"), m_DeviceID);
	RFX_Long(pFX, _T("[TmiType]"), m_TmiType);
	RFX_Long(pFX, _T("[Period]"), m_Period);
	RFX_Bool(pFX, _T("[Enabled]"), m_Enabled);
	RFX_Date(pFX, _T("[LastTmi]"), m_LastTmi);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
	RFX_Long(pFX,"", m_Param2);
}

/////////////////////////////////////////////////////////////////////////////
// CGetTmiParamSet diagnostics

#ifdef _DEBUG
void CGetTmiParamSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CGetTmiParamSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
