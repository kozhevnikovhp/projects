/* NetworkDocAPI.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	07 Sep 2006 - initial creation
	09 Dec 2007 - doc templates: bringing existing docs of the same netobject to top, etc.
	10 Dec 2007 - CNetworkTreeView class (base class for all tree views)
	15 Jan 2008 - splitting MFC- and NonMFC-visitors to avoid multithread problems with MFC-controls 
	13 Mar 2008 - HostChanged event handling; asking user to save/discard/continue editing of modified data at tree-ctrl selection changing
	20 Mar 2008 - moving host between groups event handling
	21 Mar 2008 - NetworkTreeView: visit all items functionality
	28 Aug 2008 - Windows messages instead of method calls for more safe multythreading
	01 Sep 2008 - Window captions reflects host/subnet name and document type
*/

#include "stdafx.h"
#include "NetObserver.h"
#include "NetworkDocAPI.h"
#include "NetworkFrameAPI.h"


/////////////////////////////////////////////////////////////////////////////
// COnIdleMFCVisitor

COnIdleMFCVisitor::COnIdleMFCVisitor()
{
}

//virtual
COnIdleMFCVisitor::~COnIdleMFCVisitor()
{
}

//virtual
void COnIdleMFCVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->OnIdleMFC();
}


/////////////////////////////////////////////////////////////////////////////
// COnIdleNonMFCVisitor

COnIdleNonMFCVisitor::COnIdleNonMFCVisitor()
{
}

//virtual
COnIdleNonMFCVisitor::~COnIdleNonMFCVisitor()
{
}

//virtual
void COnIdleNonMFCVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->OnIdleNonMFCThread();
}


/////////////////////////////////////////////////////////////////////////////
// CNewHostCreatedVisitor

CNewHostCreatedVisitor::CNewHostCreatedVisitor(CHost *pHost)
{
	m_pHost = pHost;
}

//virtual
CNewHostCreatedVisitor::~CNewHostCreatedVisitor()
{
}

//virtual
void CNewHostCreatedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->NewHostCreated(m_pHost);
}


/////////////////////////////////////////////////////////////////////////////
// CHostChangedVisitor

CHostChangedVisitor::CHostChangedVisitor(CHost *pHost)
{
	m_pHost = pHost;
}

//virtual
CHostChangedVisitor::~CHostChangedVisitor()
{
}

//virtual
void CHostChangedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->HostChanged(m_pHost);
}


/////////////////////////////////////////////////////////////////////////////
// CNewSubnetCreatedVisitor

CNewSubnetCreatedVisitor::CNewSubnetCreatedVisitor(CSubnet *pSubnet)
{
	m_pSubnet = pSubnet;
}

//virtual
CNewSubnetCreatedVisitor::~CNewSubnetCreatedVisitor()
{
}

//virtual
void CNewSubnetCreatedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->NewSubnetCreated(m_pSubnet);
}


/////////////////////////////////////////////////////////////////////////////
// CSubnetChangedVisitor

CSubnetChangedVisitor::CSubnetChangedVisitor(CSubnet *pSubnet)
{
	m_pSubnet = pSubnet;
}

//virtual
CSubnetChangedVisitor::~CSubnetChangedVisitor()
{
}

//virtual
void CSubnetChangedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->SubnetChanged(m_pSubnet);
}


/////////////////////////////////////////////////////////////////////////////
// CHostAddedToSubnetVisitor

CHostAddedToSubnetVisitor::CHostAddedToSubnetVisitor(CHost *pHost, CSubnet *pSubnet)
{
	m_pHost = pHost;
	m_pSubnet = pSubnet;
}

//virtual
CHostAddedToSubnetVisitor::~CHostAddedToSubnetVisitor()
{
}

//virtual
void CHostAddedToSubnetVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->HostAddedToSubnet(m_pHost, m_pSubnet);
}


/////////////////////////////////////////////////////////////////////////////
// CHostRemovedFromSubnetVisitor

CHostRemovedFromSubnetVisitor::CHostRemovedFromSubnetVisitor(CHost *pHost, CSubnet *pSubnet)
{
	m_pHost = pHost;
	m_pSubnet = pSubnet;
}

