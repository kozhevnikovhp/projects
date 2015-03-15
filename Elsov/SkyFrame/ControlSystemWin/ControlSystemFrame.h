// ControlSystemFrame.h : interface of the CControlSystemFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLSYSTEMFRAME_H__FDCC6B51_491E_4508_8231_0863211F9A39__INCLUDED_)
#define AFX_CONTROLSYSTEMFRAME_H__FDCC6B51_491E_4508_8231_0863211F9A39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CControlSystemFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CControlSystemFrame();
	DECLARE_DYNCREATE(CControlSystemFrame)

// Attributes
public:

// Operations
public:
	void Refresh() { m_SplitterVert.RecalcLayout(); m_SplitterHor.RecalcLayout(); }
	CWnd *GetTopAbstractPane() { return m_SplitterHor.GetPane(0, 0); }
	CWnd *GetBottomAbstractPane() { return m_SplitterHor.GetPane(1, 0); }

// Overrides
	//{{AFX_VIRTUAL(CControlSystemFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlSystemFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CSplitterWnd m_SplitterVert, m_SplitterHor;

// Generated message map functions
protected:
	//{{AFX_MSG(CControlSystemFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLSYSTEMFRAME_H__FDCC6B51_491E_4508_8231_0863211F9A39__INCLUDED_)
