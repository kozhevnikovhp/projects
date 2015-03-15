/* GroupEditor.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	10 Dec 2007 - initial creation
	25 Dec 2007 - form switching from tree view
	20 Feb 2008 - group creation/deletion
	14 Mar 2008 - storing group params in DB, tree-view update while group name is changed
	20 Mar 2008 - ping group creation, event handling
	21 Mar 2008 - default groups are dynamically allocated as ordinary ones; TreeView: visit all items functionality
	06 Jun 2008 - appearance groups introduced (not completed)
	08 Jul 2008 - color schemes for appearance groups
	26 Aug 2008 - SNMP groups
*/

#include "stdafx.h"
#include "netobserver.h"
#include "GroupEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorDoc

IMPLEMENT_DYNCREATE(CGroupEditorDoc, CNetworkDocAPI)

CGroupEditorDoc::CGroupEditorDoc()
{
	memset(m_pForms, 0, sizeof(m_pForms));
}

BOOL CGroupEditorDoc::OnNewDocument()
{
	if (!CNetworkDocAPI::OnNewDocument())
		return FALSE;
	return TRUE;
}

CGroupEditorDoc::~CGroupEditorDoc()
{
}

//virtual
/*void CGroupEditorDoc::Initialize(CHost *pHost)
{
	char szWindowTitle[256]; szWindowTitle[0] = 0;
	sprintf(szWindowTitle, "%s %s - traffic", pHost->GetName(), pHost->GetIpAddressString());
	SetTitle(szWindowTitle);
	CDisplayHostTrafficFrame *pFrame = (CDisplayHostTrafficFrame *)m_pFrame;
	pFrame->Init(pHost);
}*/

void CGroupEditorDoc::CreateForms(CSplitterWnd &Splitter)
{
	m_pForms[DEFAULT_GROUP_INDEX] = (CGroupEditorForm *)Splitter.GetPane(0,1);
	AddForm(PING_GROUP_INDEX, new CPingGroupEditorForm(), &Splitter);
	AddForm(APPEARANCE_GROUP_INDEX, new CAppearanceGroupEditorForm(), &Splitter);
	AddForm(SNMP_GROUP_INDEX, new CSnmpGroupEditorForm(), &Splitter);
}

void CGroupEditorDoc::AddForm(int IndexInViewArray, CGroupEditorForm *pForm, CWnd *pParent)
{
	CreateForm(IndexInViewArray, pForm, pParent);
	m_pForms[IndexInViewArray] = pForm;
}

BEGIN_MESSAGE_MAP(CGroupEditorDoc, CNetworkDocAPI)
	//{{AFX_MSG_MAP(CGroupEditorDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorDoc diagnostics

#ifdef _DEBUG
void CGroupEditorDoc::AssertValid() const
{
	CNetworkDocAPI::AssertValid();
}

void CGroupEditorDoc::Dump(CDumpContext &dc) const
{
	CNetworkDocAPI::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorDoc commands


/////////////////////////////////////////////////////////////////////////////
// CGroupEditorFrame

/*static UINT indicators[] =
{
};*/

IMPLEMENT_DYNCREATE(CGroupEditorFrame, CNetworkFrameAPI)

CGroupEditorFrame::CGroupEditorFrame()
{
	m_pTreeView = NULL;
}

CGroupEditorFrame::~CGroupEditorFrame()
{
}

CGroupEditorForm *CGroupEditorFrame::GetCurrentForm()
{
	CGroupEditorForm *pCurrentForm = (CGroupEditorForm *)m_Splitter.GetPane(0, 1);
	return pCurrentForm;
}

//virtual
void CGroupEditorFrame::NewHostCreated(CHost *pHost)
{
	m_pTreeView->NewHostCreated(pHost);
}

//virtual
void CGroupEditorFrame::NewGroupCreated(CGroup *pGroup)
{
	m_pTreeView->NewGroupCreated(pGroup);
}

//virtual
void CGroupEditorFrame::GroupChanged(CGroup *pGroup)
{
	m_pTreeView->GroupChanged(pGroup);
}

//virtual
void CGroupEditorFrame::HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup)
{
	CGroupEditorForm *pCurrentForm = GetCurrentForm();
	if (pCurrentForm)
		pCurrentForm->HostMovedBetweenGroups(pHost, pOldGroup, pNewGroup);
}