//virtual
CHostRemovedFromSubnetVisitor::~CHostRemovedFromSubnetVisitor()
{
}

//virtual
void CHostRemovedFromSubnetVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->HostRemovedFromSubnet(m_pHost, m_pSubnet);
}


/////////////////////////////////////////////////////////////////////////////
// CNewGroupCreatedVisitor

CNewGroupCreatedVisitor::CNewGroupCreatedVisitor(CGroup *pGroup)
{
	m_pGroup = pGroup;
}

//virtual
CNewGroupCreatedVisitor::~CNewGroupCreatedVisitor()
{
}

//virtual
void CNewGroupCreatedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->NewGroupCreated(m_pGroup);
}


/////////////////////////////////////////////////////////////////////////////
// CGroupChangedVisitor

CGroupChangedVisitor::CGroupChangedVisitor(CGroup *pGroup)
{
	m_pGroup = pGroup;
}

//virtual
CGroupChangedVisitor::~CGroupChangedVisitor()
{
}

//virtual
void CGroupChangedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->GroupChanged(m_pGroup);
}


/////////////////////////////////////////////////////////////////////////////
// CHostMovedBetweenGroupsVisitor

CHostMovedBetweenGroupsVisitor::CHostMovedBetweenGroupsVisitor(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup)
{
	m_pHost = pHost;
	m_pOldGroup = pOldGroup;
	m_pNewGroup = pNewGroup;
}

//virtual
CHostMovedBetweenGroupsVisitor::~CHostMovedBetweenGroupsVisitor()
{
}

//virtual
void CHostMovedBetweenGroupsVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->HostMovedBetweenGroups(m_pHost, m_pOldGroup, m_pNewGroup);
}


/////////////////////////////////////////////////////////////////////////////
// CObjectSelectionChangedVisitor

CObjectSelectionChangedVisitor::CObjectSelectionChangedVisitor(CNetObject *pObject, void *pWhereChanged)
{
	m_pObject = pObject;
	m_pWhereChanged = pWhereChanged;
}

//virtual
CObjectSelectionChangedVisitor::~CObjectSelectionChangedVisitor()
{
}

//virtual
void CObjectSelectionChangedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->ObjectSelectionChanged(m_pObject, m_pWhereChanged);
}


/////////////////////////////////////////////////////////////////////////////
// CUpdateGuiVisitor

CUpdateGuiVisitor::CUpdateGuiVisitor()
{
}

//virtual
CUpdateGuiVisitor::~CUpdateGuiVisitor()
{
}

//virtual
void CUpdateGuiVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->UpdateAllViews(NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CNetObjectIgnoredVisitor

CNetObjectIgnoredVisitor::CNetObjectIgnoredVisitor(CNetObject *pObject)
{
	m_pObject = pObject;
}

//virtual
CNetObjectIgnoredVisitor::~CNetObjectIgnoredVisitor()
{
}

//virtual
void CNetObjectIgnoredVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->NetObjectIgnored(m_pObject);
}


/////////////////////////////////////////////////////////////////////////////
// CNetObjectHiddenVisitor

CNetObjectHiddenVisitor::CNetObjectHiddenVisitor(CNetObject *pObject)
{
	m_pObject = pObject;
}

//virtual
CNetObjectHiddenVisitor::~CNetObjectHiddenVisitor()
{
}

//virtual
void CNetObjectHiddenVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->NetObjectHidden(m_pObject);
}


/////////////////////////////////////////////////////////////////////////////
// CShowHiddenObjectsVisitor

CShowHiddenObjectsVisitor::CShowHiddenObjectsVisitor()
{
}

//virtual
CShowHiddenObjectsVisitor::~CShowHiddenObjectsVisitor()
{
}

//virtual
void CShowHiddenObjectsVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->ShowHiddenObjects();
}


/////////////////////////////////////////////////////////////////////////////
// CShowIgnoredObjectsVisitor

CShowIgnoredObjectsVisitor::CShowIgnoredObjectsVisitor()
{
}

