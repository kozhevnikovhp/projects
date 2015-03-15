#if !defined(AFX_CONTROLLEDOBJECTSRECORDSET_H__5F3561CB_DA8E_4777_A9C2_A17EECA8CBE8__INCLUDED_)
#define AFX_CONTROLLEDOBJECTSRECORDSET_H__5F3561CB_DA8E_4777_A9C2_A17EECA8CBE8__INCLUDED_

#include "ControlSystemDatabase.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ControlledObjectsRecordset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlledObjectsRecordset recordset

class CControlledObjectsRecordset : public CControlSystemRecordset
{
public:
	CControlledObjectsRecordset(CDatabase *pDatabase = NULL);
	DECLARE_DYNAMIC(CControlledObjectsRecordset)

// Field/Param Data
	//{{AFX_FIELD(CControlledObjectsRecordset, CRecordset)
	long	m_ID;
	long	m_PARENT_ID;
	CString	m_NAME;
	long	m_TYPE;
	long	m_bDeleted;
	//}}AFX_FIELD
	long m_Param1;
	long m_Param2;

// Overrides
	//{{AFX_VIRTUAL(CControlledObjectsRecordset)
	public:
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange *pFX);  // RFX support
	//}}AFX_VIRTUAL
// Public methods
public:
	long QueryTargetInstanceID();
	long CreateTargetInstance();
	long AddNewObject(long ParentID, long Type, const char *pszName);
	BOOL DeleteObject(long ID);
	BOOL DestroyObject(long ID);
	BOOL SetObjectName(long ID, CString &NewName);
	BOOL SetObjectType(long ID, long type);
	BOOL SetObjectNameAndType(long ID, CString &NewName, long type);
	BOOL SetObjectParent(long ID, long ParentID);

// Protected methods
protected:

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLLEDOBJECTSRECORDSET_H__5F3561CB_DA8E_4777_A9C2_A17EECA8CBE8__INCLUDED_)
