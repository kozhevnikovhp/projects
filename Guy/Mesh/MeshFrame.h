// MeshFrame.h : interface of the CMeshFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHFRAME_H__90A725C5_5A33_41F5_B03A_E7C00794505F__INCLUDED_)
#define AFX_MESHFRAME_H__90A725C5_5A33_41F5_B03A_E7C00794505F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMeshFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMeshFrame();
	DECLARE_DYNCREATE(CMeshFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeshFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMeshFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMeshFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHFRAME_H__90A725C5_5A33_41F5_B03A_E7C00794505F__INCLUDED_)