//virtual
CShowIgnoredObjectsVisitor::~CShowIgnoredObjectsVisitor()
{
}

//virtual
void CShowIgnoredObjectsVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->ShowIgnoredObjects();
}


/////////////////////////////////////////////////////////////////////////////
// CNetworkDocAPI

IMPLEMENT_DYNCREATE(CNetworkDocAPI, CDocument)

BEGIN_MESSAGE_MAP(CNetworkDocAPI, CDocument)
	//{{AFX_MSG_MAP(CNetworkDocAPI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkDocAPI construction/destruction

CNetworkDocAPI::CNetworkDocAPI()
{
	m_pFrame = NULL;
	m_pNetObject = NULL;
}

CNetworkDocAPI::~CNetworkDocAPI()
{
}

void CNetworkDocAPI::BringToTop()
{
	int nCmdShow = SW_SHOW;
	if (m_pFrame->IsIconic())
		nCmdShow = SW_RESTORE;
	m_pFrame->ActivateFrame(nCmdShow);
}

//virtual
void CNetworkDocAPI::UpdateWindowCaption()
{
	CString CaptionString;
	if (m_pNetObject)
	{
		char szObjectFullName[512]; szObjectFullName[0] = 0;
		m_pNetObject->GetFullName(szObjectFullName);
		CaptionString.Format("%s - %s", szObjectFullName, GetDocTypeDescription());
	}
	else
		CaptionString = GetDocTypeDescription();

	SetTitle(CaptionString);
}

void CNetworkDocAPI::CreateForm(int IndexInViewArray, CView *pViewToAdd, CWnd *pParent)
{
	pViewToAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CFrameWnd::rectDefault, pParent,
		AFX_IDW_PANE_FIRST+IndexInViewArray+16, NULL);
	AddView(pViewToAdd);
}

void CNetworkDocAPI::NewHostCreated(CHost *pHost)
{
	if (m_pFrame)
		m_pFrame->SendMessage(WM_HOST_CREATED, 0, (LPARAM)pHost);
}

void CNetworkDocAPI::HostChanged(CHost *pHost)
{
	if (m_pFrame && IsMine(pHost))
		m_pFrame->SendMessage(WM_HOST_CHANGED, 0, (LPARAM)pHost);
}

void CNetworkDocAPI::NewSubnetCreated(CSubnet *pSubnet)
{
	if (m_pFrame)
		m_pFrame->SendMessage(WM_SUBNET_CREATED, 0, (LPARAM)pSubnet);
}

void CNetworkDocAPI::SubnetChanged(CSubnet *pSubnet)
{
	if (m_pFrame && IsMine(pSubnet))
		m_pFrame->SendMessage(WM_SUBNET_CHANGED, 0, (LPARAM)pSubnet);
}

void CNetworkDocAPI::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (m_pFrame)
		m_pFrame->SendMessage(WM_HOST_ADDED_TO_SUBNET, (WPARAM)pHost, (LPARAM)pSubnet);
}

void CNetworkDocAPI::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (m_pFrame)
		m_pFrame->SendMessage(WM_HOST_REMOVED_FROM_SUBNET, (WPARAM)pHost, (LPARAM)pSubnet);
}

void CNetworkDocAPI::NewGroupCreated(CGroup *pGroup)
{
	if (m_pFrame)
		m_pFrame->NewGroupCreated(pGroup);
}

void CNetworkDocAPI::GroupChanged(CGroup *pGroup)
{
	if (m_pFrame)
		m_pFrame->GroupChanged(pGroup);
}

void CNetworkDocAPI::HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup)
{
	if (m_pFrame)
		m_pFrame->HostMovedBetweenGroups(pHost, pOldGroup, pNewGroup);
}

void CNetworkDocAPI::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	if (m_pFrame)
		m_pFrame->ObjectSelectionChanged(pObject, pWhereChanged);
}

LONG CNetworkDocAPI::OnIdleMFC()
{
	if (m_pFrame)
		return m_pFrame->OnIdleMFC();
	return 0;
}

