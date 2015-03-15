/* GranSim2DDoc.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	21 Jan 2008 - initial creation
	15 Sep 2008 - line form
	24 Sep 2008 - editor tool bar
	10 Oct 2008 - reflecting wall creation functionality
	11 Oct 2008 - linear/point particle source creation functionality
	13 Oct 2008 - status bar text from design; object selection
	16 Oct 2008 - splitted "object common form" and "geometry object common form"; Particle size and shape form
	13 Nov 2008 - material forms
	10 Dec 2008 - simulation stuff
	04 Jan 2009 - reflecting rectangle
	05 Jan 2009 - objects deletion
*/

#include "stdafx.h"
#include "GranSim2D.h"

#include "GranSim2DDoc.h"
#include "GranSim2DProjectView.h"
#include "GranSim2DView.h"
#include "ControlForms.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DDoc

IMPLEMENT_DYNCREATE(CGranSim2DDoc, CDocument)

BEGIN_MESSAGE_MAP(CGranSim2DDoc, CDocument)
	//{{AFX_MSG_MAP(CGranSim2DDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DDoc construction/destruction

CGranSim2DDoc::CGranSim2DDoc()
{
	// TODO: add one-time construction code here

}

CGranSim2DDoc::~CGranSim2DDoc()
{
}

BOOL CGranSim2DDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_Design.ClearAll();
	m_pRenderer->Init();	

	return TRUE;
}

//virtual
BOOL CGranSim2DDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (IsModified())
		TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");

	FILE *fd = fopen(lpszPathName, "r");
	if (!fd)
	{
		MessageBox(NULL, "Cannot open file", "Error", MB_ICONSTOP);
		return FALSE;
	}

	DeleteContents();
	SetModifiedFlag();  // dirty during de-serialize

	TRY
	{
		m_pRenderer->Init();
		m_Design.LoadFromFile(fd);
		m_pRenderer->UpdateExtents();
		m_pRenderer->SetDefaultView();
		fclose(fd);
	}
	CATCH_ALL(e)
	{
		MessageBox(NULL, "Fatal error happened during design loading", "Error", MB_ICONSTOP);
		return FALSE;
	}
	END_CATCH_ALL

	SetModifiedFlag(FALSE);     // start off with unmodified

	return TRUE;
}

//virtual
BOOL CGranSim2DDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	FILE *fd = fopen(lpszPathName, "w");
	if (!fd)
	{
		MessageBox(NULL, "Cannot open file", "Error", MB_ICONSTOP);
		return FALSE;
	}

	TRY
	{
		m_Design.SaveToFile(fd);
		fclose(fd);
	}
	CATCH_ALL(e)
	{
		MessageBox(NULL, "Fatal error happened during design saving", "Error", MB_ICONSTOP);
		return FALSE;
	}
	END_CATCH_ALL

	SetModifiedFlag(FALSE);     // back to unmodified

	return TRUE;        // success
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DDoc diagnostics

#ifdef _DEBUG
void CGranSim2DDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGranSim2DDoc::Dump(CDumpContext &dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DDoc commands


/////////////////////////////////////////////////////////////////////////////
// CGranSim2DFrame

IMPLEMENT_DYNCREATE(CGranSim2DFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CGranSim2DFrame, CFrameWnd)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OBJECT, OnUpdateObject)
	//{{AFX_MSG_MAP(CGranSim2DFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_DRAG_SCROLL, OnViewDragScroll)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAG_SCROLL, OnUpdateViewDragScroll)
	ON_COMMAND(ID_VIEW_INITIALVIEW, OnViewInitialView)
	ON_COMMAND(ID_VIEW_SELECT, OnViewSelect)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELECT, OnUpdateViewSelect)
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOM_RECTANGLE, OnViewZoomRectangle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_RECTANGLE, OnUpdateViewZoomRectangle)
	ON_COMMAND(ID_VIEW_EDITMODE, OnViewEditMode)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITMODE, OnUpdateViewEditMode)
	ON_COMMAND(ID_EDIT_CREATEREFLECTINGWALL, OnEditCreateReflectingWall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATEREFLECTINGWALL, OnUpdateEditCreateReflectingWall)
	ON_COMMAND(ID_EDIT_CREATEPARTICLESOURCE_LINEAR, OnEditCreateParticleSourceLinear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATEPARTICLESOURCE_LINEAR, OnUpdateEditCreateParticleSourceLinear)
	ON_COMMAND(ID_EDIT_CREATEPARTICLESOURCE_POINT, OnEditCreateParticleSourcePoint)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATEPARTICLESOURCE_POINT, OnUpdateEditCreateParticleSourcePoint)
	ON_COMMAND(ID_EDIT_CREATENEWPARTICLETYPE, OnEditCreateNewParticleType)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATENEWPARTICLETYPE, OnUpdateEditCreateNewParticleType)
	ON_COMMAND(ID_EDIT_CREATENEWMATERIAL, OnEditCreateNewMaterial)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATENEWMATERIAL, OnUpdateEditCreateNewMaterial)
	ON_COMMAND(ID_SIMULATION_START, OnSimulationStart)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_START, OnUpdateSimulationStart)
	ON_COMMAND(ID_SIMULATION_STOP, OnSimulationStop)
	ON_COMMAND(ID_SIMULATION_PAUSE, OnSimulationPause)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_CREATEREFLECTINGRECTANGLE, OnEditCreateReflectingRectangle)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CREATEREFLECTINGRECTANGLE, OnUpdateEditCreateReflectingRectangle)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT MainButtons[] =
{
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
		ID_SEPARATOR,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
	ID_EDIT_DELETE,
		ID_SEPARATOR,
	ID_FILE_PRINT,
		ID_SEPARATOR,
	ID_APP_ABOUT
};

