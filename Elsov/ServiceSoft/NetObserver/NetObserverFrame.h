// NetObserverFrame.h : interface of the CNetObserverFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETOBSERVERFRAME_H__6C5E24F7_2105_43CE_AAAD_B17BE7353644__INCLUDED_)
#define AFX_NETOBSERVERFRAME_H__6C5E24F7_2105_43CE_AAAD_B17BE7353644__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNetObserverFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CNetObserverFrame)
public:
	CNetObserverFrame();

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CNetObserverFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetObserverFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CNetObserverFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnQueryEndSession();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETOBSERVERFRAME_H__6C5E24F7_2105_43CE_AAAD_B17BE7353644__INCLUDED_)