LONG CNetworkDocAPI::OnIdleNonMFCThread()
{
	if (m_pFrame)
		return m_pFrame->OnIdleNonMFCThread();
	return 0;
}

void CNetworkDocAPI::NetObjectIgnored(CNetObject *pObject)
{
	if (m_pFrame)
		m_pFrame->NetObjectIgnored(pObject);
}

void CNetworkDocAPI::NetObjectHidden(CNetObject *pObject)
{
	if (m_pFrame)
		m_pFrame->NetObjectHidden(pObject);
}

void CNetworkDocAPI::ShowHiddenObjects()
{
	if (m_pFrame)
		m_pFrame->ShowHiddenObjects();
}

void CNetworkDocAPI::ShowIgnoredObjects()
{
	if (m_pFrame)
		m_pFrame->ShowIgnoredObjects();
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkDocAPI diagnostics

#ifdef _DEBUG
void CNetworkDocAPI::AssertValid() const
{
	CDocument::AssertValid();
}

void CNetworkDocAPI::Dump(CDumpContext &dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CNetworkDocAPI commands


/////////////////////////////////////////////////////////////////////////////
// CNetworkDocTemplate


CNetworkDocTemplate::CNetworkDocTemplate(UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass * pViewClass)
	: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

//virtual 
void CNetworkDocTemplate::InitialUpdateFrame(CFrameWnd *pFrame, CDocument *pDoc, BOOL bMakeVisible)
{
	CMultiDocTemplate::InitialUpdateFrame(pFrame, pDoc, bMakeVisible);
	CNetworkFrameAPI *pFrameAPI = (CNetworkFrameAPI *)pFrame;
	CNetworkDocAPI *pDocAPI = (CNetworkDocAPI *)pDoc;
	pFrameAPI->SetDocument(pDocAPI);
}

CNetworkDocAPI *CNetworkDocTemplate::FindDocument(CNetObject *pObject)
{
	CNetworkDocAPI *pDoc = NULL;
	for (POSITION DocPos = GetFirstDocPosition(); DocPos;)
	{
		pDoc = (CNetworkDocAPI *)GetNextDoc(DocPos);
		if (pDoc->IsMine(pObject))
		{
			return pDoc;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CHostDocTemplate

CHostDocTemplate::CHostDocTemplate(UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass * pViewClass)
	: CNetworkDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

CNetworkDocAPI *CHostDocTemplate::CreateNewWindow(CHost *pHost)
{
	CNetworkDocAPI *pDoc = FindDocument(pHost);
	if (!pDoc)
	{
		pDoc = (CNetworkDocAPI *)OpenDocumentFile(NULL);
		if (pDoc)
		{
			pDoc->Initialize(pHost);
			pDoc->UpdateWindowCaption();
		}
	}
	pDoc->BringToTop();
	return pDoc;
}

/////////////////////////////////////////////////////////////////////////////
// CSubnetDocTemplate

CSubnetDocTemplate::CSubnetDocTemplate(UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass * pViewClass)
	: CNetworkDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

CNetworkDocAPI *CSubnetDocTemplate::CreateNewWindow(CSubnet *pSubnet)
{
	CNetworkDocAPI *pDoc = FindDocument(pSubnet);
	if (!pDoc)
	{
		pDoc = (CNetworkDocAPI *)OpenDocumentFile(NULL);
		if (pDoc)
		{
			pDoc->Initialize(pSubnet);
			pDoc->UpdateWindowCaption();
		}
	}
	pDoc->BringToTop();
	return pDoc;
}

/////////////////////////////////////////////////////////////////////////////
// CAttachedBlock

CAttachedBlock::CAttachedBlock(long ViewIndex, void *pObject)
{
	m_ViewIndex = ViewIndex;
	m_pObject = pObject;
}

CAttachedBlock::~CAttachedBlock()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetTreeView

IMPLEMENT_DYNCREATE(CNetworkTreeView, CTreeView)

CNetworkTreeView::CNetworkTreeView()
{
	m_hPrevSelection = NULL;
	m_pCurrentlyVisibleForm = NULL;
	m_pFrame = NULL;
}

CNetworkTreeView::~CNetworkTreeView()
{
}

BEGIN_MESSAGE_MAP(CNetworkTreeView, CTreeView)
	//{{AFX_MSG_MAP(CNetworkTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HTREEITEM CNetworkTreeView::NewItem(char *pszText, int nImage, long nFormIndex, void *pObject, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = GetTreeCtrl().InsertItem(pszText, nImage, nImage, hParent, hInsertAfter);
	CAttachedBlock *pAttached = new CAttachedBlock(nFormIndex, pObject);
	GetTreeCtrl().SetItemData(hItem, (DWORD)pAttached);
	return hItem;
}

void CNetworkTreeView::VisitAllItems(CNetworkTreeItemVisitor *pVisitor)
{
	VisitChildItems(pVisitor, TVI_ROOT);
}

LOGICAL CNetworkTreeView::VisitChildItems(CNetworkTreeItemVisitor *pVisitor, HTREEITEM hItem)
{
	if (VisitItem(pVisitor, hItem))
		return LOGICAL_TRUE; // processed, no further processing required
	HTREEITEM hNextItem, hChildItem = GetTreeCtrl().GetChildItem(hItem);

	while (hChildItem != NULL)
	{
		hNextItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
		if (VisitChildItems(pVisitor, hChildItem))
			return LOGICAL_TRUE; // processed, no further processing required
		hChildItem = hNextItem;
	}
	return LOGICAL_FALSE; // not processed, further processing required
}

LOGICAL CNetworkTreeView::VisitItem(CNetworkTreeItemVisitor *pVisitor, HTREEITEM hItem)
{
	if (hItem == TVI_ROOT)
		return LOGICAL_FALSE; // not processed
	return pVisitor->Visit(hItem);
}

void CNetworkTreeView::SwitchToDefaultForm(CNetworkForm *pOldForm)
{
	UINT OldIDD = pOldForm->GetDlgCtrlID();
	m_pCurrentlyVisibleForm = GetDefaultForm();
	UINT NewIDD = m_pCurrentlyVisibleForm->GetDlgCtrlID();
	pOldForm->ShowWindow(SW_HIDE);
	m_pCurrentlyVisibleForm->ShowWindow(SW_SHOW);
	pOldForm->SetDlgCtrlID(NewIDD);
	m_pCurrentlyVisibleForm->SetDlgCtrlID(OldIDD);
	if (m_hPrevSelection)
		pOldForm->Leaving();
	m_pFrame->Refresh();
}

void CNetworkTreeView::RefreshCurrentForm()
{
	CWaitCursor cursor;
	if (m_pCurrentlyVisibleForm)
		m_pCurrentlyVisibleForm->Entering();
}

/////////////////////////////////////////////////////////////////////////////
// CNetTreeView drawing

void CNetworkTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CNetTreeView diagnostics

#ifdef _DEBUG
void CNetworkTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CNetworkTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNetTreeView message handlers

void CNetworkTreeView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();

	m_ImageList.Create(IDB_BITMAP_TREE, 16, 5, RGB(192,192,192));
	GetTreeCtrl().SetImageList(&m_ImageList, TVSIL_NORMAL);
	m_ImageList.SetOverlayImage(0, 1);
	m_ImageList.SetOverlayImage(1, 2);
	m_ImageList.SetOverlayImage(2, 3);
	m_ImageList.SetOverlayImage(3, 4);

	long lStyleOld;
	lStyleOld = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyleOld |= (TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
}

void CNetworkTreeView::OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult) 
{
	CWaitCursor cursor;

	CNetworkForm *pOldForm = m_pCurrentlyVisibleForm;
	if (m_pFrame && pOldForm)
	{
		m_pCurrentlyVisibleForm = NULL;
		HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
		if (!hItem)
		{
			SwitchToDefaultForm(pOldForm);
			return;
		}
		CAttachedBlock *pAttached = (CAttachedBlock *)GetTreeCtrl().GetItemData(hItem);
		int nNewViewIndex = pAttached->m_ViewIndex;

		CNetworkForm *pNewForm = GetForm(nNewViewIndex);

		UINT OldIDD = pOldForm->GetDlgCtrlID();
		UINT NewIDD = 0;
		if (pNewForm)
			NewIDD = pNewForm->GetDlgCtrlID();
		if (OldIDD == NewIDD && hItem == m_hPrevSelection)
			return;
		if (m_hPrevSelection != NULL)
		{
			LOGICAL bGoForward = pOldForm->CanLeave();
			if (bGoForward)
			{
				if (pOldForm->IsModified())
					bGoForward = pOldForm->Leaving();
			}
			
			if (!bGoForward)
			{
				GetTreeCtrl().SelectItem(m_hPrevSelection);
				m_pCurrentlyVisibleForm = pOldForm;
				return;
			}
		}

		if (pNewForm)
		{
			pNewForm->SetAttachedBlock(pAttached);
			if (pNewForm->Entering())
			{
				pNewForm->ClearModifiedFlag();
				pNewForm->UpdateStatus();
				m_hPrevSelection = hItem;
				m_pCurrentlyVisibleForm = pNewForm;
				if (OldIDD != NewIDD)
				{
					pOldForm->ShowWindow(SW_HIDE);
					pNewForm->ShowWindow(SW_SHOW);
					pOldForm->SetDlgCtrlID(NewIDD);
					pNewForm->SetDlgCtrlID(OldIDD);
					m_pFrame->Refresh();
				}
			}
			else
			{
				MessageBeep(0);
				GetTreeCtrl().SelectItem(m_hPrevSelection);
			}
		}
		else
			SwitchToDefaultForm(pOldForm);

	}
	*pResult = 0;
}

void CNetworkTreeView::OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult) 
{
	NM_TREEVIEW * pNMTreeView = (NM_TREEVIEW *)pNMHDR;

	CAttachedBlock *pAttached = (CAttachedBlock *)pNMTreeView->itemOld.lParam;
	delete pAttached;
	
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkForm

IMPLEMENT_DYNCREATE(CNetworkForm, CFormView)

CNetworkForm::CNetworkForm() : CFormView(IDD_NETWORK_FORM)
{
	//{{AFX_DATA_INIT(CNetworkForm)
	//}}AFX_DATA_INIT
	m_bModified = LOGICAL_FALSE;
	m_pAttached = NULL;
	m_LastUpdateTime = portableGetCurrentTimeMsec();
}

CNetworkForm::CNetworkForm(UINT nIDTemplate)  : CFormView(nIDTemplate)
{
	m_bModified = LOGICAL_FALSE;
	m_pAttached = NULL;
	m_LastUpdateTime = portableGetCurrentTimeMsec();
}

CNetworkForm::~CNetworkForm()
{
}

//virtual
LOGICAL CNetworkForm::CanLeave()
{
	if (!IsModified())
		return LOGICAL_TRUE;
	
	int ID = MessageBox("Do you want to save the changes (press YES), or discard them (press NO), or continue editing (press CANCEL)?",
		"Attention", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (ID)
	{
	case IDYES:
		return LOGICAL_TRUE;
		break;
	case IDNO:
		ClearModifiedFlag(); // prevent from saving
		return LOGICAL_TRUE;
		break;
	case IDCANCEL:
		return LOGICAL_FALSE;
		break;
	}
	
	return LOGICAL_FALSE;
}

void CNetworkForm::UpdateStatus()
{
	unsigned long current_time = portableGetCurrentTimeMsec();
	long delta_time = current_time - m_LastUpdateTime;
	long period = GetUpdateStatusPeriod();
	if (delta_time > period)
	{
		OnUpdateStatus();
		m_LastUpdateTime = current_time;
	}
}

void CNetworkForm::DoDataExchange(CDataExchange *pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkForm, CFormView)
	//{{AFX_MSG_MAP(CNetworkForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkForm diagnostics

#ifdef _DEBUG
void CNetworkForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CNetworkForm::Dump(CDumpContext &dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNetworkForm message handlers