static UINT PlaneViewButtons[] =
{
	ID_VIEW_ZOOM_IN,
	ID_VIEW_ZOOM_OUT,
	ID_VIEW_INITIALVIEW,
	ID_VIEW_SELECT,
	ID_VIEW_ZOOM_RECTANGLE,
	ID_VIEW_DRAG_SCROLL,
		ID_SEPARATOR,
	ID_VIEW_EDITMODE
};

static UINT EditorPanelButtons[] =
{
	ID_EDIT_CREATEREFLECTINGWALL,
	ID_EDIT_CREATEREFLECTINGRECTANGLE,
	ID_EDIT_CREATEPARTICLESOURCE_LINEAR,
	ID_EDIT_CREATEPARTICLESOURCE_POINT,
	ID_VIEW_ZOOM_OUT,
	ID_VIEW_INITIALVIEW,
	ID_VIEW_SELECT,
	ID_VIEW_ZOOM_RECTANGLE,
	ID_VIEW_DRAG_SCROLL,
		ID_SEPARATOR,
	ID_VIEW_EDITMODE
};

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_OBJECT
};

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DFrame construction/destruction

CGranSim2DFrame::CGranSim2DFrame()
{
	m_EditorToolBarPosition.x = m_EditorToolBarPosition.y = 0; // means NOT INITIALIZED
}

CGranSim2DFrame::~CGranSim2DFrame()
{
}

int CGranSim2DFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_MAINFRAME) ||
		!m_ToolBar.SetButtons(MainButtons, sizeof(MainButtons)/sizeof(UINT)))
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
	m_ToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_ToolBar);

	// Plane view toolbar
	if (!m_Design2DViewToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_Design2DViewToolBar.LoadToolBar(IDR_DESIGN_2D_VIEW_TOOLBAR) ||
		!m_Design2DViewToolBar.SetButtons(PlaneViewButtons, sizeof(PlaneViewButtons)/sizeof(UINT)) )
	{
		TRACE0("Failed to create 2D View bar\n");
		return -1;      // fail to create
	}
	m_Design2DViewToolBar.SetWindowText("2DView Toolbar");
	m_Design2DViewToolBar.EnableDocking(CBRS_ALIGN_RIGHT);
	DockControlBar(&m_Design2DViewToolBar, AFX_IDW_DOCKBAR_RIGHT);
	
	// Editor toolbar
	if (!m_EditorToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_EditorToolBar.LoadToolBar(IDR_EDITOR_TOOLBAR) ||
		!m_EditorToolBar.SetButtons(EditorPanelButtons, sizeof(EditorPanelButtons)/sizeof(UINT)) )
	{
		TRACE0("Failed to create editor bar\n");
		return -1;      // fail to create
	}
	m_EditorToolBar.SetWindowText("Editor panel");
	m_EditorToolBar.EnableDocking(0);
	ShowControlBar(&m_EditorToolBar, FALSE, FALSE);
	FloatControlBar(&m_EditorToolBar, CPoint(300,50), 0);
	
	m_TimerID = SetTimer(1, 1000, NULL);
	
	return 0;
}

