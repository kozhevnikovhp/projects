// ArmHubFrame.h : interface of the CArmHubFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARMHUBFRAME_H__850AA253_2201_11D5_9670_008048B59EAA__INCLUDED_)
#define AFX_ARMHUBFRAME_H__850AA253_2201_11D5_9670_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CArmHubFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CArmHubFrame();
	DECLARE_DYNCREATE(CArmHubFrame)

// Attributes
public:
	CSplitterWnd m_SplitterWnd;

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CArmHubFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArmHubFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CArmHubFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARMHUBFRAME_H__850AA253_2201_11D5_9670_008048B59EAA__INCLUDED_)