BEGIN_MESSAGE_MAP(CGroupEditorFrame, CNetworkFrameAPI)
	//{{AFX_MSG_MAP(CGroupEditorFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_DELETE_GROUP, OnEditDeleteGroup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE_GROUP, OnUpdateEditDeleteGroup)
	ON_COMMAND(ID_EDIT_CREATE_PINGGROUP, OnEditCreatePingGroup)
	ON_COMMAND(ID_EDIT_CREATE_SNMPGROUP, OnEditCreateSNMPGroup)
	ON_COMMAND(ID_EDIT_CREATE_APPEARANCEGROUP, OnEditCreateAppearanceGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorFrame message handlers

int CGroupEditorFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CNetworkFrameAPI::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	return 0;
}

BOOL CGroupEditorFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) 
{
	// create a splitter with 1 row, 2 columns
	if (!m_Splitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	if (!m_Splitter.CreateView(0, 0,
		RUNTIME_CLASS(CGroupEditorTreeView), CSize(200, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if (!m_Splitter.CreateView(0, 1,
		RUNTIME_CLASS(CGroupEditorForm), CSize(100, 100), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	m_pTreeView = (CGroupEditorTreeView *)m_Splitter.GetPane(0, 0);
	CNetworkForm *pCurrentForm = GetCurrentForm();
	m_pTreeView->SetEnv(this, pCurrentForm);

	CGroupEditorDoc *pDoc = (CGroupEditorDoc *)pContext->m_pCurrentDoc;
	pDoc->CreateForms(m_Splitter);

	return TRUE;
}

void CGroupEditorFrame::OnEditDeleteGroup() 
{
	// TODO: Add your command handler code here
	
}

void CGroupEditorFrame::OnUpdateEditDeleteGroup(CCmdUI *pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}


void CGroupEditorFrame::OnEditCreatePingGroup() 
{
	CreatePingGroup(LOGICAL_FALSE, LOGICAL_TRUE);
}


void CGroupEditorFrame::OnEditCreateAppearanceGroup() 
{
	CreateAppearanceGroup(LOGICAL_FALSE, LOGICAL_TRUE);	
}

void CGroupEditorFrame::OnEditCreateSNMPGroup() 
{
	CreateSnmpGroup(LOGICAL_FALSE, LOGICAL_TRUE);	
}


/////////////////////////////////////////////////////////////////////
// CGroupEditorTreeItemVisitor

CGroupEditorTreeItemVisitor::CGroupEditorTreeItemVisitor(CGroupEditorTreeView *pTreeView, CGroup *pGroup)
{
	m_pTreeView = pTreeView;
	m_pTreeCtrl = &(pTreeView->GetTreeCtrl());
	m_pGroup = pGroup;
}

//virtual
CGroupEditorTreeItemVisitor::~CGroupEditorTreeItemVisitor()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGroupCreatedTreeItemVisitor

CGroupCreatedTreeItemVisitor::CGroupCreatedTreeItemVisitor(CGroupEditorTreeView *pTreeView, CGroup *pGroup)
	: CGroupEditorTreeItemVisitor(pTreeView, pGroup)
{
}

//virtual
CGroupCreatedTreeItemVisitor::~CGroupCreatedTreeItemVisitor()
{
}

//virtual
LOGICAL CGroupCreatedTreeItemVisitor::Visit(HTREEITEM hItem)
{
	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeView->GetTreeCtrl().GetItemData(hItem);
	if (!pAttached)
		return LOGICAL_FALSE; // not processed, further processing required
	CGroup *pGroup = (CGroup *)pAttached->m_pObject;
	if (pGroup && m_pGroup->IsEqualType(pGroup))
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hItem);
		ASSERT(hParentItem);
		m_pTreeView->AddGroup(hParentItem, m_pGroup);
		return LOGICAL_TRUE; // processed, no further processing required
	}
	return LOGICAL_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGroupChangedTreeItemVisitor

CGroupChangedTreeItemVisitor::CGroupChangedTreeItemVisitor(CGroupEditorTreeView *pTreeView, CGroup *pGroup)
	: CGroupEditorTreeItemVisitor(pTreeView, pGroup)
{
}

//virtual
CGroupChangedTreeItemVisitor::~CGroupChangedTreeItemVisitor()
{
}

//virtual
LOGICAL CGroupChangedTreeItemVisitor::Visit(HTREEITEM hItem)
{
	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(hItem);
	if (!pAttached)
		return LOGICAL_FALSE; // not processed, further processing required
	if (pAttached->m_pObject == m_pGroup)
	{
		m_pTreeCtrl->SetItemText(hItem, m_pGroup->GetName());
		return LOGICAL_TRUE; // processed, no further processing required
	}
	return LOGICAL_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGroupEditorTreeView

IMPLEMENT_DYNCREATE(CGroupEditorTreeView, CNetworkTreeView)

CGroupEditorTreeView::CGroupEditorTreeView()
{
}

CGroupEditorTreeView::~CGroupEditorTreeView()
{
}


BEGIN_MESSAGE_MAP(CGroupEditorTreeView, CNetworkTreeView)
	//{{AFX_MSG_MAP(CGroupEditorTreeView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
CNetworkForm *CGroupEditorTreeView::GetForm(int nViewIndex)
{
	CGroupEditorDoc *pDoc = (CGroupEditorDoc *)m_pDocument;
	return pDoc->GetControlForm(nViewIndex);
}

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorTreeView drawing

void CGroupEditorTreeView::OnDraw(CDC *pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorTreeView diagnostics

#ifdef _DEBUG
void CGroupEditorTreeView::AssertValid() const
{
	CNetworkTreeView::AssertValid();
}

void CGroupEditorTreeView::Dump(CDumpContext &dc) const
{
	CNetworkTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorTreeView message handlers

void CGroupEditorTreeView::OnInitialUpdate() 
{
	CNetworkTreeView::OnInitialUpdate();
	HTREEITEM hPingItem = NewItem("Ping groups", 4, DEFAULT_GROUP_INDEX, NULL);
	AddGroups(hPingItem, g_pPingDefaultGroup);
	HTREEITEM hAppearanceItem = NewItem("Appearance groups", 4, DEFAULT_GROUP_INDEX, NULL);
	AddGroups(hAppearanceItem, g_pAppearanceDefaultGroup);
	HTREEITEM hSnmpItem = NewItem("SNMP groups", 4, DEFAULT_GROUP_INDEX, NULL);
	AddGroups(hSnmpItem, g_pSnmpDefaultGroup);
}

void CGroupEditorTreeView::NewHostCreated(CHost *pHost)
{
	CGroupEditorForm *pForm = (CGroupEditorForm *)m_pCurrentlyVisibleForm;
	if (pForm)
	{
		CGroup *pGroup = pForm->GetGroup();
		if (pGroup)
		{
			//if (pGroup->IsDefault())
			//	pForm->AddHostToList(pHost);
		}
	}
}

void CGroupEditorTreeView::AddGroups(HTREEITEM hItem, CGroup *pDefaultGroup)
{
	for (CListOfGroups::iterator p = g_AllGroups.begin(); p != g_AllGroups.end(); p++)
	{
		CGroup *pGroup = *p;
		if (pDefaultGroup->IsEqualType(pGroup))
			AddGroup(hItem, pGroup);
	}
}

void CGroupEditorTreeView::AddGroup(HTREEITEM hItem, CGroup *pGroup)
{
	NewItem(pGroup->GetName(), 4, pGroup->GetType(), pGroup, hItem);
}

void CGroupEditorTreeView::NewGroupCreated(CGroup *pGroup)
{
	CGroupCreatedTreeItemVisitor visitor(this, pGroup);
	VisitAllItems(&visitor);
}

void CGroupEditorTreeView::GroupChanged(CGroup *pGroup)
{
	CGroupChangedTreeItemVisitor visitor(this, pGroup);
	VisitAllItems(&visitor);
}


/////////////////////////////////////////////////////////////////////////////
// CGroupHostsListBox

void CGroupHostsListBox::Init(CGroup *pGroup)
{
	SetRedraw(FALSE);
	ResetContent();
	int iCount = pGroup->GetHostCount();
	for (int i = 0; i < iCount; i++)
	{
		CHost *pHost = pGroup->GetHost(i);
		AddHost(pHost);
	}
	SetRedraw(TRUE);
}

int CGroupHostsListBox::AddHost(CHost *pHost)
{
	if (!pHost->IsVisible())
		return LB_ERR;
	int index, InsertLocation = CB_ERR;
	for (int i = 0; i < GetCount(); i++)
	{
		CHost *pOldHost = (CHost *)GetItemDataPtr(i);
		ASSERT(pOldHost);
		if (pOldHost->CompareIpAddress(pHost) > 0)
		{
			InsertLocation = i;
			break;
		}
	}
	CString str = pHost->GetIpAddressString();
	char *pszName = pHost->GetName();
	if (strlen(pszName))
	{
		str += " ( ";
		str += pszName;
		str += " )";
	}
	index = InsertString(InsertLocation, str);
	SetItemDataPtr(index, pHost);
	return index;
}

void CGroupHostsListBox::RemoveHost(CHost *pHost)
{
	for (int i = 0; i < GetCount(); i++)
	{
		CHost *pOldHost = (CHost *)GetItemDataPtr(i);
		ASSERT(pOldHost);
		if (pOldHost == pHost)
		{
			DeleteString(i);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorForm

IMPLEMENT_DYNCREATE(CGroupEditorForm, CNetworkForm)

CGroupEditorForm::CGroupEditorForm() : CNetworkForm(IDD_GROUP_EDITOR_FORM)
{
	//{{AFX_DATA_INIT(CGroupEditorForm)
	//}}AFX_DATA_INIT
}

CGroupEditorForm::CGroupEditorForm(UINT nIDTemplate)  : CNetworkForm(nIDTemplate)
{
}

CGroupEditorForm::~CGroupEditorForm()
{
}

void CGroupEditorForm::DoDataExchange(CDataExchange *pDX)
{
	CNetworkForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupEditorForm)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGroupEditorForm, CNetworkForm)
	//{{AFX_MSG_MAP(CGroupEditorForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorForm diagnostics

#ifdef _DEBUG
void CGroupEditorForm::AssertValid() const
{
	CNetworkForm::AssertValid();
}

void CGroupEditorForm::Dump(CDumpContext &dc) const
{
	CNetworkForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorForm message handlers

void CGroupEditorForm::HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup)
{
//	if (GetGroup() == pOldGroup)
//		GetHostsListBox()->RemoveHost(pHost);
//	else if (GetGroup() == pNewGroup)
//		GetHostsListBox()->AddHost(pHost);
}


/////////////////////////////////////////////////////////////////////////////
// CPingGroupEditorForm

IMPLEMENT_DYNCREATE(CPingGroupEditorForm, CGroupEditorForm)

CPingGroupEditorForm::CPingGroupEditorForm()
	: CGroupEditorForm(IDD_PING_GROUP_EDITOR_FORM)
{
	//{{AFX_DATA_INIT(CPingGroupEditorForm)
	m_GroupName = _T("");
	m_bKeepAliveEnabled = FALSE;
	m_uKeepAlivePeriod = 0;
	//}}AFX_DATA_INIT
	m_pGroup = NULL;
}

CPingGroupEditorForm::~CPingGroupEditorForm()
{
}

void CPingGroupEditorForm::DoDataExchange(CDataExchange *pDX)
{
	CGroupEditorForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPingGroupEditorForm)
	DDX_Text(pDX, IDC_GROUP_NAME_EDIT, m_GroupName);
	DDX_Check(pDX, IDC_ENABLED_CHECK, m_bKeepAliveEnabled);
	DDX_Text(pDX, IDC_PERIOD_EDIT, m_uKeepAlivePeriod);
	//}}AFX_DATA_MAP
}

//virtual
LOGICAL CPingGroupEditorForm::Entering()
{
	m_pGroup = (CPingGroup *)m_pAttached->m_pObject;
	
	SetGroupName(m_pGroup->GetName());
	m_bKeepAliveEnabled = m_pGroup->IsKeepAliveEnabled();
	m_uKeepAlivePeriod = m_pGroup->GetKeepAlivePeriod();

	UpdateData(FALSE);

//	m_HostsListBox.Init(m_pGroup);

	return LOGICAL_TRUE;
}

//virtual
LOGICAL CPingGroupEditorForm::Leaving()
{
	OnApplyButton();
	return LOGICAL_TRUE;
}

BEGIN_MESSAGE_MAP(CPingGroupEditorForm, CGroupEditorForm)
	//{{AFX_MSG_MAP(CPingGroupEditorForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPingGroupEditorForm diagnostics

#ifdef _DEBUG
void CPingGroupEditorForm::AssertValid() const
{
	CGroupEditorForm::AssertValid();
}

void CPingGroupEditorForm::Dump(CDumpContext &dc) const
{
	CGroupEditorForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPingGroupEditorForm message handlers

void CPingGroupEditorForm::OnApplyButton() 
{
	UpdateData();
	m_pGroup->SetName(GetGroupName());
	m_pGroup->EnableKeepAlive(m_bKeepAliveEnabled);
	m_pGroup->SetKeepAlivePeriod(m_uKeepAlivePeriod);
	
	g_pWholeNetwork->GroupChanged(m_pGroup);
	
	ClearModifiedFlag();
}

void CPingGroupEditorForm::OnCancelButton() 
{
	Entering();
}

/////////////////////////////////////////////////////////////////////////////
// CAppearanceGroupEditorForm

IMPLEMENT_DYNCREATE(CAppearanceGroupEditorForm, CFormView)

CAppearanceGroupEditorForm::CAppearanceGroupEditorForm()
	: CGroupEditorForm(IDD_APPEARANCE_GROUP_EDITOR_FORM)
{
	//{{AFX_DATA_INIT(CAppearanceGroupEditorForm)
	m_GroupName = _T("");
	//}}AFX_DATA_INIT
	m_pGroup = NULL;
}

CAppearanceGroupEditorForm::~CAppearanceGroupEditorForm()
{
}

void CAppearanceGroupEditorForm::DoDataExchange(CDataExchange *pDX)
{
	CGroupEditorForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppearanceGroupEditorForm)
	DDX_Control(pDX, IDC_TEXT_COLOR_BUTTON, m_TextColorButton);
	DDX_Control(pDX, IDC_ALARM_COLOR_BUTTON, m_AlarmColorButton);
	DDX_Control(pDX, IDC_WARNING_COLOR_BUTTON, m_WarningColorButton);
	DDX_Control(pDX, IDC_ORDINARY_COLOR_BUTTON, m_OrdinaryColorButton);
	DDX_Text(pDX, IDC_GROUP_NAME_EDIT, m_GroupName);
	//}}AFX_DATA_MAP
}

// Windows/MFC specific
/*COLORREF RGBAtoCOLORREF(SRGBA *pRGBA)
{
	COLORREF color = RGB(pRGBA->R(), pRGBA->G(), pRGBA->B());
	return color;
}*/

//virtual
LOGICAL CAppearanceGroupEditorForm::Entering()
{
	m_pGroup = (CAppearanceGroup *)m_pAttached->m_pObject;
	
	SetGroupName(m_pGroup->GetName());
//	GetHostsListBox()->Init(m_pGroup);

	m_OrdinaryColorButton.SetColor(RGBAtoCOLORREF(m_pGroup->GetOrdinaryColor()));
	m_OrdinaryColor.Init(m_pGroup->GetOrdinaryColor());
	m_WarningColorButton.SetColor(RGBAtoCOLORREF(m_pGroup->GetWarningColor()));
	m_WarningColor.Init(m_pGroup->GetWarningColor());
	m_AlarmColorButton.SetColor(RGBAtoCOLORREF(m_pGroup->GetAlarmColor()));
	m_AlarmColor.Init(m_pGroup->GetAlarmColor());
	m_TextColorButton.SetColor(RGBAtoCOLORREF(m_pGroup->GetTextColor()));
	m_TextColor.Init(m_pGroup->GetTextColor());

	return LOGICAL_TRUE;
}

//virtual
LOGICAL CAppearanceGroupEditorForm::Leaving()
{
	OnApplyButton();
	return LOGICAL_TRUE;
}


BEGIN_MESSAGE_MAP(CAppearanceGroupEditorForm, CGroupEditorForm)
	//{{AFX_MSG_MAP(CAppearanceGroupEditorForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	ON_BN_CLICKED(IDC_ORDINARY_COLOR_BUTTON, OnOrdinaryColorButton)
	ON_BN_CLICKED(IDC_WARNING_COLOR_BUTTON, OnWarningColorButton)
	ON_BN_CLICKED(IDC_ALARM_COLOR_BUTTON, OnAlarmColorButton)
	ON_BN_CLICKED(IDC_TEXT_COLOR_BUTTON, OnTextColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppearanceGroupEditorForm diagnostics

#ifdef _DEBUG
void CAppearanceGroupEditorForm::AssertValid() const
{
	CGroupEditorForm::AssertValid();
}

void CAppearanceGroupEditorForm::Dump(CDumpContext &dc) const
{
	CGroupEditorForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAppearanceGroupEditorForm message handlers

void CAppearanceGroupEditorForm::OnApplyButton() 
{
	UpdateData();
	
	m_pGroup->SetName(GetGroupName());
	m_pGroup->GetOrdinaryColor()->Init(&m_OrdinaryColor);
	m_pGroup->GetWarningColor()->Init(&m_WarningColor);
	m_pGroup->GetAlarmColor()->Init(&m_AlarmColor);
	m_pGroup->GetTextColor()->Init(&m_TextColor);
	m_pGroup->InitSelectedColors();
	
	g_pWholeNetwork->GroupChanged(m_pGroup);	
	ClearModifiedFlag();
}

void CAppearanceGroupEditorForm::OnCancelButton() 
{
	Entering();
}

void CAppearanceGroupEditorForm::OnOrdinaryColorButton() 
{
	m_OrdinaryColorButton.ChangeColor(&m_OrdinaryColor);
}

void CAppearanceGroupEditorForm::OnWarningColorButton() 
{
	m_WarningColorButton.ChangeColor(&m_WarningColor);
}

void CAppearanceGroupEditorForm::OnAlarmColorButton() 
{
	m_AlarmColorButton.ChangeColor(&m_AlarmColor);
}

void CAppearanceGroupEditorForm::OnTextColorButton() 
{
	m_TextColorButton.ChangeColor(&m_TextColor);
}


/////////////////////////////////////////////////////////////////////////////
// CSnmpGroupEditorForm

IMPLEMENT_DYNCREATE(CSnmpGroupEditorForm, CGroupEditorForm)

CSnmpGroupEditorForm::CSnmpGroupEditorForm()
	: CGroupEditorForm(IDD_SNMP_GROUP_EDITOR_FORM)
{
	//{{AFX_DATA_INIT(CSnmpGroupEditorForm)
	m_bEnabled = FALSE;
	m_nRetries = 0;
	//}}AFX_DATA_INIT
	m_pGroup = NULL;
}

CSnmpGroupEditorForm::~CSnmpGroupEditorForm()
{
}

void CSnmpGroupEditorForm::DoDataExchange(CDataExchange *pDX)
{
	CGroupEditorForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSnmpGroupEditorForm)
	DDX_Control(pDX, IDC_RETRIES_SPIN, m_RetriesSpin);
	DDX_Text(pDX, IDC_GROUP_NAME_EDIT, m_GroupName);
	DDX_Text(pDX, IDC_COMMUNITY_NAME_EDIT, m_CommunityName);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_Password);
	DDX_Check(pDX, IDC_ENABLED_CHECK, m_bEnabled);
	DDX_Text(pDX, IDC_RETRIES_EDIT, m_nRetries);
	//}}AFX_DATA_MAP
}

//virtual
LOGICAL CSnmpGroupEditorForm::Entering()
{
	m_pGroup = (CSnmpGroup *)m_pAttached->m_pObject;
	
	SetGroupName(m_pGroup->GetName());
	m_bEnabled = m_pGroup->IsSnmpEnabled();
	m_CommunityName = m_pGroup->GetCommunityName();
	m_Password = m_pGroup->GetPassword();
	m_nRetries = m_pGroup->GetRetries();

	UpdateData(FALSE);

//	m_HostsListBox.Init(m_pGroup);

	return LOGICAL_TRUE;
}

//virtual
LOGICAL CSnmpGroupEditorForm::Leaving()
{
	OnApplyButton();
	return LOGICAL_TRUE;
}

BEGIN_MESSAGE_MAP(CSnmpGroupEditorForm, CGroupEditorForm)
	//{{AFX_MSG_MAP(CSnmpGroupEditorForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnmpGroupEditorForm diagnostics

#ifdef _DEBUG
void CSnmpGroupEditorForm::AssertValid() const
{
	CGroupEditorForm::AssertValid();
}

void CSnmpGroupEditorForm::Dump(CDumpContext &dc) const
{
	CGroupEditorForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSnmpGroupEditorForm message handlers

void CSnmpGroupEditorForm::OnApplyButton() 
{
	UpdateData();
	m_pGroup->SetName(GetGroupName());
	m_pGroup->EnableSnmp(m_bEnabled);
	m_pGroup->SetCommunityName(m_CommunityName);
	m_pGroup->SetPassword(m_Password);
	m_pGroup->SetRetries(m_nRetries);
	
	g_pWholeNetwork->GroupChanged(m_pGroup);
	
	ClearModifiedFlag();
}

void CSnmpGroupEditorForm::OnCancelButton() 
{
	Entering();
}

//virtual 
void CSnmpGroupEditorForm::OnInitialUpdate()
{
	CNetworkForm::OnInitialUpdate();
	m_RetriesSpin.SetBuddy(GetDescendantWindow(IDC_RETRIES_EDIT));
	m_RetriesSpin.SetRange(0, 256);
}

/////////////////////////////////////////////////////////////////////////////
// CGroupEditorDocTemplate

CGroupEditorDocTemplate::CGroupEditorDocTemplate(UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass * pViewClass)
	: CNetworkDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

CNetworkDocAPI *CGroupEditorDocTemplate::CreateNewWindow(CGroup *pGroup)
{
	CNetworkDocAPI *pDoc = NULL;
	POSITION Pos = GetFirstDocPosition();
	if (Pos)
		pDoc = (CNetworkDocAPI *)GetNextDoc(Pos);
	else
		pDoc = (CNetworkDocAPI *)OpenDocumentFile(NULL);
	pDoc->UpdateWindowCaption();
	pDoc->BringToTop();
	return pDoc;
}

