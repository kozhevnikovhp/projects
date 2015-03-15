// LithoFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Litho.h"

#include "LithoFrame.h"
#include "LithoView.h"
#include "SettingsDialog.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLithoFrame

IMPLEMENT_DYNCREATE(CLithoFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CLithoFrame, CFrameWnd)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OBJECT, OnUpdateObject)
	//{{AFX_MSG_MAP(CLithoFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_INITIALVIEW, OnViewInitialView)
	ON_COMMAND(ID_VIEW_SELECT, OnViewSelect)
	ON_COMMAND(ID_VIEW_ZOOM_RECTANGLE, OnViewZoomRectangle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_RECTANGLE, OnUpdateViewZoomRectangle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELECT, OnUpdateViewSelect)
	ON_COMMAND(ID_VIEW_DRAG_SCROLL, OnViewDragScroll)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAG_SCROLL, OnUpdateViewDragScroll)
	ON_COMMAND(ID_EDIT_PHASESHIFT0, OnEditPhaseShift0)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHASESHIFT0, OnUpdateEditPhaseShift0)
	ON_COMMAND(ID_EDIT_EDITMODE, OnEditEditMode)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EDITMODE, OnUpdateEditEditMode)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_COMMAND(ID_SIMULATE, OnSimulate)
	ON_COMMAND(ID_VIEW_IMAGE_GRID, OnViewImageGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IMAGE_GRID, OnUpdateViewImageGrid)
	ON_COMMAND(ID_VIEW_MASK_GRID, OnViewMaskGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MASK_GRID, OnUpdateViewMaskGrid)
	ON_COMMAND(ID_VIEW_MASK, OnViewMask)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MASK, OnUpdateViewMask)
	ON_COMMAND(ID_EDIT_PHASESHIFT90, OnEditPhaseShift90)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHASESHIFT90, OnUpdateEditPhaseshift90)
	ON_COMMAND(ID_EDIT_PHASESHIFT180, OnEditPhaseShift180)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHASESHIFT180, OnUpdateEditPhaseShift180)
	ON_COMMAND(ID_EDIT_PHASESHIFT270, OnEditPhaseShift270)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHASESHIFT270, OnUpdateEditPhaseShift270)
	ON_COMMAND(ID_VIEW_MOUSE_GRID, OnViewMouseGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOUSE_GRID, OnUpdateViewMouseGrid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT MainButtons[] =
{
	ID_EDIT_EDITMODE,
	ID_EDIT_PHASESHIFT0,
	ID_EDIT_PHASESHIFT90,
	ID_EDIT_PHASESHIFT180,
	ID_EDIT_PHASESHIFT270,
		ID_SEPARATOR,
	ID_VIEW_MOUSE_GRID,
	ID_VIEW_ZOOM_IN,
	ID_VIEW_ZOOM_OUT,
	ID_VIEW_INITIALVIEW,
	ID_VIEW_SELECT,
	ID_VIEW_ZOOM_RECTANGLE,
	ID_VIEW_DRAG_SCROLL,
		ID_SEPARATOR,
	ID_VIEW_MASK,
	ID_VIEW_MASK_GRID,
	ID_VIEW_IMAGE_GRID,
		ID_SEPARATOR,
	ID_FILE_PRINT,
		ID_SEPARATOR,
	ID_APP_ABOUT
};

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_OBJECT
};


/////////////////////////////////////////////////////////////////////////////
// CLithoFrame construction/destruction

CLithoFrame::CLithoFrame()
{
	// TODO: add member initialization code here
	
}

CLithoFrame::~CLithoFrame()
{
}

int CLithoFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(MainButtons, sizeof(MainButtons)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CLithoFrame::PreCreateWindow(CREATESTRUCT &cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLithoFrame diagnostics

#ifdef _DEBUG
void CLithoFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CLithoFrame::Dump(CDumpContext &dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLithoFrame message handlers


void CLithoFrame::OnViewZoomIn() 
{
	m_pRenderer->IncreaseZoom(2);
}

void CLithoFrame::OnViewZoomOut() 
{
	m_pRenderer->IncreaseZoom(0.5);
}

void CLithoFrame::OnViewInitialView() 
{
	m_pRenderer->SetDefaultView();
}

void CLithoFrame::OnViewSelect() 
{
	m_pRenderer->SetSelectMode();
}

void CLithoFrame::OnUpdateViewSelect(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsInSelectMode());
}

void CLithoFrame::OnViewZoomRectangle() 
{
	m_pRenderer->SetZoomRectangleMode();
}

void CLithoFrame::OnUpdateViewZoomRectangle(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsInZoomRectangleMode());
}

void CLithoFrame::OnViewDragScroll() 
{
	m_pRenderer->SetDragScrollMode();	
}

void CLithoFrame::OnUpdateViewDragScroll(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsInDragScrollMode());
}

BOOL CLithoFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) 
{
	BOOL bRetValue = CFrameWnd::OnCreateClient(lpcs, pContext);
	CDocument *pDoc = pContext->m_pCurrentDoc;
	POSITION pos = pDoc->GetFirstViewPosition();
	m_pView = (CLithoView *)pDoc->GetNextView(pos);
	m_pRenderer = (CLithoRenderer *)m_pView->GetRenderer();

	return bRetValue;
}

void CLithoFrame::OnEditEditMode() 
{
	m_pRenderer->ToggleEditingMode();
}

