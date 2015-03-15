// StationsView.cpp : implementation of the CStationsView class
//

#include "stdafx.h"

#include "CuvsDoc.h"
#include "StationsView.h"
#include "StationsFrame.h"

#include "Station.h"
#include "TmiQueryParametersForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationsView

IMPLEMENT_DYNCREATE(CStationsView, CDragTreeView)

BEGIN_MESSAGE_MAP(CStationsView, CDragTreeView)
	//{{AFX_MSG_MAP(CStationsView)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CDragTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CDragTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CDragTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationsView construction/destruction

CStationsView::CStationsView()
{
	m_PrevSelection=NULL;
	m_pCurrentlyVisibleForm=NULL;
}

CStationsView::~CStationsView()
{
}

BOOL CStationsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CDragTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CStationsView drawing

void CStationsView::OnDraw(CDC* pDC)
{
	// TODO: add draw code for native data here
}

void CStationsView::OnInitialUpdate()
{
	CDragTreeView::OnInitialUpdate();
	m_pTreeCtrl = &GetTreeCtrl();

	m_ImageList.Create(IDB_BITMAP_TREE,16,5,RGB(192,192,192));
	m_pTreeCtrl->SetImageList(&m_ImageList, TVSIL_NORMAL);

	long lStyleOld;
	lStyleOld = GetWindowLong(m_pTreeCtrl->m_hWnd, GWL_STYLE);
	lStyleOld |= (TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
}

/////////////////////////////////////////////////////////////////////////////
// CStationsView printing

BOOL CStationsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CStationsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CStationsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CStationsView diagnostics

#ifdef _DEBUG
void CStationsView::AssertValid() const
{
	CDragTreeView::AssertValid();
}

void CStationsView::Dump(CDumpContext& dc) const
{
	CDragTreeView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStationsView message handlers

void CStationsView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	VERIFY(menu.LoadMenu( IDR_MAINFRAME ));

	CMenu* pPopup = menu.GetSubMenu(3);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
			pWndPopupOwner);
}

void CStationsView::TurnToAbstractPane(CAbstractForm *pOld)
{
	CStationsFrame *pFrame=(CStationsFrame *)GetParentFrame();

	UINT OldIDD = pOld->GetDlgCtrlID();
	CAbstractForm *pAbstractForm = pFrame->GetAbstractForm();
	UINT NewIDD = pAbstractForm->GetDlgCtrlID();
	pOld->ShowWindow(SW_HIDE);
	pAbstractForm->ShowWindow(SW_SHOW);
	pOld->SetDlgCtrlID(NewIDD);
	pAbstractForm->SetDlgCtrlID(OldIDD);
	if (m_PrevSelection)
		pOld->BeforeHiding();
	pFrame->RecalcLayout();
}

void CStationsView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CWaitCursor cursor;

	CStationsFrame *pFrame = (CStationsFrame *)GetParentFrame();
	CAbstractForm *pOld = pFrame->GetVisibleAbstractForm();
	HTREEITEM Item = m_pTreeCtrl->GetSelectedItem();
	if (!Item)
	{
		TurnToAbstractPane(pOld);
		return;
	}

	CAttachedBlock *pOldPtr=NULL, *pNewPtr=(CAttachedBlock *)m_pTreeCtrl->GetItemData(Item);
	if (m_PrevSelection)
		pOldPtr = (CAttachedBlock *)m_pTreeCtrl->GetItemData(m_PrevSelection);

	if (!pNewPtr)
	{
		TurnToAbstractPane(pOld);
		return;
	}

	CAbstractForm *pNew = m_pDoc->m_pForms[pNewPtr->m_ViewIndex];

	UINT OldIDD = pOld->GetDlgCtrlID();
	UINT NewIDD = pNew->GetDlgCtrlID();
	if (OldIDD == NewIDD && Item == m_PrevSelection)
		return;
	if (pOldPtr)
	{
		if (!pOld->BeforeHiding())
		{
			m_pTreeCtrl->SelectItem(m_PrevSelection);
			return;
		}
	}

	pNew->m_pAttached=pNewPtr;
	m_pCurrentlyVisibleForm = NULL;
	if (pNew->SetupControls())
	{
		m_PrevSelection = Item;
		m_pCurrentlyVisibleForm = pNew;
		if (OldIDD != NewIDD)
		{
			pOld->ShowWindow(SW_HIDE);
			pNew->ShowWindow(SW_SHOW);
			pOld->SetDlgCtrlID(NewIDD);
			pNew->SetDlgCtrlID(OldIDD);
			pFrame->RecalcLayout();
		}
	}
	else
	{
		MessageBeep(0);
		m_pTreeCtrl->SelectItem(m_PrevSelection);
	}
	*pResult = 0;
}

