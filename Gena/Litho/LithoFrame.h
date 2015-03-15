// LithoFrame.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __LITHO_FRAME_H_INCLUDED__
#define __LITHO_FRAME_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward declarations
class CLithoView;
class CLithoRenderer;

class CLithoFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CLithoFrame();
	DECLARE_DYNCREATE(CLithoFrame)

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CLithoFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLithoFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CLithoView *m_pView;
	CLithoRenderer *m_pRenderer;

// Generated message map functions
protected:
	afx_msg void OnUpdateObject(CCmdUI *pCmdUI);
	//{{AFX_MSG(CLithoFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewInitialView();
	afx_msg void OnViewSelect();
	afx_msg void OnViewZoomRectangle();
	afx_msg void OnUpdateViewZoomRectangle(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewSelect(CCmdUI *pCmdUI);
	afx_msg void OnViewDragScroll();
	afx_msg void OnUpdateViewDragScroll(CCmdUI *pCmdUI);
	afx_msg void OnEditPhaseShift0();
	afx_msg void OnUpdateEditPhaseShift0(CCmdUI *pCmdUI);
	afx_msg void OnEditEditMode();
	afx_msg void OnUpdateEditEditMode(CCmdUI *pCmdUI);
	afx_msg void OnSettings();
	afx_msg void OnSimulate();
	afx_msg void OnViewImageGrid();
	afx_msg void OnUpdateViewImageGrid(CCmdUI* pCmdUI);
	afx_msg void OnViewMaskGrid();
	afx_msg void OnUpdateViewMaskGrid(CCmdUI* pCmdUI);
	afx_msg void OnViewMask();
	afx_msg void OnUpdateViewMask(CCmdUI* pCmdUI);
	afx_msg void OnEditPhaseShift90();
	afx_msg void OnUpdateEditPhaseshift90(CCmdUI* pCmdUI);
	afx_msg void OnEditPhaseShift180();
	afx_msg void OnUpdateEditPhaseShift180(CCmdUI* pCmdUI);
	afx_msg void OnEditPhaseShift270();
	afx_msg void OnUpdateEditPhaseShift270(CCmdUI* pCmdUI);
	afx_msg void OnViewMouseGrid();
	afx_msg void OnUpdateViewMouseGrid(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __LITHO_FRAME_H_INCLUDED__
