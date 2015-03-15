// DbRecordset.cpp : implementation file
//

#include "stdafx.h"
#include "DBSizeTest.h"
#include "DbRecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDbRecordset

IMPLEMENT_DYNAMIC(CDbRecordset, CRecordset)

CDbRecordset::CDbRecordset(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CDbRecordset)
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CDbRecordset::GetDefaultConnect()
{
	return _T("ODBC;DSN=");
}

CString CDbRecordset::GetDefaultSQL()
{
	return _T("");
}

void CDbRecordset::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CDbRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDbRecordset diagnostics

#ifdef _DEBUG
void CDbRecordset::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDbRecordset::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
