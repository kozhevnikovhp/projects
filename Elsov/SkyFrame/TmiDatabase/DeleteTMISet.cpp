// DeleteTMISet.cpp : implementation file
//

#include "stdafx.h"
#include "tmidatabase.h"
#include "DeleteTMISet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteTMISet

IMPLEMENT_DYNAMIC(CDeleteTMISet, CRecordset)

CDeleteTMISet::CDeleteTMISet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CDeleteTMISet)
	m_StationID = 0;
	m_SN = 0.0;
	m_bAzs = FALSE;
	m_nFields = 4;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams += 3;
	m_strFilter = "Time >= ? And Time <= ? And StationID = ?";
}


CString CDeleteTMISet::GetDefaultConnect()
{
	return _T("ODBC;DSN=TMI_DB");
}

CString CDeleteTMISet::GetDefaultSQL()
{
	return _T("[Tmi]");
}

void CDeleteTMISet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CDeleteTMISet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Date(pFX, _T("[Time]"), m_Time);
	RFX_Long(pFX, _T("[StationID]"), m_StationID);
	RFX_Double(pFX, _T("[SN]"), m_SN);
	RFX_Bool(pFX, _T("[Azs]"), m_bAzs);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Date(pFX, _T(""), m_StartTime);
	RFX_Date(pFX, _T(""), m_EndTime);
	RFX_Long(pFX, _T(""), m_StationParam);
}

/////////////////////////////////////////////////////////////////////////////
// CDeleteTMISet diagnostics

#ifdef _DEBUG
void CDeleteTMISet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDeleteTMISet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
