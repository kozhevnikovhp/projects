#if !defined(AFX_TMIAVAILABLEPARAMSSET_H__BE37A7FF_1EB1_4D2A_9BD5_D3697F47C961__INCLUDED_)
#define AFX_TMIAVAILABLEPARAMSSET_H__BE37A7FF_1EB1_4D2A_9BD5_D3697F47C961__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiAvailableParamsSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiAvailableParamsSet recordset

class CTmiAvailableParamsSet : public CRecordset
{
public:
	CTmiAvailableParamsSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CTmiAvailableParamsSet)

// Field/Param Data
	//{{AFX_FIELD(CTmiAvailableParamsSet, CRecordset)
	long	m_TmiType;
	CString	m_TmiParamDescription;
	//}}AFX_FIELD
	long m_Param1;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiAvailableParamsSet)
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

#endif // !defined(AFX_TMIAVAILABLEPARAMSSET_H__BE37A7FF_1EB1_4D2A_9BD5_D3697F47C961__INCLUDED_)
