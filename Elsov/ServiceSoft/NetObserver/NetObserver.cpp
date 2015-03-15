/* NetObserver.cpp : Defines the class behaviors for the application.

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	01 Sep 2006 - idle thread instead of OnIdle() method
	01 Nov 2006 - IDatabaseAPI interface
	15 Jun 2007 - Host traffic display
	14 Nov 2007 - CNetworkDocTemplate, CHostDocTemplate, CSubnetDocTemplate
	15 Nov 2007 - global IGuiAPI pointer instead of member variables
	16 Nov 2007 - global IDatabaseAPI pointer instead of member variables
	10 Jan 2008 - host group created and changed functionality
	15 Jan 2008 - splitting MFC- and NonMFC-visitors to avoid multithread problems with MFC-controls 
	20 Mar 2008 - moving host between groups event handling
*/


#include "stdafx.h"
#include "NetObserver.h"
#include "NetObserverFrame.h"

#include "WholeNetDocView.h"

#include "DisplayHostTraffic.h"
#include "NetObjectConfig.h"
#include "GroupEditor.h"
#include "Network.h"
#include "OdbcDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IGuiAPI *g_pGui = NULL;

/////////////////////////////////////////////////////////////////////
// CIdleNonMFCThread class implementation

//Constuctors/destructors
CIdleNonMFCThread::CIdleNonMFCThread(CNetObserverApp *pApp)
{
	m_pApp = pApp;
}

//virtual
CIdleNonMFCThread::~CIdleNonMFCThread()
{
}

//virtual
void CIdleNonMFCThread::OnThreadStarted()
{
}

//virtual
LOGICAL CIdleNonMFCThread::ThreadBody()
{
	if (NeedContinueThread())
	{
		m_pApp->OnIdleNonMFCThread();
		portableSleep(100);
	}

	return LOGICAL_TRUE;
}

//virtual
void CIdleNonMFCThread::OnThreadEnded()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetObserverApp

BEGIN_MESSAGE_MAP(CNetObserverApp, CWinApp)
	//{{AFX_MSG_MAP(CNetObserverApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HOST_CONFIGURE, OnHostConfigure)
	ON_COMMAND(ID_HOST_LOOK_TRAFFIC, OnHostLookTraffic)
	ON_COMMAND(ID_SUBNET_CONFIGURE, OnSubnetConfigure)
	ON_COMMAND(ID_HOST_DELETE, OnHostDelete)
	ON_UPDATE_COMMAND_UI(ID_HOST_DELETE, OnUpdateHostDelete)
	ON_COMMAND(ID_HOST_HIDE, OnHostHide)
	ON_UPDATE_COMMAND_UI(ID_HOST_HIDE, OnUpdateHostHide)
	ON_COMMAND(ID_VIEW_SHOW_IGNORED_OBJECTS, OnViewShowIgnoredObjects)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_IGNORED_OBJECTS, OnUpdateViewShowIgnoredObjects)
	ON_COMMAND(ID_VIEW_SHOW_HIDDEN_OBJECTS, OnViewShowHiddenObjects)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_HIDDEN_OBJECTS, OnUpdateViewShowHiddenObjects)
	ON_COMMAND(ID_TOOLS_GROUP_EDITOR, OnToolsGroupEditor)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetObserverApp construction