BOOL CGranSim2DFrame::PreCreateWindow(CREATESTRUCT &cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DFrame diagnostics

#ifdef _DEBUG
void CGranSim2DFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CGranSim2DFrame::Dump(CDumpContext &dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DFrame message handlers


BOOL CGranSim2DFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) 
{
	// create a splitter with 1 row, 2 columns
	if (!m_SplitterVert.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create vert. splitter\n");
		return FALSE;
	}

	// add the second splitter pane - which is a nested splitter with 2 rows
	if (!m_SplitterHor.CreateStatic(
		&m_SplitterVert,     // our parent window is the first splitter
		2, 1,               // the new splitter is 2 rows, 1 column
		WS_CHILD | WS_VISIBLE/* | WS_BORDER*/,  // style, WS_BORDER is needed
		m_SplitterVert.IdFromRowCol(0, 0)
			// new splitter is in the first row, 2nd column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}	 

	m_SplitterVert.SetColumnInfo(0, 300, 0);
	
	if (!m_SplitterHor.CreateView(0, 0,
		pContext->m_pNewViewClass, CSize(100, 100), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}
	
	if (!m_SplitterHor.CreateView(1, 0,
		RUNTIME_CLASS(CAbstractForm), CSize(100, 100), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	if (!m_SplitterVert.CreateView(0, 1,
		RUNTIME_CLASS(CGranSim2DView), CSize(10, 10), pContext))
	{
		TRACE0("Failed to create third pane\n");
		return FALSE;
	}
	m_p2DView = (CGranSim2DView *)m_SplitterVert.GetPane(0, 1);
	CGranSim2DDoc *pDoc = (CGranSim2DDoc *)pContext->m_pCurrentDoc;
	m_pDesign = pDoc->GetDesign();
	m_pRenderer = m_p2DView->GetDesignRenderer();
	m_pProjectView = (CGranSim2DProjectView *)m_SplitterHor.GetPane(0, 0);
	m_pProjectView->SetDesign(m_pDesign);
	m_pProjectView->SetRenderer(m_pRenderer);
	m_pProjectView->CreateForms(m_SplitterHor);
	m_pRenderer->SetProjectView(m_pProjectView);
  
	return TRUE;
}

void CGranSim2DFrame::OnUpdateObject(CCmdUI *pCmdUI)
{
	CString str = m_p2DView->GetDisplayedText();
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

void CGranSim2DFrame::OnViewDragScroll() 
{
	m_pRenderer->SetDragScrollMode();
}

void CGranSim2DFrame::OnUpdateViewDragScroll(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsInDragScrollMode());
}

void CGranSim2DFrame::OnViewInitialView() 
{
	m_pRenderer->SetDefaultView();	
}

void CGranSim2DFrame::OnViewSelect() 
{
	m_pRenderer->SetSelectMode();
}

void CGranSim2DFrame::OnUpdateViewSelect(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsInSelectMode());
}

void CGranSim2DFrame::OnViewZoomIn() 
{
	m_pRenderer->ZoomIn();
}

void CGranSim2DFrame::OnViewZoomOut() 
{
	m_pRenderer->ZoomOut();
}

void CGranSim2DFrame::OnViewZoomRectangle() 
{
	m_pRenderer->SetZoomRectangleMode();
}

void CGranSim2DFrame::OnUpdateViewZoomRectangle(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsInZoomRectangleMode());
}

void CGranSim2DFrame::OnViewEditMode() 
{
	m_pRenderer->ToggleEditingMode();
	ShowControlBar(&m_EditorToolBar, m_pRenderer->IsEditingEnabled(), FALSE);
	if (m_EditorToolBarPosition.x == 0 && m_EditorToolBarPosition.y == 0)
	{ // position is not initialized yet
		CRect rect(0,0,0,0);
		GetClientRect(rect);
		ClientToScreen(rect);
		m_EditorToolBarPosition.x = rect.left+300;
		m_EditorToolBarPosition.y = rect.top+50;
		FloatControlBar(&m_EditorToolBar, m_EditorToolBarPosition, 0);
	}
}

void CGranSim2DFrame::OnUpdateViewEditMode(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pRenderer->IsEditingEnabled());
	pCmdUI->Enable(!m_pDesign->IsSimulating());
}

BOOL CGranSim2DFrame::Idle()
{
	if (!m_pDesign->IsSimulating())
		return FALSE;
	BOOL bNeedContinue = m_pDesign->SimulateOneTimeStep();
	if (bNeedContinue)
	{
		static int nCount = 0;
		++nCount;
		if ((nCount % 1) == 0)
			m_pRenderer->Redraw();
	}
	else
	{
		m_pDesign->StopSimulation();
		m_pRenderer->Redraw();
		MessageBox("Simulation ended", "Info", MB_ICONINFORMATION);
	}
	return bNeedContinue;
}

void CGranSim2DFrame::DesignCleaned()
{
	m_pProjectView->ClearAll();
}

void CGranSim2DFrame::ReflectingWallCreated(CReflectingWall *pWall, void *pWhere)
{
	m_pProjectView->ReflectingWallCreated(pWall);
	m_pRenderer->ReflectingWallCreated(pWall, pWhere);
}

void CGranSim2DFrame::ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere)
{
	m_pProjectView->ReflectingRectangleCreated(pRectangle);
	m_pRenderer->ReflectingRectangleCreated(pRectangle, pWhere);
}

void CGranSim2DFrame::DesignChanged()
{
	m_pProjectView->DesignChanged();
}

void CGranSim2DFrame::MaterialCreated(CMaterial *pMaterial, void *pWhere)
{
	m_pProjectView->MaterialCreated(pMaterial);
}

void CGranSim2DFrame::ParticleTypeCreated(CParticleType *pType, void *pWhere)
{
	m_pProjectView->ParticleTypeCreated(pType);
}

void CGranSim2DFrame::ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere)
{
	m_pProjectView->ParticlePointSourceCreated(pSource);
	m_pRenderer->ParticlePointSourceCreated(pSource, pWhere);
}

void CGranSim2DFrame::ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere)
{
	m_pProjectView->ParticleLinearSourceCreated(pSource);
	m_pRenderer->ParticleLinearSourceCreated(pSource, pWhere);
}

