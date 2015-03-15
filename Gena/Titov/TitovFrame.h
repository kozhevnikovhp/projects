// TitovFrame.h : interface of the CTitovFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TITOVFRAME_H__E497E171_D120_4227_B2E8_63531F74FDBE__INCLUDED_)
#define AFX_TITOVFRAME_H__E497E171_D120_4227_B2E8_63531F74FDBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTitovFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CTitovFrame();
	DECLARE_DYNCREATE(CTitovFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitovFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTitovFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	UINT m_TimerID;

// Generated message map functions
protected:
	//{{AFX_MSG(CTitovFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnUpdateTime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNRight(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITOVFRAME_H__E497E171_D120_4227_B2E8_63531F74FDBE__INCLUDED_)
