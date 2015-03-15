// TmiStationsView.h : interface of the CTmiStationsView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMISTATIONSVIEW_H__694A1D90_8016_427C_A4E9_7B67251C6A2A__INCLUDED_)
#define AFX_TMISTATIONSVIEW_H__694A1D90_8016_427C_A4E9_7B67251C6A2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStationSet;

class CTmiStationsView : public CRecordView
{
protected: // create from serialization only
	CTmiStationsView();
	DECLARE_DYNCREATE(CTmiStationsView)

public:
	//{{AFX_DATA(CTmiStationsView)
	enum { IDD = IDD_TMIDATABASE_FORM };
	CStationSet *m_pSet;
	CString	m_StationName;
	//}}AFX_DATA

// Attributes
public:
	CTmiDatabaseDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiStationsView)
	public:
	virtual CRecordset* OnGetRecordset();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTmiStationsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTmiStationsView)
	afx_msg void OnRecordFirst();
	afx_msg void OnRecordLast();
	afx_msg void OnRecordNext();
	afx_msg void OnRecordPrev();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TmiStationsView.cpp
inline CTmiDatabaseDoc* CTmiStationsView::GetDocument()
   { return (CTmiDatabaseDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMISTATIONSVIEW_H__694A1D90_8016_427C_A4E9_7B67251C6A2A__INCLUDED_)
