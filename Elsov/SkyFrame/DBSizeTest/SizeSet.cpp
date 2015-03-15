// SizeSet.cpp : implementation file
//

#include "stdafx.h"
#include "DBSizeTest.h"
#include "SizeSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSizeSet

IMPLEMENT_DYNAMIC(CSizeSet, CRecordset)

CSizeSet::CSizeSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CSizeSet)
	m_Time = CTime::GetCurrentTime();
	m_ParamID = 0;
	m_Value = 0.0;
	m_nFields = 3;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CSizeSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=DB_SIZE");
}

CString CSizeSet::GetDefaultSQL()
{
	return _T("[Tmi]");
}

void CSizeSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CSizeSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Date(pFX, _T("[Time]"), m_Time);
	RFX_Long(pFX, _T("[ParamID]"), m_ParamID);
	RFX_Double(pFX, _T("[Value]"), m_Value);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSizeSet diagnostics

#ifdef _DEBUG
void CSizeSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSizeSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
