// ComPortHelperFrame.h : interface of the CComPortHelperFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPORTHELPERFRAME_H__34CF3A92_38E2_4435_986E_5FE7A6AF228A__INCLUDED_)
#define AFX_COMPORTHELPERFRAME_H__34CF3A92_38E2_4435_986E_5FE7A6AF228A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CComPortHelperFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CComPortHelperFrame();
	DECLARE_DYNCREATE(CComPortHelperFrame)

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CComPortHelperFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComPortHelperFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	DWORD m_TimerID;
	
	afx_msg void OnUpdateSentBytes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReadBytes(CCmdUI* pCmdUI);

protected:
	//{{AFX_MSG(CComPortHelperFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPORTHELPERFRAME_H__34CF3A92_38E2_4435_986E_5FE7A6AF228A__INCLUDED_)
