/* NetObjectConfig.cpp 
	
Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Oct 2007 - initial creation
	14 Nov 2007 - templates for host and subnet configure windows
	15 Nov 2007 - switching between control forms
	16 Nov 2007 - control form initialization
	10 Jan 2008 - host group created and changed functionality
	13 Mar 2008 - HostChanged event processing
	08 Jun 2008 - host appearance control form
	26 Aug 2008 - host SNMP control form
	28 Aug 2008 - Windows messages instead of method calls for more safe multythreading
*/


#include "stdafx.h"
#include "netobserver.h"
#include "NetObjectConfig.h"
#include "ControlForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigDoc

IMPLEMENT_DYNCREATE(CNetObjectConfigDoc, CNetworkDocAPI)

CNetObjectConfigDoc::CNetObjectConfigDoc()
{
}

BOOL CNetObjectConfigDoc::OnNewDocument()
{
	if (!CNetworkDocAPI::OnNewDocument())
		return FALSE;
	return TRUE;
}

CNetObjectConfigDoc::~CNetObjectConfigDoc()
{
}

BEGIN_MESSAGE_MAP(CNetObjectConfigDoc, CNetworkDocAPI)
	//{{AFX_MSG_MAP(CNetObjectConfigDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigDoc diagnostics

#ifdef _DEBUG
void CNetObjectConfigDoc::AssertValid() const
{
	CNetworkDocAPI::AssertValid();
}

