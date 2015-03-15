#if !defined(AFX_OBJECTCONNECTIONSRECORDSET_H__D872D9C2_8910_4ED0_9888_0253E9F0A7C8__INCLUDED_)
#define AFX_OBJECTCONNECTIONSRECORDSET_H__D872D9C2_8910_4ED0_9888_0253E9F0A7C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ObjectConnectionsRecordset.h : header file
//

#include "ControlObject.h"

// Forward declarations

/////////////////////////////////////////////////////////////////////////////
// CObjectConnectionsRecordset recordset

class CObjectConnectionsRecordset : public CRecordset
{
public:
	CObjectConnectionsRecordset(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CObjectConnectionsRecordset)

// Public methods
public:
	BOOL LoadConnectionParams(CMCParams &params, long ID);
	BOOL SaveConnectionParams(CMCParams &params, long ID);
	BOOL AddDefaultParams(long ID);

	// Protected methods
protected:
	BOOL FindByID(long ID);
// Field/Param Data
	//{{AFX_FIELD(CObjectConnectionsRecordset, CRecordset)
	long	m_ID;
	long	m_USE_PARENT;
	long	m_TYPE;
	long	m_SerialPortNo;
	long	m_CONNECTED;
	CString	m_TelnetOptions;
	CString	m_SerialPortOptions;
	long	m_SerialPortBusAddress;
	//}}AFX_FIELD
	long m_Param1;


// Overrides
	//{{AFX_VIRTUAL(CObjectConnectionsRecordset)
	public:
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTCONNECTIONSRECORDSET_H__D872D9C2_8910_4ED0_9888_0253E9F0A7C8__INCLUDED_)



