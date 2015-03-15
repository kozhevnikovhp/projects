// StationsView.cpp : implementation of the CStationsView class
//

#include "stdafx.h"
#include "Attached.h"
#include "ArmHub.h"

#include "ArmHubDoc.h"
#include "StationsView.h"
#include "ArmHubFrame.h"

// Forms
#include "AbstractForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationsView

IMPLEMENT_DYNCREATE(CStationsView, CTreeView)

BEGIN_MESSAGE_MAP(CStationsView, CTreeView)
	//{{AFX_MSG_MAP(CStationsView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationsView construction/destruction

CStationsView::CStationsView()
{
	m_PrevSelection=NULL;
	m_pTreeCtrl=NULL;
	m_pCurrentlyVisibleForm=NULL;
}

CStationsView::~CStationsView()
{
}

BOOL CStationsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CStationsView drawing

void CStationsView::OnDraw(CDC* pDC)
{
	CArmHubDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CStationsView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();


 	CArmHubFrame *pFrame=(CArmHubFrame *)GetParentFrame();
	CSplitterWnd *pSplitter=&(pFrame->m_SplitterWnd);
	m_pAbstractPane=(CAbstractForm *)pSplitter->GetPane(0,1);

	m_ImageList.Create(IDB_BITMAP_TREE,16,5,RGB(192,192,192));
	m_pTreeCtrl->SetImageList(&m_ImageList, TVSIL_NORMAL);
	m_ImageList.SetOverlayImage(0, 1);
//	m_ImageList.SetOverlayImage(1, 2);

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
	CTreeView::AssertValid();
}

void CStationsView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CArmHubDoc* CStationsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CArmHubDoc)));
	return (CArmHubDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStationsView message handlers

void CStationsView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CWaitCursor cursor;

	CArmHubFrame *pFrame=(CArmHubFrame *)GetParentFrame();
	CSplitterWnd *pSplitter=&(pFrame->m_SplitterWnd);
	CAbstractForm *pOld=(CAbstractForm *)(pSplitter->GetPane(0,1));
	HTREEITEM Item=m_pTreeCtrl->GetSelectedItem();
	if (!Item)
	{
		TurnToAbstractPane(pOld);
		return;
	}

	CAttachedBlock *pOldPtr=NULL, *pNewPtr=(CAttachedBlock *)m_pTreeCtrl->GetItemData(Item);
	if (m_PrevSelection)
		pOldPtr=(CAttachedBlock *)m_pTreeCtrl->GetItemData(m_PrevSelection);

	if ( !pNewPtr )
	{
		TurnToAbstractPane(pOld);
		return;
	}

	CAbstractForm *pNew=GetDocument()->m_pForms[pNewPtr->m_ViewIndex];

	UINT OldIDD=pOld->GetDlgCtrlID();
	UINT NewIDD=pNew->GetDlgCtrlID();
	if ( OldIDD==NewIDD && Item==m_PrevSelection) return;
	if (pOldPtr )
	{
		if (!pOld->BeforeHiding())
		{
			m_pTreeCtrl->SelectItem(m_PrevSelection);
			return;
		}
	}

	pNew->m_pAttached=pNewPtr;
	pNew->m_pStation=pNewPtr->m_pStation;
	pNew->m_pStationsView=this;
	if (pNew->SetupControls())
	{
		m_PrevSelection=Item;
		m_pCurrentlyVisibleForm=pNew;
		if ( OldIDD != NewIDD )
		{
			pOld->ShowWindow(SW_HIDE);
			pNew->ShowWindow(SW_SHOW);
			pOld->SetDlgCtrlID(NewIDD);
			pNew->SetDlgCtrlID(OldIDD);
			pSplitter->RecalcLayout();
		}
	}
	else
	{
		MessageBeep(0);
		m_pTreeCtrl->SelectItem(m_PrevSelection);
	}
	
	*pResult = 0;
}

