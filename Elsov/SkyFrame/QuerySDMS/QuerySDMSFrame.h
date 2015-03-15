// QuerySDMSFrame.h : interface of the CQuerySDMSFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYSDMSFRAME_H__97F09AEA_9107_4E33_A605_CE5627FB08AA__INCLUDED_)
#define AFX_QUERYSDMSFRAME_H__97F09AEA_9107_4E33_A605_CE5627FB08AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CControlForm;

class CQuerySDMSFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CQuerySDMSFrame();
	DECLARE_DYNCREATE(CQuerySDMSFrame)

// Attributes
public:

// Operations
public:
	void OnIdle();

// Overrides
	//{{AFX_VIRTUAL(CQuerySDMSFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQuerySDMSFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif
	
protected:
	void AddNewForm(CView *pFormToAdd, CDocument *pDoc);
	void SwitchToForm(CControlForm *pForm);
	BOOL IsFormVisible(CControlForm *pForm);
	BOOL DeltaTimeElapsed();
	void Timer(BOOL bAnyway);

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	
	DWORD m_LastTimerTime;
	CControlForm *m_pParamsForm;
	CControlForm *m_pModIfTableForm;
	CControlForm *m_pModDataTableForm;
	CControlForm *m_pModBucTableForm;
	CControlForm *m_pDemodIfTableForm;
	CControlForm *m_pDemodDataTableForm;
	CControlForm *m_pDemodLnbTableForm;
	CControlForm *m_pDemodStatusForm;

	CControlForm *m_pCurrentlyVisibleForm;
	CQuerySDMSApp *m_pApp;

// Generated message map functions
protected:

	//{{AFX_MSG(CQuerySDMSFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewParams();
	afx_msg void OnUpdateViewParams(CCmdUI* pCmdUI);
	afx_msg void OnViewModIfTable();
	afx_msg void OnUpdateViewModIfTable(CCmdUI* pCmdUI);
	afx_msg void OnViewDemodIfTable();
	afx_msg void OnUpdateViewDemodIfTable(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnViewDemodStatus();
	afx_msg void OnUpdateViewDemodStatus(CCmdUI* pCmdUI);
	afx_msg void OnViewModDataTable();
	afx_msg void OnUpdateViewModDataTable(CCmdUI* pCmdUI);
	afx_msg void OnViewDemodDataTable();
	afx_msg void OnUpdateViewDemodDataTable(CCmdUI* pCmdUI);
	afx_msg void OnViewModBucForm();
	afx_msg void OnUpdateViewModBucForm(CCmdUI* pCmdUI);
	afx_msg void OnViewDemodLnbForm();
	afx_msg void OnUpdateViewDemodLnbForm(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYSDMSFRAME_H__97F09AEA_9107_4E33_A605_CE5627FB08AA__INCLUDED_)
