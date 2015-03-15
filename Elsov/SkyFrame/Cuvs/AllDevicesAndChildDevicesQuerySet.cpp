// AllDevicesAndChildDevicesQuerySet.cpp : implementation file
//

#include "stdafx.h"
#include "AllDevicesAndChildDevicesQuerySet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAllDevicesAndChildDevicesQuerySet

IMPLEMENT_DYNAMIC(CAllDevicesAndChildDevicesQuerySet, CRecordset)

CAllDevicesAndChildDevicesQuerySet::CAllDevicesAndChildDevicesQuerySet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CAllDevicesAndChildDevicesQuerySet)
	m_DeviceID = 0;
	m_DeviceName = _T("");
	m_StationName = _T("");
	m_DeviceType = 0;
	m_nFields = 4;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_Param1 = 0;
	m_Param2 = 0;
	m_nParams = 2;
}


CString CAllDevicesAndChildDevicesQuerySet::GetDefaultConnect()
{
	return _T("ODBC;DSN=CUVS_DB");
}

CString CAllDevicesAndChildDevicesQuerySet::GetDefaultSQL()
{
	return _T("[StationDevices],[Stations]");
}

void CAllDevicesAndChildDevicesQuerySet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CAllDevicesAndChildDevicesQuerySet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[DeviceID]"), m_DeviceID);
	RFX_Text(pFX, _T("[StationDevices].[Name]"), m_DeviceName);
	RFX_Long(pFX, _T("[StationDevices].[DeviceType]"), m_DeviceType);
	RFX_Text(pFX, _T("[Stations].[Name]"), m_StationName);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
	RFX_Long(pFX,"", m_Param2);
}

/////////////////////////////////////////////////////////////////////////////
// CAllDevicesAndChildDevicesQuerySet diagnostics

#ifdef _DEBUG
void CAllDevicesAndChildDevicesQuerySet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CAllDevicesAndChildDevicesQuerySet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
