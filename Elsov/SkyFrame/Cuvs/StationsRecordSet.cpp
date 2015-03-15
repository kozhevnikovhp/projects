// StationsRecordSet.cpp : implementation file
//

#include "stdafx.h"
#include "StationsRecordSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationsRecordSet

IMPLEMENT_DYNAMIC(CStationsRecordSet, CRecordset)

CStationsRecordSet::CStationsRecordSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CStationsRecordSet)
	m_StationID = 0;
	m_ParentStationID = 0;
	m_Name = _T("");
	m_nFields = 3;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_Param1 = 0;
	m_nParams = 1;
}


CString CStationsRecordSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=CUVS_DB");
}

CString CStationsRecordSet::GetDefaultSQL()
{
	return _T("[Stations]");
}

void CStationsRecordSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CStationsRecordSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[StationID]"), m_StationID);
	RFX_Long(pFX, _T("[ParentStationID]"), m_ParentStationID);
	RFX_Text(pFX, _T("[Name]"), m_Name);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
}

/////////////////////////////////////////////////////////////////////////////
// CStationsRecordSet diagnostics

#ifdef _DEBUG
void CStationsRecordSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CStationsRecordSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
