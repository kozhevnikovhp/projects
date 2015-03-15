// NetworkAnalyzerView.cpp : implementation of the CNetworkAnalyzerView class
//

#include "stdafx.h"
#include "NetworkAnalyzer.h"

#include "NetworkAnalyzerDoc.h"
#include "NetworkAnalyzerView.h"
#include "Network.h"
#include "Portable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int IMAGE_INDEX_STATUS_OK = 0;
const int IMAGE_INDEX_STATUS_WARNING = 1;
const int IMAGE_INDEX_STATUS_ALARM = 2;
const int IMAGE_INDEX_STATUS_UNKNOWN = 3;
const int IMAGE_INDEX_HOST = 4;
const int IMAGE_INDEX_ROUTER = 5;
const int IMAGE_INDEX_SUBNET = 6;
const int IMAGE_INDEX_UNREACH = 7;


/////////////////////////////////////////////////////////////////////////////
// CIdleVisitor

//virtual
unsigned long CIdleVisitor::Visit(CTreeCtrl &TreeCtrl, HTREEITEM hItem)
{
//	CString ItemText = TreeCtrl.GetItemText(hItem);
//	TRACE("CIdleVisitor::Visit - %s\n", ItemText);
	CNetObject *pObject = (CNetObject *)TreeCtrl.GetItemData(hItem);
	if (!pObject)
		return 0;
	
	TVITEM TVItem;
	TVItem.mask = TVIF_HANDLE;
	TVItem.hItem = hItem;
	TreeCtrl.GetItem(&TVItem);

	TVItem.mask = TVIF_STATE;
	TVItem.state &= 0xFFFFF0FF; // гасим старые флаги overlay image
	switch (pObject->GetGeneralStatusCode())
	{
	case STATUS_OK:
		TVItem.state |= INDEXTOOVERLAYMASK(IMAGE_INDEX_STATUS_OK); // устанавливаем новые флаги overlay image
		break;
	case STATUS_WARNING:
		TVItem.state |= INDEXTOOVERLAYMASK(IMAGE_INDEX_STATUS_WARNING); // устанавливаем новые флаги overlay image
		break;
	case STATUS_ALARM:
		TVItem.state |= INDEXTOOVERLAYMASK(IMAGE_INDEX_STATUS_ALARM); // устанавливаем новые флаги overlay image
		break;
	case STATUS_UNKNOWN:
		TVItem.state |= INDEXTOOVERLAYMASK(IMAGE_INDEX_STATUS_UNKNOWN); // устанавливаем новые флаги overlay image
		break;
	}
	TVItem.stateMask |=TVIS_OVERLAYMASK;
	TreeCtrl.SetItem(&TVItem);
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerView

IMPLEMENT_DYNCREATE(CNetworkAnalyzerView, CTreeView)

BEGIN_MESSAGE_MAP(CNetworkAnalyzerView, CTreeView)
	//{{AFX_MSG_MAP(CNetworkAnalyzerView)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerView construction/destruction

CNetworkAnalyzerView::CNetworkAnalyzerView()
{
	m_uLastOnIdleProcessedTime = portableGetCurrentTimeMsec();
}

CNetworkAnalyzerView::~CNetworkAnalyzerView()
{
}

BOOL CNetworkAnalyzerView::PreCreateWindow(CREATESTRUCT &cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerView drawing

void CNetworkAnalyzerView::OnDraw(CDC *pDC)
{
	CNetworkAnalyzerDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerView printing

BOOL CNetworkAnalyzerView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CNetworkAnalyzerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CNetworkAnalyzerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


HTREEITEM CNetworkAnalyzerView::InsertNetObject(HTREEITEM hRootItem, CNetObject *pObject, int nImageIndex)
{
	// find place for insertion based on names
	HTREEITEM NextItem, hNewItem;
	CString NewObjectName;
	NewObjectName.Format("%s/%s %s", pObject->GetIpAddressString(), pObject->GetSubnetMaskString(), pObject->GetName());
	HTREEITEM InsertAfterItem = FindItemInsertAfter(hRootItem, pObject, NextItem);
	if (InsertAfterItem)
	{
		hNewItem = GetTreeCtrl().InsertItem(NewObjectName, nImageIndex, nImageIndex, hRootItem, InsertAfterItem);
	}
	else if (NextItem)
	{
		hNewItem = GetTreeCtrl().InsertItem(NewObjectName, nImageIndex, nImageIndex, hRootItem, TVI_FIRST);
	}
	else
	{
		hNewItem = GetTreeCtrl().InsertItem(NewObjectName, nImageIndex, nImageIndex, hRootItem, TVI_LAST);
	}
	GetTreeCtrl().SetItemData(hNewItem, (DWORD)pObject);
	return hNewItem;
}

CNetObject *CNetworkAnalyzerView::GetNetObject(HTREEITEM hItem)
{
	if (!hItem)
		return NULL;
	CNetObject *pObject = (CNetObject *)GetTreeCtrl().GetItemData(hItem);
	return pObject;
}

void CNetworkAnalyzerView::NewHostCreated(CHost *pHost)
{
	TRACE("DEBUG %s\n", pHost->GetIpAddressString());
	if (strcmp(pHost->GetIpAddressString(), "192.168.31.128") == 0 ||
		strcmp(pHost->GetIpAddressString(), "192.168.31.122") == 0 ||
		strcmp(pHost->GetIpAddressString(), "192.168.31.159") == 0 ||
		strcmp(pHost->GetIpAddressString(), "192.168.31.82") == 0 )
		pHost = pHost;//debug
	HTREEITEM hNewItem = InsertNetObject(TVI_ROOT, pHost, IMAGE_INDEX_HOST);
}

HTREEITEM CNetworkAnalyzerView::InsertHost(HTREEITEM hSubnetItem, CHost *pHost)
{
	HTREEITEM hNewItem = InsertNetObject(hSubnetItem, pHost, IMAGE_INDEX_HOST);
	return hNewItem;
}

void CNetworkAnalyzerView::HostChanged(CHost *pHost)
{
	HTREEITEM hItem = Find(pHost);
	if (!hItem)
		return;

	GetTreeCtrl().SetRedraw(FALSE);
	// Update name
	char *pszName = pHost->GetGivenName();
	if (pszName[0] == 0)
		pszName = pHost->GetName();
	CString str;
	str.Format("%s/%s %s", pHost->GetIpAddressString(), pHost->GetSubnetMaskString(), pszName);
	if (strcmp(pHost->GetIpAddressString(), "192.168.31.128") == 0 ||
		strcmp(pHost->GetIpAddressString(), "192.168.31.122") == 0 ||
		strcmp(pHost->GetIpAddressString(), "192.168.31.159") == 0 ||
		strcmp(pHost->GetIpAddressString(), "192.168.31.82") == 0 )
		pHost = pHost;//debug
	GetTreeCtrl().SetItemText(hItem, str);

	int nCurrentImage, nCurrentImageSelected;
	GetTreeCtrl().GetItemImage(hItem, nCurrentImage, nCurrentImageSelected);
	int nDesiredImage = IMAGE_INDEX_HOST;
	// Is it router?
	if (pHost->IsUnreach())
		nDesiredImage = IMAGE_INDEX_UNREACH;
	else if (pHost->IsRouter())
		nDesiredImage = IMAGE_INDEX_ROUTER;
	if (nCurrentImage != nDesiredImage)
		GetTreeCtrl().SetItemImage(hItem, nDesiredImage, nDesiredImage);

	GetTreeCtrl().SetRedraw(TRUE);
}

void CNetworkAnalyzerView::NewSubnetCreated(CSubnet *pSubnet)
{
	HTREEITEM hNewItem = InsertNetObject(TVI_ROOT, pSubnet, IMAGE_INDEX_SUBNET);
}

void CNetworkAnalyzerView::SubnetChanged(CSubnet *pSubnet)
{
	HTREEITEM hItem = Find(pSubnet);
	if (!hItem)
		return;

	GetTreeCtrl().SetRedraw(FALSE);
	// Update name
	CString str;
	str.Format("%s/%s %s", pSubnet->GetIpAddressString(), pSubnet->GetSubnetMaskString(), pSubnet->GetGivenName());
	GetTreeCtrl().SetItemText(hItem, str);

	GetTreeCtrl().SetRedraw(TRUE);
}

void CNetworkAnalyzerView::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (strcmp(pHost->GetIpAddressString(), "192.168.31.128") == 0 ||
		strcmp(pHost->GetIpAddressString(), "192.168.31.122") == 0 ||
		strcmp(pHost->GetIpAddressString(), "192.168.31.159") == 0 ||
		strcmp(pHost->GetIpAddressString(), "192.168.31.82") == 0 )
		pHost = pHost;//debug
	HTREEITEM hSubnetItem = Find(pSubnet);
	ASSERT(hSubnetItem);
	if (!hSubnetItem)
		return;

	GetTreeCtrl().SetRedraw(FALSE);
	HTREEITEM hHostItem = Find(pHost);
	if (hHostItem)
	{
		if (GetTreeCtrl().GetParentItem(hHostItem) == hSubnetItem)
			return; // already in this subnet
		GetTreeCtrl().DeleteItem(hHostItem);
	}
	InsertHost(hSubnetItem, pHost);
	GetTreeCtrl().SetRedraw(TRUE);
}

void CNetworkAnalyzerView::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
}

void CNetworkAnalyzerView::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	if (pWhereChanged == this)
		return;
	HTREEITEM hItem = Find(pObject);
	if (!hItem)
		return;
	GetTreeCtrl().Select(hItem, TVGN_CARET);
	GetTreeCtrl().EnsureVisible(hItem);
}

