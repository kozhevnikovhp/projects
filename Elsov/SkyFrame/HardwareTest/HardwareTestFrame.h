// HardwareTestFrame.h : interface of the CHardwareTestFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HARDWARETESTFRAME_H__1C2CEEFC_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
#define AFX_HARDWARETESTFRAME_H__1C2CEEFC_300A_11D4_BBDF_E46DF892F25A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHardwareTestFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CHardwareTestFrame();
	DECLARE_DYNCREATE(CHardwareTestFrame)

// Attributes
public:

// Operations
public:
	void Refresh() { m_SplitterWnd.RecalcLayout(); }
	CWnd *GetPane(int row, int col) { return m_SplitterWnd.GetPane(row, col); }

// Overrides
	//{{AFX_VIRTUAL(CHardwareTestFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHardwareTestFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CSplitterWnd m_SplitterWnd;

// Generated message map functions
protected:
	//{{AFX_MSG(CHardwareTestFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HARDWARETESTFRAME_H__1C2CEEFC_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
