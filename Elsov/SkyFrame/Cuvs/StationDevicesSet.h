#if !defined(AFX_STATIONDEVICESSET_H__1F056599_78B2_455D_BBAA_818A5D6AE1A0__INCLUDED_)
#define AFX_STATIONDEVICESSET_H__1F056599_78B2_455D_BBAA_818A5D6AE1A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationDevicesSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationDevicesSet recordset

class CStationDevicesSet : public CRecordset
{
public:
	CStationDevicesSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CStationDevicesSet)

// Field/Param Data
	//{{AFX_FIELD(CStationDevicesSet, CRecordset)
	long	m_DeviceID;
	long	m_StationID;
	CString	m_Name;
	long	m_IP;
	long	m_Type;
	//}}AFX_FIELD
	long m_Param1;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationDevicesSet)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONDEVICESSET_H__1F056599_78B2_455D_BBAA_818A5D6AE1A0__INCLUDED_)
