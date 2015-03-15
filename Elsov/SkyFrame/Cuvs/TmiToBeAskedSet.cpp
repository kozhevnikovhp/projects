// TmiToBeAskedSet.cpp : implementation file
//

#include "stdafx.h"
#include "cuvs.h"
#include "TmiToBeAskedSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmiToBeAskedSet

IMPLEMENT_DYNAMIC(CTmiToBeAskedSet, CRecordset)

CTmiToBeAskedSet::CTmiToBeAskedSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CTmiToBeAskedSet)
	m_IP = 0;
	m_TmiType = 0;
	m_Period = 0;
	m_DeviceID = 0;
	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_Param1 = 0;
	m_nParams = 1;
	m_LastTmi = CTime::GetCurrentTime();
}


CString CTmiToBeAskedSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=CUVS_DB");
}

CString CTmiToBeAskedSet::GetDefaultSQL()
{
	return _T("[StationDevices],[TmiQueryParameters]");
}

void CTmiToBeAskedSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CTmiToBeAskedSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[IP]"), m_IP);
	RFX_Date(pFX, _T("[LastTmi]"), m_LastTmi);
	RFX_Long(pFX, _T("[TmiType]"), m_TmiType);
	RFX_Long(pFX, _T("[Period]"), m_Period);
	RFX_Long(pFX, _T("[TmiQueryParameters].[DeviceID]"), m_DeviceID);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
}

/////////////////////////////////////////////////////////////////////////////
// CTmiToBeAskedSet diagnostics

#ifdef _DEBUG
void CTmiToBeAskedSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTmiToBeAskedSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