void CGranSim2DFrame::ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere)
{
	m_pProjectView->ObjectSelectionChanged(pObject, pWhere);
	m_pRenderer->ObjectSelectionChanged(pObject, pWhere);
}

void CGranSim2DFrame::ObjectDeleted(CGranSim2DObject *pObject, void *pWhere)
{
	m_pProjectView->ObjectDeleted(pObject);
}

void CGranSim2DFrame::OnEditCreateReflectingWall() 
{
	m_pRenderer->ToggleReflectingWallCreation();
}

void CGranSim2DFrame::OnUpdateEditCreateReflectingWall(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable(m_pRenderer->IsEditingEnabled());
	pCmdUI->SetCheck(m_pRenderer->IsEditingEnabled() && m_pDesign->IsInCreationReflectingWall() && m_pRenderer->GetEditMode() == LINE_EDIT_MODE);
}

void CGranSim2DFrame::OnEditCreateParticleSourceLinear() 
{
	m_pRenderer->ToggleParticleLinearSourceCreation();
}

void CGranSim2DFrame::OnUpdateEditCreateParticleSourceLinear(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable(m_pRenderer->IsEditingEnabled());
	pCmdUI->SetCheck(m_pRenderer->IsEditingEnabled() && m_pDesign->IsInCreationParticleLinearSource()  && m_pRenderer->GetEditMode() == LINE_EDIT_MODE);
}

void CGranSim2DFrame::OnEditCreateParticleSourcePoint() 
{
	m_pRenderer->ToggleParticlePointSourceCreation();
}

void CGranSim2DFrame::OnUpdateEditCreateParticleSourcePoint(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable(m_pRenderer->IsEditingEnabled());
	pCmdUI->SetCheck(m_pRenderer->IsEditingEnabled() && m_pDesign->IsInCreationParticlePointSource()  && m_pRenderer->GetEditMode() == POINT_EDIT_MODE);
}

void CGranSim2DFrame::OnEditCreateNewParticleType() 
{
	m_pDesign->CreateNewParticleType(LOGICAL_TRUE);
}

void CGranSim2DFrame::OnUpdateEditCreateNewParticleType(CCmdUI *pCmdUI) 
{
}

void CGranSim2DFrame::OnEditCreateNewMaterial() 
{
	m_pDesign->CreateNewMaterial(LOGICAL_TRUE);
}

void CGranSim2DFrame::OnUpdateEditCreateNewMaterial(CCmdUI *pCmdUI) 
{
}

void CGranSim2DFrame::OnEditCreateReflectingRectangle() 
{
	m_pRenderer->ToggleReflectingRectangleCreation();
}

void CGranSim2DFrame::OnUpdateEditCreateReflectingRectangle(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pRenderer->IsEditingEnabled());
	pCmdUI->SetCheck(m_pRenderer->IsEditingEnabled() && m_pDesign->IsInCreationReflectingRectangle() && m_pRenderer->GetEditMode() == RECTANGLE_EDIT_MODE);
}

void CGranSim2DFrame::OnSimulationStart() 
{
	m_pDesign->StartSimulation();
//	m_CurrentEditMode = NONE_EDIT_MODE;
	m_pRenderer->EnableEditing(LOGICAL_FALSE);
	ShowControlBar(&m_EditorToolBar, FALSE, FALSE);
}

void CGranSim2DFrame::OnUpdateSimulationStart(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_pDesign->IsSimulating());
}

void CGranSim2DFrame::OnSimulationStop() 
{
	m_pDesign->StopSimulation();
}

void CGranSim2DFrame::OnSimulationPause() 
{
	// TODO: Add your command handler code here
	
}

void CGranSim2DFrame::OnDestroy() 
{
	KillTimer(m_TimerID);
	CFrameWnd::OnDestroy();
}

void CGranSim2DFrame::OnEditDelete() 
{
	m_pRenderer->DeleteSelectedObject();
}

void CGranSim2DFrame::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CAbstractForm *pForm = m_pProjectView->GetVisibleForm();
	if (pForm)
		bEnable = pForm->CanBeDeleted();
	pCmdUI->Enable(bEnable);
}
