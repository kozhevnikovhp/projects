// ActionsSet.cpp : implementation file
//

#include "stdafx.h"
#include "tmidatabase.h"
#include "ActionsSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActionsSet

IMPLEMENT_DYNAMIC(CActionsSet, CRecordset)

CActionsSet::CActionsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CActionsSet)
	m_StationID = 0;
	m_DeltaLevel = 0.0;
	m_nFields = 3;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams += 3;
	m_strFilter = "StationID = ? AND Time > ? AND Time < ?";
	m_strSort = "Time";
	m_StartTime = m_EndTime = CTime::GetCurrentTime();
}


CString CActionsSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=TMI_DB");
}

CString CActionsSet::GetDefaultSQL()
{
	return _T("[Actions]");
}

void CActionsSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CActionsSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[StationID]"), m_StationID);
	RFX_Date(pFX, _T("[Time]"), m_Time);
	RFX_Double(pFX, _T("[DeltaLevel]"), m_DeltaLevel);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX, _T(""), m_StationIDParam);
	RFX_Date(pFX, _T(""), m_StartTime);
	RFX_Date(pFX, _T(""), m_EndTime);
}

/////////////////////////////////////////////////////////////////////////////
// CActionsSet diagnostics

#ifdef _DEBUG
void CActionsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CActionsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
