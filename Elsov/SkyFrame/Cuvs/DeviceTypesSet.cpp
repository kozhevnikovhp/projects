// DeviceTypesSet.cpp : implementation file
//

#include "stdafx.h"
#include "cuvs.h"
#include "DeviceTypesSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeviceTypesSet

IMPLEMENT_DYNAMIC(CDeviceTypesSet, CRecordset)

CDeviceTypesSet::CDeviceTypesSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CDeviceTypesSet)
	m_DeviceType = 0;
	m_Description = _T("");
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CDeviceTypesSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=CUVS_DB");
}

CString CDeviceTypesSet::GetDefaultSQL()
{
	return _T("[DeviceTypes]");
}

void CDeviceTypesSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CDeviceTypesSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[DeviceType]"), m_DeviceType);
	RFX_Text(pFX, _T("[Description]"), m_Description);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDeviceTypesSet diagnostics

#ifdef _DEBUG
void CDeviceTypesSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDeviceTypesSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
