#if !defined(AFX_SIZESET_H__C99CE162_E356_4B74_94FA_71D99B5E1812__INCLUDED_)
#define AFX_SIZESET_H__C99CE162_E356_4B74_94FA_71D99B5E1812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SizeSet.h : header file
//
#include <afxdb.h>			// MFC ODBC database classes

/////////////////////////////////////////////////////////////////////////////
// CSizeSet recordset

class CSizeSet : public CRecordset
{
public:
	CSizeSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CSizeSet)

// Field/Param Data
	//{{AFX_FIELD(CSizeSet, CRecordset)
	CTime	m_Time;
	long	m_ParamID;
	double	m_Value;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSizeSet)
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

#endif // !defined(AFX_SIZESET_H__C99CE162_E356_4B74_94FA_71D99B5E1812__INCLUDED_)
