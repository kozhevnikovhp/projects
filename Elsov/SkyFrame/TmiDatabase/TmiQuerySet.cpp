// TmiQuerySet.cpp : implementation file
//

#include "stdafx.h"
#include "TmiDatabase.h"
#include "TmiQuerySet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmiFromTimeToTimeQuerySet

IMPLEMENT_DYNAMIC(CTmiFromTimeToTimeQuerySet, CRecordset)

CTmiFromTimeToTimeQuerySet::CTmiFromTimeToTimeQuerySet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CTmiFromTimeToTimeQuerySet)
	m_StationID = 0;
	m_Azs = FALSE;
	m_SN = 0.0;
	m_T = 0.0;
	m_Power = 0.0;
	m_Offset = 0;
	m_Level = 0.0;
	m_nFields = 8;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_nParams += 4;
	m_strFilter = "Time>? And Time<? And StationID=? And Azs=?";
	m_strSort = "Time";
}


CString CTmiFromTimeToTimeQuerySet::GetDefaultConnect()
{
	return _T("ODBC;DSN=TMI_DB");
}

CString CTmiFromTimeToTimeQuerySet::GetDefaultSQL()
{
	return _T("[Tmi]");
}

void CTmiFromTimeToTimeQuerySet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CTmiFromTimeToTimeQuerySet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Date(pFX, _T("[Time]"), m_Time);
	RFX_Long(pFX, _T("[StationID]"), m_StationID);
	RFX_Bool(pFX, _T("[Azs]"), m_Azs);
	RFX_Double(pFX, _T("[SN]"), m_SN);
	RFX_Double(pFX, _T("[T]"), m_T);
	RFX_Double(pFX, _T("[Power]"), m_Power);
	RFX_Long(pFX, _T("[Offset]"), m_Offset);
	RFX_Double(pFX, _T("[Level]"), m_Level);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Date(pFX, _T(""), m_StartTime);
	RFX_Date(pFX, _T(""), m_EndTime);
	RFX_Long(pFX, _T(""), m_StationParam);
	RFX_Bool(pFX, _T(""), m_bAzsParam);
}

/////////////////////////////////////////////////////////////////////////////
// CTmiFromTimeToTimeQuerySet diagnostics

#ifdef _DEBUG
void CTmiFromTimeToTimeQuerySet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTmiFromTimeToTimeQuerySet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
