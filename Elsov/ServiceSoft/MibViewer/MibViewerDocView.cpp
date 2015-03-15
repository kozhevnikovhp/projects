// MibViewerDocView.cpp : implementation of the CMibViewerFrame class
//

#include "stdafx.h"
#include "MibViewer.h"

#include "MibViewerDoc.h"
#include "MibViewerDocView.h"
#include "MibTreeCtrl.h"
#include "NodeForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMibViewerView

IMPLEMENT_DYNCREATE(CMibViewerView, CTreeView)

BEGIN_MESSAGE_MAP(CMibViewerView, CTreeView)
	//{{AFX_MSG_MAP(CMibViewerView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMibViewerView construction/destruction

CMibViewerView::CMibViewerView()
{
	m_pNodeForm = NULL;
}

CMibViewerView::~CMibViewerView()
{
}

void ItemCreatedCallback(CTreeCtrl *pTreeCtrl, HTREEITEM hItem, CMibNode *pNode)
{
	pTreeCtrl->SetItemData(hItem, (DWORD)pNode);
}

void CMibViewerView::LoadMibTree(CMibTree &MibTree)
{
	LoadMibTreeToCtrl(&GetTreeCtrl(), MibTree, &ItemCreatedCallback);
}

BOOL CMibViewerView::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMibViewerView drawing

void CMibViewerView::OnDraw(CDC* pDC)
{
	CMibViewerDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CMibViewerView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();
	long lStyleOld = GetWindowLong(GetTreeCtrl().m_hWnd, GWL_STYLE);
	lStyleOld |= (TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
}

/////////////////////////////////////////////////////////////////////////////
// CMibViewerView printing

BOOL CMibViewerView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMibViewerView::OnBeginPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMibViewerView::OnEndPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMibViewerView diagnostics

#ifdef _DEBUG
void CMibViewerView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CMibViewerView::Dump(CDumpContext &dc) const
{
	CTreeView::Dump(dc);
}

CMibViewerDoc* CMibViewerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMibViewerDoc)));
	return (CMibViewerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMibViewerView message handlers


/////////////////////////////////////////////////////////////////////////////
// CMibViewerFrame

IMPLEMENT_DYNCREATE(CMibViewerFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMibViewerFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMibViewerFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMibViewerFrame construction/destruction

CMibViewerFrame::CMibViewerFrame()
{
	// TODO: add member initialization code here
	
}

CMibViewerFrame::~CMibViewerFrame()
{
}

int CMibViewerFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
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

BOOL CMibViewerFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMibViewerFrame diagnostics

#ifdef _DEBUG
void CMibViewerFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMibViewerFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMibViewerFrame message handlers


BOOL CMibViewerFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
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
		RUNTIME_CLASS(CNodeForm), CSize(100, 100), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	if (!m_SplitterVert.CreateView(0, 1,
		RUNTIME_CLASS(CEditView), CSize(10, 10), pContext))
	{
		TRACE0("Failed to create third pane\n");
		return FALSE;
	}
	CMibViewerView *pTreeView = (CMibViewerView *)m_SplitterHor.GetPane(0, 0);
	CNodeForm *pNodeForm = (CNodeForm *)m_SplitterHor.GetPane(1, 0);
	
	CMibViewerDoc *pDoc = (CMibViewerDoc *)pContext->m_pCurrentDoc;
	pDoc->SetMibTreeView(pTreeView);
	pTreeView->SetNodeForm(pNodeForm);
  
	return TRUE;
}

void CMibViewerView::OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult) 
{
	NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem)
	{
		CMibNode *pNode = (CMibNode *)GetTreeCtrl().GetItemData(hItem);
		if (pNode)
		{
			m_pNodeForm->SetupControls(pNode);
		}
	}

	*pResult = 0;
}
