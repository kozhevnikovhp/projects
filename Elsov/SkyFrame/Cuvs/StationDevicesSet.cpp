// StationDevicesSet.cpp : implementation file
//

#include "stdafx.h"
#include "StationDevicesSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationDevicesSet

IMPLEMENT_DYNAMIC(CStationDevicesSet, CRecordset)

CStationDevicesSet::CStationDevicesSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CStationDevicesSet)
	m_DeviceID = 0;
	m_StationID = 0;
	m_Name = _T("");
	m_IP = 0;
	m_Type = 0;
	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	m_Param1 = 0;
	m_nParams = 1;
}


CString CStationDevicesSet::GetDefaultConnect()
{
	return _T("ODBC;DSN=CUVS_DB");
}

CString CStationDevicesSet::GetDefaultSQL()
{
	return _T("[StationDevices]");
}

void CStationDevicesSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CStationDevicesSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[DeviceID]"), m_DeviceID);
	RFX_Long(pFX, _T("[StationID]"), m_StationID);
	RFX_Text(pFX, _T("[Name]"), m_Name);
	RFX_Long(pFX, _T("[IP]"), m_IP);
	RFX_Long(pFX, _T("[DeviceType]"), m_Type);
	//}}AFX_FIELD_MAP
	pFX->SetFieldType(CFieldExchange::param);
	RFX_Long(pFX,"", m_Param1);
}

/////////////////////////////////////////////////////////////////////////////
// CStationDevicesSet diagnostics

#ifdef _DEBUG
void CStationDevicesSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CStationDevicesSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