void CNetworkAnalyzerView::VisitAllItems(CItemVisitorAPI *pVisitor, HTREEITEM hParentItem)
{
	HTREEITEM hItem = GetTreeCtrl().GetNextItem(hParentItem, TVGN_CHILD);
	while (hItem)
	{
		pVisitor->Visit(GetTreeCtrl(), hItem);
		VisitAllItems(pVisitor, hItem);
		hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
	}
}

BOOL CNetworkAnalyzerView::OnIdleProcess()
{
	unsigned long uCurrentTime = portableGetCurrentTimeMsec();
	if (uCurrentTime - m_uLastOnIdleProcessedTime > 1000)
	{
		CIdleVisitor v;
		VisitAllItems(&v, TVGN_ROOT);
		m_uLastOnIdleProcessedTime = uCurrentTime;
	}

	return FALSE;
}

HTREEITEM CNetworkAnalyzerView::FindItemInsertAfter(HTREEITEM hRootItem, CNetObject *pObject, HTREEITEM &hNextItem)
{
	HTREEITEM hPrevItem = NULL, hItem = GetTreeCtrl().GetNextItem(hRootItem, TVGN_CHILD);
	while (hItem)
	{
		CNetObject *pExistingObject = (CNetObject *)GetTreeCtrl().GetItemData(hItem);
		if (pExistingObject->CompareIpAddress(pObject) > 0)
		{
			hNextItem = hItem;
			return hPrevItem;
		}
		hPrevItem = hItem;
		hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
	}
	hNextItem = NULL;
	return NULL;
}

