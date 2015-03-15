#if !defined(AFX_TMITOBEASKEDSET_H__306A78E5_74FD_4CB9_A945_026531F0ABDC__INCLUDED_)
#define AFX_TMITOBEASKEDSET_H__306A78E5_74FD_4CB9_A945_026531F0ABDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiToBeAskedSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiToBeAskedSet recordset

class CTmiToBeAskedSet : public CRecordset
{
public:
	CTmiToBeAskedSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CTmiToBeAskedSet)

// Field/Param Data
	//{{AFX_FIELD(CTmiToBeAskedSet, CRecordset)
	long	m_IP;
	CTime	m_LastTmi;
	long	m_TmiType;
	long	m_Period;
	long	m_DeviceID;
	//}}AFX_FIELD
	long m_Param1;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiToBeAskedSet)
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

#endif // !defined(AFX_TMITOBEASKEDSET_H__306A78E5_74FD_4CB9_A945_026531F0ABDC__INCLUDED_)
