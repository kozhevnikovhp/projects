#if !defined(AFX_DBRECORDSET_H__1F2D3146_D795_4F02_B7B8_F6917DD7A896__INCLUDED_)
#define AFX_DBRECORDSET_H__1F2D3146_D795_4F02_B7B8_F6917DD7A896__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DbRecordset.h : header file
//
#include <afxdb.h>			// MFC ODBC database classes

/////////////////////////////////////////////////////////////////////////////
// CDbRecordset recordset

class CDbRecordset : public CRecordset
{
public:
	CDbRecordset(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CDbRecordset)

// Field/Param Data
	//{{AFX_FIELD(CDbRecordset, CRecordset)
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbRecordset)
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

#endif // !defined(AFX_DBRECORDSET_H__1F2D3146_D795_4F02_B7B8_F6917DD7A896__INCLUDED_)