CNetObserverApp::CNetObserverApp()
{
	m_bHiddenObjectsVisible = LOGICAL_TRUE;
	m_bIgnoredObjectsVisible = LOGICAL_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNetObserverApp object

CNetObserverApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNetObserverApp initialization

//virtual
BOOL CNetObserverApp::InitInstance()
{
	g_pGui = this;

	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	SetRegistryKey(_T("Kozhevnikov"));

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pWholeNetDocTemplate = new CNetworkDocTemplate(
		IDR_NETOBSTYPE,
		RUNTIME_CLASS(CWholeNetDoc),
		RUNTIME_CLASS(CWholeNetFrame), // custom MDI child frame
		RUNTIME_CLASS(CWholeNetTreeView));
	AddDocTemplate(m_pWholeNetDocTemplate);

	m_pDisplayHostTrafficDocTemplate = new CHostDocTemplate(
		IDR_HOST_TRAFFIC,
		RUNTIME_CLASS(CDisplayHostTrafficDoc),
		RUNTIME_CLASS(CDisplayHostTrafficFrame), // custom MDI child frame
		RUNTIME_CLASS(CDisplayHostTrafficView));
	AddDocTemplate(m_pDisplayHostTrafficDocTemplate);

	m_pHostConfigDocTemplate = new CHostDocTemplate(
		IDR_HOST_CONFIG,
		RUNTIME_CLASS(CHostConfigDoc),
		RUNTIME_CLASS(CHostConfigFrame), // custom MDI child frame
		RUNTIME_CLASS(CHostConfigTreeView));
	AddDocTemplate(m_pHostConfigDocTemplate);
	
	m_pSubnetConfigDocTemplate = new CSubnetDocTemplate(
		IDR_SUBNET_CONFIG,
		RUNTIME_CLASS(CSubnetConfigDoc),
		RUNTIME_CLASS(CSubnetConfigFrame), // custom MDI child frame
		RUNTIME_CLASS(CSubnetConfigTreeView));
	AddDocTemplate(m_pSubnetConfigDocTemplate);
	
	m_pGroupEditorDocTemplate = new CGroupEditorDocTemplate(
		IDR_GROUP_EDITOR,
		RUNTIME_CLASS(CGroupEditorDoc),
		RUNTIME_CLASS(CGroupEditorFrame), // custom MDI child frame
		RUNTIME_CLASS(CGroupEditorForm));
	AddDocTemplate(m_pGroupEditorDocTemplate);
	
	// create main MDI Frame window
	CNetObserverFrame *pMainFrame = new CNetObserverFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	/*
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE; */
	CNetworkDocAPI *pDoc = (CNetworkDocAPI *)m_pWholeNetDocTemplate->OpenDocumentFile(NULL);
	pDoc->UpdateWindowCaption();

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	pDoc->UpdateWindowCaption();

	g_pDB = OpenDatabase();
	if (!g_pDB)
		return FALSE;
	
	CWaitCursor cursor;

	CreateAllThreads();
	m_pMainWnd->SetTimer(1, 1000, NULL);

	return TRUE;
}

//virtual
int CNetObserverApp::ExitInstance() 
{
	return CWinApp::ExitInstance();
}

//virtual
IDatabaseAPI *CNetObserverApp::OpenDatabase()
{
	// Add DB type chosing dialog (ODBC/DAO/etc) here
	COdbcDatabase *pOdbcDatabase = new COdbcDatabase;
	IDatabaseAPI *pDatabase = pOdbcDatabase;
	if (!pOdbcDatabase->Create())
	{
		delete pOdbcDatabase; pOdbcDatabase = NULL;
		pDatabase = NULL;
		if (IDYES == MessageBox(NULL, "Database is not open.\nDo you want to continue w/o database?", "Error", MB_ICONEXCLAMATION | MB_YESNO))
		{
			pDatabase = new CDatabaseStub;
		}
	}
	return pDatabase;
}

//virtual
void CNetObserverApp::NewHostCreated(CHost *pHost)
{
	CNewHostCreatedVisitor v(pHost);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::HostChanged(CHost *pHost)
{
	CHostChangedVisitor v(pHost);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::NewSubnetCreated(CSubnet *pSubnet)
{
	CNewSubnetCreatedVisitor v(pSubnet);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::SubnetChanged(CSubnet *pSubnet)
{
	CSubnetChangedVisitor v(pSubnet);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	CHostAddedToSubnetVisitor v(pHost, pSubnet);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
	CHostRemovedFromSubnetVisitor v(pHost, pSubnet);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::HostIgnored(CHost *pHost)
{
	CNetObjectIgnoredVisitor v(pHost);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::HostHidden(CHost *pHost)
{
	CNetObjectHiddenVisitor v(pHost);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::IgnoredObjectsVisibilityChanged()
{
	CShowIgnoredObjectsVisitor v;
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::HiddenObjectsVisibilityChanged()
{
	CShowHiddenObjectsVisitor v;
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::NewGroupCreated(CGroup *pGroup)
{
	CNewGroupCreatedVisitor v(pGroup);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::GroupChanged(CGroup *pGroup)
{
	CGroupChangedVisitor v(pGroup);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup)
{
	ASSERT(pOldGroup != pNewGroup);
	CHostMovedBetweenGroupsVisitor v(pHost, pOldGroup, pNewGroup);
	VisitAllDocs(&v);
}

//virtual
void CNetObserverApp::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	CObjectSelectionChangedVisitor v(pObject, pWhereChanged);
	VisitAllDocs(&v);
}

//virtual
LOGICAL CNetObserverApp::HostContextMenu(int ScreenX, int ScreenY, CHost *pHost)
{
	m_pHostContextMenu = pHost;
	CMenu menu;
	BOOL b1 = menu.CreatePopupMenu();
	BOOL b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_HOST_CONFIGURE, "Configure");
	b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_HOST_DELETE, "Deleted");
	b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_HOST_HIDE, "Hidden");
	b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_HOST_LOOK_TRAFFIC, "Traffic");
	BOOL b3 = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, ScreenX, ScreenY, m_pMainWnd, NULL);
	menu.DestroyMenu();

	return LOGICAL_TRUE;
}

//virtual
LOGICAL CNetObserverApp::SubnetContextMenu(int ScreenX, int ScreenY, CSubnet *pSubnet)
{
	m_pSubnetContextMenu = pSubnet;

	CMenu menu;
	BOOL b1 = menu.CreatePopupMenu();
	BOOL b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_SUBNET_CONFIGURE, "Configure");
	b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_SUBNET_DELETE, "Deleted");
	BOOL b3 = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, ScreenX, ScreenY, m_pMainWnd, NULL);

	menu.DestroyMenu();

	return LOGICAL_TRUE;
}

unsigned long CNetObserverApp::VisitAllDocs(CDocVisitorAPI *pVisitor)
{
	if (!g_pGui)
		return 0;

	for (POSITION TemplatePos = GetFirstDocTemplatePosition(); TemplatePos;)
	{
		CDocTemplate *pTemplate = GetNextDocTemplate(TemplatePos);
		for (POSITION DocPos = pTemplate->GetFirstDocPosition(); DocPos;)
		{
			CNetworkDocAPI *pDoc = (CNetworkDocAPI *)pTemplate->GetNextDoc(DocPos);
			pVisitor->Visit(pDoc);
		}
	}
	return 0;
}

BOOL CNetObserverApp::OnIdleNonMFCThread()
{
	COnIdleNonMFCVisitor v;
	VisitAllDocs(&v);
	return TRUE;
}

// App command to run the dialog
void CNetObserverApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CNetObserverApp::OnHostConfigure() 
{
	CWaitCursor cursor;
	m_pHostConfigDocTemplate->CreateNewWindow(m_pHostContextMenu);
	
	UpdateWholeGUI();
}

void CNetObserverApp::OnHostLookTraffic() 
{
	CWaitCursor cursor;
	m_pDisplayHostTrafficDocTemplate->CreateNewWindow(m_pHostContextMenu);
}

void CNetObserverApp::OnSubnetConfigure() 
{
	CWaitCursor cursor;
	m_pSubnetConfigDocTemplate->CreateNewWindow(m_pSubnetContextMenu);

	UpdateWholeGUI();
}

void CNetObserverApp::UpdateWholeGUI()
{
	CUpdateGuiVisitor v;
	VisitAllDocs(&v);
}

void CNetObserverApp::CreateAllThreads()
{
	g_pWholeNetwork = new CNetwork();
	g_pWholeNetwork->Run();
	m_pIdleThread = new CIdleNonMFCThread(this);
	m_pIdleThread->Run();
}

void CNetObserverApp::KillAllThreads()
{
	g_pGui = NULL;
	if (m_pIdleThread)
	{
		m_pIdleThread->Stop();
		delete m_pIdleThread;
	}
	m_pIdleThread = NULL;

	if (g_pWholeNetwork)
		delete g_pWholeNetwork;
	g_pWholeNetwork = NULL;

	if (g_pDB)
		delete g_pDB;
	g_pDB = NULL;
}


void CNetObserverApp::OnHostDelete() 
{
	ASSERT(m_pHostContextMenu);
	g_pWholeNetwork->IgnoreHost(m_pHostContextMenu);
}

void CNetObserverApp::OnUpdateHostDelete(CCmdUI *pCmdUI) 
{
	ASSERT(m_pHostContextMenu);
	pCmdUI->SetCheck(m_pHostContextMenu->IsIgnored());
}

void CNetObserverApp::OnHostHide() 
{
	ASSERT(m_pHostContextMenu);
	g_pWholeNetwork->HideHost(m_pHostContextMenu);
}

void CNetObserverApp::OnUpdateHostHide(CCmdUI *pCmdUI) 
{
	ASSERT(m_pHostContextMenu);
	pCmdUI->SetCheck(m_pHostContextMenu->IsHidden());
}

void CNetObserverApp::OnViewShowIgnoredObjects() 
{
	m_bIgnoredObjectsVisible = ! m_bIgnoredObjectsVisible;
	g_pWholeNetwork->ShowIgnoredObjects(m_bIgnoredObjectsVisible);
}


void CNetObserverApp::OnUpdateViewShowIgnoredObjects(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_bIgnoredObjectsVisible);
}

void CNetObserverApp::OnViewShowHiddenObjects() 
{
	m_bHiddenObjectsVisible = ! m_bHiddenObjectsVisible;
	g_pWholeNetwork->ShowHiddenObjects(m_bHiddenObjectsVisible);
}

void CNetObserverApp::OnUpdateViewShowHiddenObjects(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_bHiddenObjectsVisible);
}


void CNetObserverApp::OnToolsGroupEditor() 
{
	m_pGroupEditorDocTemplate->CreateNewWindow(NULL);
}

BOOL CNetObserverApp::OnIdle(LONG lCount) 
{
	COnIdleMFCVisitor v;
	VisitAllDocs(&v);
	return CWinApp::OnIdle(lCount);
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog

