// TmiAvailableParamsSet.cpp : implementation file
//

#include "stdafx.h"
#include "cuvs.h"
#include "TmiAvailableParamsSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmiAvailableParamsSet

IMPLEMENT_DYNAMIC(CTmiAvailableParamsSet, CRecordset)

CTmiAvailableParamsSet::CTmiAvailableParamsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CTmiAvailableParamsSet)
	m_TmiType = 0;
	m_TmiParamDescription = _T("");
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_Param1 = 0;
	m_nParams = 1;
}


CString CTmiAvailableParamsSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=CUVS_DB");
}

CString CTmiAvailableParamsSet::GetDefaultSQL()
{
	return _T("[DeviceTypes],[StationDevices],[TmiAvailability],[TmiTypes]");
}

void CTmiAvailableParamsSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CTmiAvailableParamsSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[TmiAvailability].[TmiType]"), m_TmiType);
	RFX_Text(pFX, _T("[TmiTypes].[Description]"), m_TmiParamDescription);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
}

/////////////////////////////////////////////////////////////////////////////
// CTmiAvailableParamsSet diagnostics

#ifdef _DEBUG
void CTmiAvailableParamsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTmiAvailableParamsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