HTREEITEM CNetworkAnalyzerView::Find(CNetObject *pObject)
{
	HTREEITEM hItem = GetTreeCtrl().GetNextItem(NULL, TVGN_CHILD);
	while (hItem)
	{
		HTREEITEM hFound = FindIn(pObject, hItem);
		if (hFound)
			return hFound;
		hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
	}

	return NULL;
}

HTREEITEM CNetworkAnalyzerView::FindIn(CNetObject *pObject, HTREEITEM hItemLookIn)
{
	if (pObject == (CNetObject *)GetTreeCtrl().GetItemData(hItemLookIn))
		return hItemLookIn;
	HTREEITEM hItem = GetTreeCtrl().GetNextItem(hItemLookIn, TVGN_CHILD);
	while (hItem)
	{
		HTREEITEM hFound = FindIn(pObject, hItem);
		if (hFound)
			return hFound;
		hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerView diagnostics

#ifdef _DEBUG
void CNetworkAnalyzerView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CNetworkAnalyzerView::Dump(CDumpContext &dc) const
{
	CTreeView::Dump(dc);
}

CNetworkAnalyzerDoc *CNetworkAnalyzerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNetworkAnalyzerDoc)));
	return (CNetworkAnalyzerDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerView message handlers

void CNetworkAnalyzerView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();

	m_ImageList.Create(IDB_BITMAP_TREE,16,5,RGB(192,192,192));
	GetTreeCtrl().SetImageList(&m_ImageList, TVSIL_NORMAL);
	m_ImageList.SetOverlayImage(0, 1);
	m_ImageList.SetOverlayImage(1, 2);
	m_ImageList.SetOverlayImage(2, 3);
	m_ImageList.SetOverlayImage(3, 4);

	long lStyleOld;
	lStyleOld = GetWindowLong(GetTreeCtrl().m_hWnd, GWL_STYLE);
	lStyleOld |= (TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
}

void CNetworkAnalyzerView::OnContextMenu(CWnd *pWnd, CPoint point) 
{
	CPoint ClientPoint(point);
	ScreenToClient(&ClientPoint);
	UINT uFlags = 0;
	HTREEITEM hItem = GetTreeCtrl().HitTest(ClientPoint, &uFlags);
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		CNetObject *pObject = GetNetObject(hItem);
		if (pObject)
			pObject->GuiContextMenu(point.x, point.y);
	}
}

void CNetworkAnalyzerView::OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult) 
{
	NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;
	HTREEITEM hSelectedItem = pNMTreeView->itemNew.hItem;
	if (hSelectedItem)
	{
		CNetObject *pObject = (CNetObject *)GetTreeCtrl().GetItemData(hSelectedItem);
		if (pObject)
		{
			if (m_pGui)
			{
				m_pGui->ObjectSelectionChanged(pObject, this);
			}
		}
	}
	*pResult = 0;
}