void CNetObjectConfigDoc::Dump(CDumpContext &dc) const
{
	CNetworkDocAPI::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigDoc commands


/////////////////////////////////////////////////////////////////////////////
// CHostConfigDoc

IMPLEMENT_DYNCREATE(CHostConfigDoc, CNetObjectConfigDoc)

CHostConfigDoc::CHostConfigDoc()
{
	memset(m_pControlForms, 0, sizeof(m_pControlForms));
}

BOOL CHostConfigDoc::OnNewDocument()
{
	if (!CNetObjectConfigDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}

CHostConfigDoc::~CHostConfigDoc()
{
}

//virtual
void CHostConfigDoc::Initialize(CHost *pHost)
{
	m_pNetObject = pHost;
	CHostConfigFrame *pFrame = (CHostConfigFrame *)m_pFrame;
	pFrame->Init(pHost);
	for (int i = DEFAULT_CONTROL_INDEX; i < STOP_CONTROL_INDEX; i++)
	{
		CHostControlForm *pForm = m_pControlForms[i];
		if (pForm)
			pForm->Init(pHost);
	}
}

//virtual
void CHostConfigDoc::CreateControlForms(CSplitterWnd &Splitter)
{
	m_pControlForms[DEFAULT_CONTROL_INDEX] = (CHostControlForm *)Splitter.GetPane(1,0);
	AddControlForm(HOST_NAMES_CONTROL_INDEX, new CHostNamesControlForm(), &Splitter);
	AddControlForm(HOST_ADDRESSES_CONTROL_INDEX, new CHostAddressesControlForm(), &Splitter);
	AddControlForm(HOST_APPEARANCE_CONTROL_INDEX, new CHostAppearanceControlForm(), &Splitter);
	AddControlForm(HOST_PING_CONTROL_INDEX, new CHostPingControlForm(), &Splitter);
	AddControlForm(HOST_SNMP_CONTROL_INDEX, new CHostSnmpControlForm(), &Splitter);
}

//virtual
CControlForm *CHostConfigDoc::GetControlForm(int nViewIndex)
{
	return m_pControlForms[nViewIndex];
}

void CHostConfigDoc::AddControlForm(int IndexInViewArray, CHostControlForm *pForm, CWnd *pParent)
{
	CreateForm(IndexInViewArray, pForm, pParent);
	m_pControlForms[IndexInViewArray] = pForm;
}

BEGIN_MESSAGE_MAP(CHostConfigDoc, CNetObjectConfigDoc)
	//{{AFX_MSG_MAP(CHostConfigDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostConfigDoc diagnostics

#ifdef _DEBUG
void CHostConfigDoc::AssertValid() const
{
	CNetObjectConfigDoc::AssertValid();
}

void CHostConfigDoc::Dump(CDumpContext &dc) const
{
	CNetObjectConfigDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHostConfigDoc commands


/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigDoc

IMPLEMENT_DYNCREATE(CSubnetConfigDoc, CNetObjectConfigDoc)

CSubnetConfigDoc::CSubnetConfigDoc()
{
	memset(m_pControlForms, 0, sizeof(m_pControlForms));
}

BOOL CSubnetConfigDoc::OnNewDocument()
{
	if (!CNetObjectConfigDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}

CSubnetConfigDoc::~CSubnetConfigDoc()
{
}

//virtual
void CSubnetConfigDoc::Initialize(CSubnet *pSubnet)
{
	m_pNetObject = pSubnet;
	CSubnetConfigFrame *pFrame = (CSubnetConfigFrame *)m_pFrame;
	pFrame->Init(pSubnet);
	for (int i = DEFAULT_CONTROL_INDEX; i < STOP_CONTROL_INDEX; i++)
	{
		CSubnetControlForm *pForm = m_pControlForms[i];
		if (pForm)
			pForm->Init(pSubnet);
	}
}

//virtual
void CSubnetConfigDoc::CreateControlForms(CSplitterWnd &Splitter)
{
	m_pControlForms[DEFAULT_CONTROL_INDEX] = (CSubnetControlForm *)Splitter.GetPane(1,0);
	AddControlForm(SUBNET_NAMES_CONTROL_INDEX, new CSubnetNamesControlForm(), &Splitter);
	AddControlForm(SUBNET_ADDRESS_CONTROL_INDEX, new CSubnetAddressControlForm(), &Splitter);
	AddControlForm(SUBNET_APPEARANCE_CONTROL_INDEX, new CSubnetAppearanceControlForm(), &Splitter);
}

//virtual
CControlForm *CSubnetConfigDoc::GetControlForm(int nViewIndex)
{
	return m_pControlForms[nViewIndex];
}

void CSubnetConfigDoc::AddControlForm(int IndexInViewArray, CSubnetControlForm *pForm, CWnd *pParent)
{
	CreateForm(IndexInViewArray, pForm, pParent);
	m_pControlForms[IndexInViewArray] = pForm;
}

BEGIN_MESSAGE_MAP(CSubnetConfigDoc, CNetObjectConfigDoc)
	//{{AFX_MSG_MAP(CSubnetConfigDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigDoc diagnostics

#ifdef _DEBUG
void CSubnetConfigDoc::AssertValid() const
{
	CNetObjectConfigDoc::AssertValid();
}

void CSubnetConfigDoc::Dump(CDumpContext &dc) const
{
	CNetObjectConfigDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigDoc commands


/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigTreeView

IMPLEMENT_DYNCREATE(CNetObjectConfigTreeView, CNetworkTreeView)

CNetObjectConfigTreeView::CNetObjectConfigTreeView()
{
	m_pFrame = NULL;
}

CNetObjectConfigTreeView::~CNetObjectConfigTreeView()
{
}


BEGIN_MESSAGE_MAP(CNetObjectConfigTreeView, CNetworkTreeView)
	//{{AFX_MSG_MAP(CNetObjectConfigTreeView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//virtual
CNetworkForm *CNetObjectConfigTreeView::GetForm(int nViewIndex)
{
	CNetObjectConfigDoc *pDoc = (CNetObjectConfigDoc *)m_pDocument;
	return pDoc->GetControlForm(nViewIndex);
}

/////////////////////////////////////////////////////////////////////////////
// CHostConfigTreeView drawing

void CNetObjectConfigTreeView::OnDraw(CDC *pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

void CNetObjectConfigTreeView::ObjectChanged(CNetObject *pObject)
{
	if (m_pCurrentlyVisibleForm)
		m_pCurrentlyVisibleForm->UpdateReadOnlyFields();
}

/////////////////////////////////////////////////////////////////////////////
// CHostConfigTreeView diagnostics

#ifdef _DEBUG
void CNetObjectConfigTreeView::AssertValid() const
{
	CNetworkTreeView::AssertValid();
}

void CNetObjectConfigTreeView::Dump(CDumpContext &dc) const
{
	CNetworkTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigTreeView message handlers

void CNetObjectConfigTreeView::OnInitialUpdate() 
{
	CNetworkTreeView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CHostConfigTreeView

IMPLEMENT_DYNCREATE(CHostConfigTreeView, CNetObjectConfigTreeView)

CHostConfigTreeView::CHostConfigTreeView()
{
	m_pHost = NULL;
}

CHostConfigTreeView::~CHostConfigTreeView()
{
}

BEGIN_MESSAGE_MAP(CHostConfigTreeView, CNetObjectConfigTreeView)
	//{{AFX_MSG_MAP(CHostConfigTreeView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostConfigTreeView drawing

void CHostConfigTreeView::OnDraw(CDC *pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CHostConfigTreeView diagnostics

#ifdef _DEBUG
void CHostConfigTreeView::AssertValid() const
{
	CNetObjectConfigTreeView::AssertValid();
}

void CHostConfigTreeView::Dump(CDumpContext &dc) const
{
	CNetObjectConfigTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHostConfigTreeView message handlers

void CHostConfigTreeView::OnInitialUpdate() 
{
	CNetObjectConfigTreeView::OnInitialUpdate();
	// Common items
	HTREEITEM hCommonItem = NewItem("Common", 4, DEFAULT_CONTROL_INDEX, NULL);
	NewItem("Names", 4, HOST_NAMES_CONTROL_INDEX, NULL, hCommonItem);
	NewItem("Addresses", 4, HOST_ADDRESSES_CONTROL_INDEX, NULL, hCommonItem);
	NewItem("Appearance", 4, HOST_APPEARANCE_CONTROL_INDEX, NULL, hCommonItem);
	// Ping item
	NewItem("Ping", 4, HOST_PING_CONTROL_INDEX, NULL);
	// Snmp item
	NewItem("SNMP", 4, HOST_SNMP_CONTROL_INDEX, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigTreeView

IMPLEMENT_DYNCREATE(CSubnetConfigTreeView, CNetObjectConfigTreeView)

CSubnetConfigTreeView::CSubnetConfigTreeView()
{
	m_pSubnet = NULL;
}

CSubnetConfigTreeView::~CSubnetConfigTreeView()
{
}


BEGIN_MESSAGE_MAP(CSubnetConfigTreeView, CNetObjectConfigTreeView)
	//{{AFX_MSG_MAP(CSubnetConfigTreeView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigTreeView drawing

void CSubnetConfigTreeView::OnDraw(CDC *pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigTreeView diagnostics

#ifdef _DEBUG
void CSubnetConfigTreeView::AssertValid() const
{
	CNetObjectConfigTreeView::AssertValid();
}

void CSubnetConfigTreeView::Dump(CDumpContext &dc) const
{
	CNetObjectConfigTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigTreeView message handlers


void CSubnetConfigTreeView::OnInitialUpdate() 
{
	CNetObjectConfigTreeView::OnInitialUpdate();
	HTREEITEM hCommonItem = NewItem("Common", 4, DEFAULT_CONTROL_INDEX, NULL);
	NewItem("Names", 4, SUBNET_NAMES_CONTROL_INDEX, NULL, hCommonItem, NULL);
	NewItem("Addresses", 4, SUBNET_ADDRESS_CONTROL_INDEX, NULL, hCommonItem, NULL);
	NewItem("Appearance", 4, SUBNET_APPEARANCE_CONTROL_INDEX, NULL, hCommonItem, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigFrame

IMPLEMENT_DYNCREATE(CNetObjectConfigFrame, CNetworkFrameAPI)

CNetObjectConfigFrame::CNetObjectConfigFrame()
{
	m_pTreeView = NULL;
}

CNetObjectConfigFrame::~CNetObjectConfigFrame()
{
}

CNetworkForm *CNetObjectConfigFrame::GetCurrentForm()
{
	return (CNetworkForm *)m_SplitterHor.GetPane(1, 0);
}

//virtual
BOOL CNetObjectConfigFrame::OnIdleMFC()
{
	CNetworkForm *pCurrentForm = GetCurrentForm();
	if (pCurrentForm)
		pCurrentForm->UpdateStatus();
	return FALSE;
}

BEGIN_MESSAGE_MAP(CNetObjectConfigFrame, CNetworkFrameAPI)
	//{{AFX_MSG_MAP(CNetObjectConfigFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetObjectConfigFrame message handlers

BOOL CNetObjectConfigFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) 
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
		GetControlFormRunTime(), CSize(100, 100), pContext))
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
	
	CHostConfigDoc *pDoc = (CHostConfigDoc *)pContext->m_pCurrentDoc;
	pDoc->CreateControlForms(m_SplitterHor);
  
	m_pTreeView = (CNetObjectConfigTreeView *)m_SplitterHor.GetPane(0, 0);
	CNetworkForm *pCurrentForm = GetCurrentForm();
	m_pTreeView->SetEnv(this, pCurrentForm);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHostConfigFrame

IMPLEMENT_DYNCREATE(CHostConfigFrame, CNetObjectConfigFrame)

CHostConfigFrame::CHostConfigFrame()
{
}

CHostConfigFrame::~CHostConfigFrame()
{
}

void CHostConfigFrame::Init(CHost *pHost)
{
	m_pTreeView->Init(pHost);
}

CHostControlForm *CHostConfigFrame::GetCurrentForm()
{
	return (CHostControlForm *)m_SplitterHor.GetPane(1, 0);
}

//virtual
void CHostConfigFrame::HostChanged(CHost *pHost)
{
	m_pTreeView->ObjectChanged(pHost);
}

//virtual
void CHostConfigFrame::NewGroupCreated(CGroup *pGroup)
{
	CHostControlForm *pCurrentForm = (CHostControlForm *)GetCurrentForm();
	pCurrentForm->NewGroupCreated(pGroup);
}

//virtual
void CHostConfigFrame::GroupChanged(CGroup *pGroup)
{
	CHostControlForm *pCurrentForm = (CHostControlForm *)GetCurrentForm();
	pCurrentForm->GroupChanged(pGroup);
}

//virtual
CRuntimeClass *CHostConfigFrame::GetControlFormRunTime()
{
	return RUNTIME_CLASS(CHostControlForm);
}

BEGIN_MESSAGE_MAP(CHostConfigFrame, CNetObjectConfigFrame)
	//{{AFX_MSG_MAP(CHostConfigFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostConfigFrame message handlers

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigFrame

IMPLEMENT_DYNCREATE(CSubnetConfigFrame, CNetObjectConfigFrame)

CSubnetConfigFrame::CSubnetConfigFrame()
{
}

CSubnetConfigFrame::~CSubnetConfigFrame()
{
}

void CSubnetConfigFrame::Init(CSubnet *pSubnet)
{
	m_pTreeView->Init(pSubnet);
}

//virtual
CRuntimeClass *CSubnetConfigFrame::GetControlFormRunTime()
{
	return RUNTIME_CLASS(CSubnetControlForm);
}

BEGIN_MESSAGE_MAP(CSubnetConfigFrame, CNetObjectConfigFrame)
	//{{AFX_MSG_MAP(CSubnetConfigFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubnetConfigFrame message handlers


