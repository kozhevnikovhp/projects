#if !defined(AFX_ALLDEVICESANDCHILDDEVICESQUERYSET_H__900021CF_82B2_4804_A226_33F819B37564__INCLUDED_)
#define AFX_ALLDEVICESANDCHILDDEVICESQUERYSET_H__900021CF_82B2_4804_A226_33F819B37564__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AllDevicesAndChildDevicesQuerySet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAllDevicesAndChildDevicesQuerySet recordset

class CAllDevicesAndChildDevicesQuerySet : public CRecordset
{
public:
	CAllDevicesAndChildDevicesQuerySet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CAllDevicesAndChildDevicesQuerySet)

// Field/Param Data
	//{{AFX_FIELD(CAllDevicesAndChildDevicesQuerySet, CRecordset)
	long	m_DeviceID;
	CString	m_DeviceName;
	long	m_DeviceType;
	CString	m_StationName;
	//}}AFX_FIELD
	long m_Param1;
	long m_Param2;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllDevicesAndChildDevicesQuerySet)
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

#endif // !defined(AFX_ALLDEVICESANDCHILDDEVICESQUERYSET_H__900021CF_82B2_4804_A226_33F819B37564__INCLUDED_)