void CLithoFrame::OnUpdateEditEditMode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsEditingEnabled());
	m_pRenderer->RebuildAllDisplayLists(LOGICAL_TRUE);
}

void CLithoFrame::OnEditPhaseShift0() 
{
	if (m_pRenderer->GetPSM() == PSM_0)
	{
		m_pRenderer->SetPSM(PSM_None);
		m_pRenderer->SetEditMode(NONE_EDIT_MODE);
	}
	else
	{
		m_pRenderer->SetPSM(PSM_0);
		m_pRenderer->SetEditMode(RECTANGLE_EDIT_MODE);
	}
}

void CLithoFrame::OnUpdateEditPhaseShift0(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->GetPSM() == PSM_0 && m_pRenderer->GetEditMode() == RECTANGLE_EDIT_MODE);
	pCmdUI->Enable(m_pRenderer->IsEditingEnabled());
}

void CLithoFrame::OnEditPhaseShift90() 
{
	if (m_pRenderer->GetPSM() == PSM_90)
	{
		m_pRenderer->SetPSM(PSM_None);
		m_pRenderer->SetEditMode(NONE_EDIT_MODE);
	}
	else
	{
		m_pRenderer->SetPSM(PSM_90);
		m_pRenderer->SetEditMode(RECTANGLE_EDIT_MODE);
	}
}

void CLithoFrame::OnUpdateEditPhaseshift90(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->GetPSM() == PSM_90 && m_pRenderer->GetEditMode() == RECTANGLE_EDIT_MODE);
	pCmdUI->Enable(m_pRenderer->IsEditingEnabled());
}

void CLithoFrame::OnEditPhaseShift180() 
{
	if (m_pRenderer->GetPSM() == PSM_180)
	{
		m_pRenderer->SetPSM(PSM_None);
		m_pRenderer->SetEditMode(NONE_EDIT_MODE);
	}
	else
	{
		m_pRenderer->SetPSM(PSM_180);
		m_pRenderer->SetEditMode(RECTANGLE_EDIT_MODE);
	}
}

void CLithoFrame::OnUpdateEditPhaseShift180(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->GetPSM() == PSM_180 && m_pRenderer->GetEditMode() == RECTANGLE_EDIT_MODE);
	pCmdUI->Enable(m_pRenderer->IsEditingEnabled());
}

void CLithoFrame::OnEditPhaseShift270() 
{
	if (m_pRenderer->GetPSM() == PSM_270)
	{
		m_pRenderer->SetPSM(PSM_None);
		m_pRenderer->SetEditMode(NONE_EDIT_MODE);
	}
	else
	{
		m_pRenderer->SetPSM(PSM_270);
		m_pRenderer->SetEditMode(RECTANGLE_EDIT_MODE);
	}
}

void CLithoFrame::OnUpdateEditPhaseShift270(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->GetPSM() == PSM_270 && m_pRenderer->GetEditMode() == RECTANGLE_EDIT_MODE);
	pCmdUI->Enable(m_pRenderer->IsEditingEnabled());
}

void CLithoFrame::OnUpdateObject(CCmdUI *pCmdUI)
{
	CString str = m_pView->GetDisplayedText();
	CSize size;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont != NULL) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(str);
	if (hOldFont != NULL) 
		dc.SelectObject(hOldFont);

	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, size.cx);
	pCmdUI->SetText(str);
	pCmdUI->Enable(TRUE);
}


void CLithoFrame::OnSettings() 
{
	CSettingsDialog dlg;
	dlg.m_Distance = m_pRenderer->m_Distance;
	dlg.m_MaskSize = m_pRenderer->m_MaskSize;
	dlg.m_ImageSize = m_pRenderer->m_ImageSize;
	dlg.m_ImageAccuracy = m_pRenderer->m_ImageAccuracy;
	dlg.m_WaveLength = m_pRenderer->m_WaveLength;
	dlg.m_bPositiveMask = m_pRenderer->m_bPositiveMask;
	if (dlg.DoModal() != IDOK)
		return;
	m_pRenderer->m_Distance = dlg.m_Distance;
	m_pRenderer->m_MaskSize = dlg.m_MaskSize;
	m_pRenderer->m_ImageSize = dlg.m_ImageSize;
	m_pRenderer->m_ImageAccuracy = dlg.m_ImageAccuracy;
	m_pRenderer->m_WaveLength = dlg.m_WaveLength;
	m_pRenderer->m_bPositiveMask = dlg.m_bPositiveMask;
	m_pRenderer->ReallocImage();
}

void CLithoFrame::OnSimulate() 
{
	CProgressDlg dlg;
	dlg.Create(this);
	m_pRenderer->Simulate(&dlg);
}

void CLithoFrame::OnViewImageGrid() 
{
	m_pRenderer->ToggleImageGrid();
}

void CLithoFrame::OnUpdateViewImageGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->m_bImageGridVisible);
}

void CLithoFrame::OnViewMaskGrid() 
{
	m_pRenderer->ToggleMaskGrid();
}

void CLithoFrame::OnUpdateViewMaskGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->m_bMaskGridVisible);
}

void CLithoFrame::OnViewMask() 
{
	m_pRenderer->ToggleMask();
}

void CLithoFrame::OnUpdateViewMask(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->m_bMaskVisible);
}

void CLithoFrame::OnViewMouseGrid() 
{
	m_pRenderer->ToggleGrid();
}

void CLithoFrame::OnUpdateViewMouseGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsGridEnabled());	
}
