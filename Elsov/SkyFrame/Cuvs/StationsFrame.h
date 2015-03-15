// StationsFrame.h : interface of the CStationsFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATIONSFRAME_H__5E5A3F94_92AD_44CC_8C76_14B758DE65B5__INCLUDED_)
#define AFX_STATIONSFRAME_H__5E5A3F94_92AD_44CC_8C76_14B758DE65B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Forward declarations
class CStationsView;
class CCuvsDoc;
class CAbstractForm;

class CStationsFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CStationsFrame();
	DECLARE_DYNCREATE(CStationsFrame)

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CStationsFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStationsFrame();
	CAbstractForm *GetAbstractForm();
	CAbstractForm *GetVisibleAbstractForm() { return (CAbstractForm *)m_SplitterWnd.GetPane(0,1); }
	void RecalcLayout() { m_SplitterWnd.RecalcLayout();	}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CSplitterWnd m_SplitterWnd;
	CCuvsDoc *m_pDoc;
	CStationsView *m_pStationsView;

// Generated message map functions
protected:
	//{{AFX_MSG(CStationsFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONSFRAME_H__5E5A3F94_92AD_44CC_8C76_14B758DE65B5__INCLUDED_)
