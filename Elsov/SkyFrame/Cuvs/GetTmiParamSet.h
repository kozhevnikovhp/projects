#if !defined(AFX_GETTMIPARAMSET_H__712D4B1F_203B_4AEC_95BB_694DF4F74F54__INCLUDED_)
#define AFX_GETTMIPARAMSET_H__712D4B1F_203B_4AEC_95BB_694DF4F74F54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetTmiParamSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetTmiParamSet recordset

class CGetTmiParamSet : public CRecordset
{
public:
	CGetTmiParamSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CGetTmiParamSet)

// Field/Param Data
	//{{AFX_FIELD(CGetTmiParamSet, CRecordset)
	long	m_DeviceID;
	long	m_TmiType;
	long	m_Period;
	BOOL	m_Enabled;
	CTime	m_LastTmi;
	//}}AFX_FIELD
	long m_Param1, m_Param2;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetTmiParamSet)
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

#endif // !defined(AFX_GETTMIPARAMSET_H__712D4B1F_203B_4AEC_95BB_694DF4F74F54__INCLUDED_)
