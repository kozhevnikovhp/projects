#if !defined(AFX_EVENTLOGRECORDSET_H__805807DA_172D_4224_921F_9D3F22E8D100__INCLUDED_)
#define AFX_EVENTLOGRECORDSET_H__805807DA_172D_4224_921F_9D3F22E8D100__INCLUDED_

#include "ControlSystemDatabase.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventLogRecordset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventLogRecordset recordset

class CEventLogRecordset : public CControlSystemRecordset
{
public:
	CEventLogRecordset(CDatabase *pDatabase);
	DECLARE_DYNAMIC(CEventLogRecordset)

// Field/Param Data
	//{{AFX_FIELD(CEventLogRecordset, CRecordset)
	long	m_EventID;
	long	m_EventType;
	long	m_ObjectID;
	CTime	m_EventTime;
	long	m_CommentID;
	//}}AFX_FIELD

// Public methods
public:
	void OperationStarted();
	void OperationStopped();
	void ObjectCreated(long ObjectID);
	void ObjectDeleted(long ObjectID);

// Protected methods
protected:


// Overrides
	//{{AFX_VIRTUAL(CEventLogRecordset)
	public:
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange *pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTLOGRECORDSET_H__805807DA_172D_4224_921F_9D3F22E8D100__INCLUDED_)
