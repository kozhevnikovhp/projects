// TmiStationsSet.cpp : implementation of the CTmiStationSet class
//

#include "stdafx.h"
#include "TmiDatabase.h"
#include "TmiStationsSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationSet implementation

IMPLEMENT_DYNAMIC(CStationSet, CRecordset)

CStationSet::CStationSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CTmiStationSet)
	m_StationID = 0;
	m_Name = _T("");
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}

CString CStationSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=TMI_DB");
}

CString CStationSet::GetDefaultSQL()
{
	return _T("[Stations]");
}

void CStationSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CTmiStationSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[StationID]"), m_StationID);
	RFX_Text(pFX, _T("[Name]"), m_Name);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CStationSet diagnostics

#ifdef _DEBUG
void CStationSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CStationSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