HTREEITEM CStationsView::InsertAllNetItem()
{
	HTREEITEM Item = GetTreeCtrl().InsertItem("Вся сеть", 5, 5);
	CAttachedBlock *pAttached = new CAttachedBlock(ALL_NET_INDEX, m_pDoc->m_pForms[ALL_NET_INDEX]);
	GetTreeCtrl().SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CStationsView::InsertStation(CString &StationName, long StationID, HTREEITEM ParentItem)
{
	// Station first
	HTREEITEM Item = GetTreeCtrl().InsertItem(StationName, 4, 4, ParentItem);
	CStationAttachedBlock *pStationAttached = new CStationAttachedBlock(STATION_PROPERTIES_INDEX, m_pDoc->m_pForms[STATION_PROPERTIES_INDEX]);
	CStation *pStation = pStationAttached->GetStation();
	pStation->SetStationID(StationID);
	pStation->RebuildDevices();
	pStationAttached->SetStation(pStation);
	GetTreeCtrl().SetItemData(Item, (DWORD)pStationAttached);

	// Devices
	HTREEITEM DevicesItem = GetTreeCtrl().InsertItem("Устройства", 6, 6, Item);
	CAttachedBlock *pDevicesAttached = new CAttachedBlock(STATION_DEVICES_INDEX, m_pDoc->m_pForms[STATION_DEVICES_INDEX]);
	pDevicesAttached->SetStation(pStation);
	GetTreeCtrl().SetItemData(DevicesItem, (DWORD)pDevicesAttached);

	// Channel groups
	HTREEITEM GroupsItem = GetTreeCtrl().InsertItem("Группы каналов", 6, 6, Item);
	CAttachedBlock *pGroupsAttached = new CAttachedBlock(CHANNEL_GROUPS_INDEX, m_pDoc->m_pForms[CHANNEL_GROUPS_INDEX]);
	pGroupsAttached->SetStation(pStation);
	GetTreeCtrl().SetItemData(GroupsItem, (DWORD)pGroupsAttached);
	
	// TMI
	HTREEITEM TmiItem = GetTreeCtrl().InsertItem("Телеметрия", 6, 6, Item);
	CAttachedBlock *pTmiAttached = new CTmiAttachedBlock(TMI_PARAMETERS_INDEX, m_pDoc->m_pForms[TMI_PARAMETERS_INDEX]);
	pTmiAttached->SetStation(pStation);
	GetTreeCtrl().SetItemData(TmiItem, (DWORD)pTmiAttached);
	
	return Item;
}

void CStationsView::SetSelectedItemLabel(CString &NewLabel)
{
	HTREEITEM Item = m_pTreeCtrl->GetSelectedItem();
	ASSERT(Item);
	if (!Item)
		return;
	m_pTreeCtrl->SetItemText(Item, NewLabel);
}

//virtual
BOOL CStationsView::CanDragItem(HTREEITEM DragItem)
{
	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(DragItem);
	if (!pAttached)
		return FALSE;
	return pAttached->CanDragItem();
}

//virtual
BOOL CStationsView::CanDropItem(HTREEITEM DragItem, HTREEITEM DropItem)
{
	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(DropItem);
	if (!pAttached)
		return FALSE;
	return pAttached->CanDropItem();
}

//virtual
void CStationsView::DraggingStarted(HTREEITEM DraggedItem)
{
	m_PrevSelection = NULL;
}

//virtual
void CStationsView::DraggingCompleted(HTREEITEM DroppedItem)
{
	CStationAttachedBlock *pAttached = (CStationAttachedBlock *)m_pTreeCtrl->GetItemData(DroppedItem);
	ASSERT(pAttached);
	if (!pAttached)
		return;
	HTREEITEM ParentItem = m_pTreeCtrl->GetParentItem(DroppedItem);
	ASSERT(ParentItem);
	if (!ParentItem)
		return;
	CStationAttachedBlock *pParentAttached = (CStationAttachedBlock *)m_pTreeCtrl->GetItemData(ParentItem);
	ASSERT(pParentAttached);
	if (!pParentAttached)
		return;
	long ParentStationID = pParentAttached->GetStationID();
	long StationID = pAttached->GetStationID();
	m_pDoc->SetStationParentID(StationID, ParentStationID);
}

// virtual
void CStationsView::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!IsDraggingNow())
	{
		NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;
		CAttachedBlock *pAttached=(CAttachedBlock *)pNMTreeView->itemOld.lParam;
		delete pAttached;
	}
	*pResult = 0;
}

