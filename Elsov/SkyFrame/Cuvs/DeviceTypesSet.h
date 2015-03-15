#if !defined(AFX_DEVICETYPESSET_H__59748B04_F63E_4E1B_9985_B5874821F47D__INCLUDED_)
#define AFX_DEVICETYPESSET_H__59748B04_F63E_4E1B_9985_B5874821F47D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeviceTypesSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeviceTypesSet recordset

class CDeviceTypesSet : public CRecordset
{
public:
	CDeviceTypesSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CDeviceTypesSet)

// Field/Param Data
	//{{AFX_FIELD(CDeviceTypesSet, CRecordset)
	long	m_DeviceType;
	CString	m_Description;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceTypesSet)
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

#endif // !defined(AFX_DEVICETYPESSET_H__59748B04_F63E_4E1B_9985_B5874821F47D__INCLUDED_)