void CStationsView::TurnToAbstractPane(CAbstractForm *pOld)
{
	CArmHubFrame *pFrame=(CArmHubFrame *)GetParentFrame();
	CSplitterWnd *pSplitter=&(pFrame->m_SplitterWnd);

	UINT OldIDD=pOld->GetDlgCtrlID();
	UINT NewIDD=m_pAbstractPane->GetDlgCtrlID();
	pOld->ShowWindow(SW_HIDE);
	m_pAbstractPane->ShowWindow(SW_SHOW);
	pOld->SetDlgCtrlID(NewIDD);
	m_pAbstractPane->SetDlgCtrlID(OldIDD);
	if (m_PrevSelection)
	{
		CAttachedBlock *pAttached=(CAttachedBlock *)m_pTreeCtrl->GetItemData(m_PrevSelection);
		pOld->BeforeHiding();
	}
	pSplitter->RecalcLayout();
}


int CStationsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	m_TimerID=SetTimer(1,1000, NULL);
	
	m_pTreeCtrl=&GetTreeCtrl();
	
	return 0;
}

void CStationsView::OnDestroy() 
{
	KillTimer(m_TimerID);
	CTreeView::OnDestroy();	
}

void CStationsView::AddStation(CStation *pStation)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem(pStation->GetName(), 4, 4);
	CAttachedBlock *pAttached=new CAttachedBlock(STATION_INDEX);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	pAttached->m_pStation=pStation;

	HTREEITEM IpAddressItem=m_pTreeCtrl->InsertItem("IP Address", 5, 5, Item);
	pAttached=new CAttachedBlock(IP_ADDRESS_INDEX);
	m_pTreeCtrl->SetItemData(IpAddressItem, (DWORD)pAttached);
	pAttached->m_pStation=pStation;

	HTREEITEM TmiItem=m_pTreeCtrl->InsertItem("Telemetric Information", 6, 6, Item);
	pAttached=new CAttachedBlock(QUERY_PARAMETERS_INDEX);
	m_pTreeCtrl->SetItemData(TmiItem, (DWORD)pAttached);
	pAttached->m_pStation=pStation;

	HTREEITEM TmiIpStatisticsItem=m_pTreeCtrl->InsertItem("IP-statistics", 6, 6, TmiItem);
	pAttached=new CAttachedBlock(TMI_IP_STATISTICS_INDEX);
	m_pTreeCtrl->SetItemData(TmiIpStatisticsItem, (DWORD)pAttached);
	pAttached->m_pStation=pStation;

	HTREEITEM TmiUdpStatisticsItem=m_pTreeCtrl->InsertItem("UDP-statistics", 6, 6, TmiItem);
	pAttached=new CAttachedBlock(TMI_UDP_STATISTICS_INDEX);
	m_pTreeCtrl->SetItemData(TmiUdpStatisticsItem, (DWORD)pAttached);
	pAttached->m_pStation=pStation;

	HTREEITEM TmiNetDeviceStatisticsItem=m_pTreeCtrl->InsertItem("Interface statistics", 6, 6, TmiItem);
	pAttached=new CAttachedBlock(TMI_NET_DEVICE_STATISTICS_INDEX);
	m_pTreeCtrl->SetItemData(TmiNetDeviceStatisticsItem, (DWORD)pAttached);
	pAttached->m_pStation=pStation;

	HTREEITEM TmiModemStatusItem=m_pTreeCtrl->InsertItem("Modem status", 6, 6, TmiItem);
	pAttached=new CAttachedBlock(TMI_MODEM_STATUS_INDEX);
	m_pTreeCtrl->SetItemData(TmiModemStatusItem, (DWORD)pAttached);
	pAttached->m_pStation=pStation;

	HTREEITEM TmiBstStatusItem=m_pTreeCtrl->InsertItem("BST status", 6, 6, TmiItem);
	pAttached=new CAttachedBlock(TMI_BST_STATUS_INDEX);
	m_pTreeCtrl->SetItemData(TmiBstStatusItem, (DWORD)pAttached);
	pAttached->m_pStation=pStation;

	HTREEITEM TelnetItem=m_pTreeCtrl->InsertItem("Telnet", 7, 7, Item);
	pAttached=new CAttachedBlock(TELNET_INDEX);
	m_pTreeCtrl->SetItemData(TelnetItem, (DWORD)pAttached);
	pAttached->m_pStation=pStation;

}

