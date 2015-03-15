// WorkingHoursSet.cpp : implementation file
//

#include "stdafx.h"
#include "tmidatabase.h"
#include "WorkingHoursSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkingHoursSet

IMPLEMENT_DYNAMIC(CWorkingHoursSet, CRecordset)

CWorkingHoursSet::CWorkingHoursSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CWorkingHoursSet)
	m_Azs = FALSE;
	m_SN = 0.0;
	m_Offset = 0;
	m_nFields = 4;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams += 3;
	m_strFilter = "Time >= ? And Time <= ? And StationID = ?";
	m_strSort = "Time";
}


CString CWorkingHoursSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=TMI_DB");
}

CString CWorkingHoursSet::GetDefaultSQL()
{
	return _T("[Tmi]");
}

void CWorkingHoursSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CWorkingHoursSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Date(pFX, _T("[Time]"), m_Time);
	RFX_Bool(pFX, _T("[Azs]"), m_Azs);
	RFX_Double(pFX, _T("[SN]"), m_SN);
	RFX_Long(pFX, _T("[Offset]"), m_Offset);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Date(pFX, _T(""), m_StartTime);
	RFX_Date(pFX, _T(""), m_EndTime);
	RFX_Long(pFX, _T(""), m_StationParam);
}

/////////////////////////////////////////////////////////////////////////////
// CWorkingHoursSet diagnostics

#ifdef _DEBUG
void CWorkingHoursSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CWorkingHoursSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
