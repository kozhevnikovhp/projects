// TmiStationsSet.h : interface of the CTmiStationSet class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMISTATIONSSET_H__DEEFFDEE_1BC0_40CF_BF03_570D8FE3EB65__INCLUDED_)
#define AFX_TMISTATIONSSET_H__DEEFFDEE_1BC0_40CF_BF03_570D8FE3EB65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStationSet : public CRecordset
{
public:
	CStationSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CStationSet)

// Field/Param Data
	//{{AFX_FIELD(CStationSet, CRecordset)
	long	m_StationID;
	CString	m_Name;
	//}}AFX_FIELD

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationSet)
	public:
	virtual CString GetDefaultConnect();	// Default connection string
	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMISTATIONSSET_H__DEEFFDEE_1BC0_40CF_BF03_570D8FE3EB65__INCLUDED_)

