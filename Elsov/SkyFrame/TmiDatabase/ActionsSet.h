#if !defined(AFX_ACTIONSSET_H__933DD634_808A_4DA5_A7C7_DEE0C7FBA525__INCLUDED_)
#define AFX_ACTIONSSET_H__933DD634_808A_4DA5_A7C7_DEE0C7FBA525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActionsSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CActionsSet recordset

class CActionsSet : public CRecordset
{
public:
	CActionsSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CActionsSet)

// Field/Param Data
	//{{AFX_FIELD(CActionsSet, CRecordset)
	long	m_StationID;
	CTime	m_Time;
	double	m_DeltaLevel;
	//}}AFX_FIELD
	// Params
	long	m_StationIDParam;
	CTime	m_StartTime;
	CTime	m_EndTime;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActionsSet)
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

#endif // !defined(AFX_ACTIONSSET_H__933DD634_808A_4DA5_A7C7_DEE0C7FBA525__INCLUDED_)