CStation *CStationsView::GetSelectedStation()
{
	HTREEITEM Item=m_pTreeCtrl->GetSelectedItem();
	if (!Item)
		return NULL;
	HTREEITEM ParentItem=Item;//m_pTreeCtrl->GetParentItem(Item);
	while (ParentItem)
	{
		Item=ParentItem;
		ParentItem=m_pTreeCtrl->GetParentItem(Item);
	}
	CAttachedBlock *pAttached=(CAttachedBlock *)m_pTreeCtrl->GetItemData(Item);
	return pAttached->m_pStation;
}
	
void CStationsView::DeleteSelectedStation()
{
	HTREEITEM Item=m_pTreeCtrl->GetSelectedItem();
	if (!Item)
		return;
	HTREEITEM ParentItem=Item;//m_pTreeCtrl->GetParentItem(Item);
	while (ParentItem)
	{
		Item=ParentItem;
		ParentItem=m_pTreeCtrl->GetParentItem(Item);
	}
	HTREEITEM NextItem=m_pTreeCtrl->GetNextItem(Item, TVGN_NEXT);
	m_pTreeCtrl->SelectItem(NextItem);
	m_pTreeCtrl->DeleteItem(Item);
	m_PrevSelection=NULL;
}

void CStationsView::UpdateSelectedStationName()
{
	HTREEITEM Item=m_pTreeCtrl->GetSelectedItem();
	if (!Item)
		return;
	HTREEITEM ParentItem=Item;//m_pTreeCtrl->GetParentItem(Item);
	while (ParentItem)
	{
		Item=ParentItem;
		ParentItem=m_pTreeCtrl->GetParentItem(Item);
	}
	CAttachedBlock *pAttached=(CAttachedBlock *)m_pTreeCtrl->GetItemData(Item);
	m_pTreeCtrl->SetItemText(Item, pAttached->m_pStation->GetName());
}

void CStationsView::Clear()
{
	m_pTreeCtrl->DeleteAllItems();
}


void CStationsView::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;
	CAttachedBlock *pAttached=(CAttachedBlock *)pNMTreeView->itemOld.lParam;
	delete pAttached;
	*pResult = 0;
}

void CStationsView::OnTimer(UINT nIDEvent) 
{
	MarkAlarm(TVI_ROOT);

	if (m_pCurrentlyVisibleForm)
		m_pCurrentlyVisibleForm->Timer();

	CTreeView::OnTimer(nIDEvent);
}

void CStationsView::MarkAlarm(HTREEITEM Item, BOOL bAlarm)
{
	TVITEM TVItem;
	TVItem.mask=TVIF_HANDLE;
	TVItem.hItem=Item;
	m_pTreeCtrl->GetItem(&TVItem);

	TVItem.mask= TVIF_STATE;
	TVItem.state &= 0xFFFFF0FF; // гасим старые флаги overlay image
	if (bAlarm)
		TVItem.state |= INDEXTOOVERLAYMASK(1); // устанавливаем новые флаги overlay image
	TVItem.stateMask|=TVIS_OVERLAYMASK;
	m_pTreeCtrl->SetItem(&TVItem);
}

void CStationsView::MarkAlarm(HTREEITEM Item)
{
	if (Item != TVI_ROOT)
	{
		CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(Item);
		if (pAttached)
		{
			CAbstractForm *pForm = GetDocument()->m_pForms[pAttached->m_ViewIndex];
			CStation *pStation = pAttached->m_pStation;
			if (pForm && pStation)
			{
				BOOL bAlarm = pForm->IsAlarm(pStation);
				MarkAlarm(Item, bAlarm);
			}
		}
	}
	// Все дочерние ветви
	HTREEITEM ChildItem=m_pTreeCtrl->GetChildItem(Item);
	while (ChildItem)
	{
		MarkAlarm(ChildItem);
		ChildItem=m_pTreeCtrl->GetNextItem(ChildItem, TVGN_NEXT);
	}
}