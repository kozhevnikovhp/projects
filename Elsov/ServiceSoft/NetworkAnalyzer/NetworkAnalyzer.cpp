/* NetworkAnalyser.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	01 Sep 2006 - idle thread instead of OnIdle() method
	01 Nov 2006 - IDatabaseAPI interface
	15 Jun 2007 - Host traffic display
*/

#include "stdafx.h"
#include "NetworkAnalyzer.h"

#include "NetworkAnalyzerFrame.h"
#include "NetworkAnalyzerDoc.h"
#include "NetworkAnalyzerView.h"
#include "NetworkDocAPI.h"
#include "Network.h"
#include "Configurator.h"
#include "HostTrafficDialog.h"
#include "OdbcDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////
// CAboutDlg class implementation

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


/////////////////////////////////////////////////////////////////////
// CIdleThread class implementation

//Constuctors/destructors
CIdleThread::CIdleThread(CNetworkAnalyzerApp *pApp)
{
	m_pApp = pApp;
}

//virtual
CIdleThread::~CIdleThread()
{
}

//virtual
void CIdleThread::OnThreadStarted()
{
}

//virtual
LOGICAL CIdleThread::ThreadBody()
{
	if (NeedContinueThread())
	{
		m_pApp->IdleProcess();
		portableSleep(100);
	}

	return LOGICAL_TRUE;
}

//virtual
void CIdleThread::OnThreadEnded()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerApp

BEGIN_MESSAGE_MAP(CNetworkAnalyzerApp, CWinApp)
	//{{AFX_MSG_MAP(CNetworkAnalyzerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HOST_DELETE, OnHostDelete)
	ON_UPDATE_COMMAND_UI(ID_HOST_DELETE, OnUpdateHostDelete)
	ON_COMMAND(ID_HOST_CONFIGURE, OnHostConfigure)
	ON_UPDATE_COMMAND_UI(ID_HOST_CONFIGURE, OnUpdateHostConfigure)
	ON_COMMAND(ID_SUBNET_CONFIGURE, OnSubnetConfigure)
	ON_UPDATE_COMMAND_UI(ID_SUBNET_CONFIGURE, OnUpdateSubnetConfigure)
	ON_COMMAND(ID_SUBNET_DELETE, OnSubnetDelete)
	ON_UPDATE_COMMAND_UI(ID_SUBNET_DELETE, OnUpdateSubnetDelete)
	ON_COMMAND(ID_HOST_LOOK_TRAFFIC, OnHostLookTraffic)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerApp construction

CNetworkAnalyzerApp::CNetworkAnalyzerApp()
{
	m_pDatabase = NULL;
	m_pHostContextMenu = NULL;
	m_pSubnetContextMenu = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNetworkAnalyzerApp object

CNetworkAnalyzerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerApp initialization

BOOL CNetworkAnalyzerApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Kozhevnikov"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	m_ucHostColor[0] = 0;
	m_ucHostColor[1] = 127;
	m_ucHostColor[2] = 0;
	m_ucHostColor[3] = 0;

	m_ucSelectedHostColor[0] = 0;
	m_ucSelectedHostColor[1] = 255;
	m_ucSelectedHostColor[2] = 0;
	m_ucSelectedHostColor[3] = 0;

	m_ucHostTextColor[0] = 200;
	m_ucHostTextColor[1] = 0;
	m_ucHostTextColor[2] = 0;
	m_ucHostTextColor[3] = 0;

	m_ucWarningHostColor[0] = 200;
	m_ucWarningHostColor[1] = 200;
	m_ucWarningHostColor[2] = 0;
	m_ucWarningHostColor[3] = 0;
	
	m_ucAlarmHostColor[0] = 220;
	m_ucAlarmHostColor[1] = 0;
	m_ucAlarmHostColor[2] = 0;
	m_ucAlarmHostColor[3] = 0;

	m_ucSubnetColor[0] = 0;
	m_ucSubnetColor[1] = 0;
	m_ucSubnetColor[2] = 127;
	m_ucSubnetColor[3] = 0;
	m_ucSelectedSubnetColor[0] = 0;
	m_ucSelectedSubnetColor[1] = 0;
	m_ucSelectedSubnetColor[2] = 255;
	m_ucSelectedSubnetColor[3] = 0;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CNetworkAnalyzerDoc),
		RUNTIME_CLASS(CNetworkAnalyzerFrame),       // main SDI frame window
		RUNTIME_CLASS(CNetworkAnalyzerView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	CGuiCreatedVisitor v(this);
	VisitAllDocs(&v, TRUE);

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	IDatabaseAPI *pDatabase = OpenDatabase();
	if (!m_bContinueWithoutDB)
		return FALSE;
	
	CWaitCursor cursor;

	m_pNetwork = new CNetwork(this, pDatabase);
	m_pIdleThread = new CIdleThread(this);
	m_pIdleThread->Run();
	m_pMainWnd->SetTimer(1, 1000, NULL);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkAnalyzerApp message handlers

//virtual
BOOL CNetworkAnalyzerApp::OnIdle(LONG lCount) 
{
	return CWinApp::OnIdle(lCount);
//	return IdleProcess();
}

int CNetworkAnalyzerApp::ExitInstance() 
{
	CStackCriticalSection section(&m_CriticalSection);
	if (m_pIdleThread)
	{
		m_pIdleThread->Stop();
		delete m_pIdleThread;
	}
	m_pIdleThread = NULL;

	if (m_pNetwork)
		delete m_pNetwork;
	m_pNetwork = NULL;

	if (m_pDatabase)
		delete m_pDatabase;
	m_pDatabase = NULL;

	return CWinApp::ExitInstance();
}

//virtual
IDatabaseAPI *CNetworkAnalyzerApp::OpenDatabase()
{
	// Add DB type chosing dialog (ODBC/DAO/etc) here
	m_bContinueWithoutDB = LOGICAL_TRUE;
	COdbcDatabase *pOdbcDatabase = new COdbcDatabase;
	if (pOdbcDatabase->Create())
	{
		m_pDatabase = pOdbcDatabase;
	}
	else
	{
		if (IDNO == MessageBox(NULL, "Database is not open.\nDo you want to continue w/o database?", "Error", MB_ICONEXCLAMATION | MB_YESNO))
		{
			m_bContinueWithoutDB = LOGICAL_FALSE;
		}

		delete pOdbcDatabase;
		m_pDatabase = NULL;
	}
	return m_pDatabase;
}

//virtual
void CNetworkAnalyzerApp::NewHostCreated(CHost *pHost)
{
	CNewHostCreatedVisitor v(pHost);
	VisitAllDocs(&v, TRUE);
}

//virtual
void CNetworkAnalyzerApp::HostChanged(CHost *pHost)
{
	CHostChangedVisitor v(pHost);
	VisitAllDocs(&v, TRUE);
}

//virtual
void CNetworkAnalyzerApp::NewSubnetCreated(CSubnet *pSubnet)
{
	CNewSubnetCreatedVisitor v(pSubnet);
	VisitAllDocs(&v, TRUE);
}

//virtual
void CNetworkAnalyzerApp::SubnetChanged(CSubnet *pSubnet)
{
	CSubnetChangedVisitor v(pSubnet);
	VisitAllDocs(&v, TRUE);
}

//virtual
void CNetworkAnalyzerApp::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	CHostAddedToSubnetVisitor v(pHost, pSubnet);
	VisitAllDocs(&v, TRUE);
}

//virtual
void CNetworkAnalyzerApp::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
	CHostRemovedFromSubnetVisitor v(pHost, pSubnet);
	VisitAllDocs(&v, TRUE);
}

//virtual
void CNetworkAnalyzerApp::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	TRACE("Selection changed\n");
	CObjectSelectionChangedVisitor v(pObject, pWhereChanged);
	VisitAllDocs(&v, FALSE);
}

//virtual
LOGICAL CNetworkAnalyzerApp::HostContextMenu(int ScreenX, int ScreenY, CHost *pHost)
{
	m_pHostContextMenu = pHost;
	CMenu menu;
	BOOL b1 = menu.CreatePopupMenu();
	BOOL b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_HOST_CONFIGURE, "Configure");
	b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_HOST_DELETE, "Delete");
	b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_HOST_LOOK_TRAFFIC, "Traffic");
	BOOL b3 = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, ScreenX, ScreenY, m_pMainWnd, NULL);
	menu.DestroyMenu();
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CNetworkAnalyzerApp::SubnetContextMenu(int ScreenX, int ScreenY, CSubnet *pSubnet)
{
	m_pSubnetContextMenu = pSubnet;

	CMenu menu;
	BOOL b1 = menu.CreatePopupMenu();
	BOOL b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_SUBNET_CONFIGURE, "Configure");
	b2 = menu.InsertMenu(-1, MF_STRING | MF_BYCOMMAND, ID_SUBNET_DELETE, "Delete");
	BOOL b3 = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, ScreenX, ScreenY, m_pMainWnd, NULL);

	menu.DestroyMenu();

	return LOGICAL_TRUE;
}

unsigned long CNetworkAnalyzerApp::VisitAllDocs(CDocVisitorAPI *pVisitor, BOOL bBlock)
{
	if (bBlock)
		CStackCriticalSection section(&m_CriticalSection);

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

BOOL CNetworkAnalyzerApp::IdleProcess()
{
//	TRACE("Idle process\n");
	COnIdleVisitor v;
	VisitAllDocs(&v, TRUE);
	return (v.IsToBeContinued());
}

// App command to run the dialog
void CNetworkAnalyzerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CNetworkAnalyzerApp::OnHostDelete() 
{
	// TODO: Add your command handler code here
	
}

void CNetworkAnalyzerApp::OnUpdateHostDelete(CCmdUI *pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CNetworkAnalyzerApp::OnHostConfigure() 
{
	CHostConfigurator configurator(m_pHostContextMenu, "Host configurator", m_pMainWnd);
	if (configurator.DoModal() == IDOK)
	{
		HostChanged(m_pHostContextMenu);
		if (m_pDatabase)
			m_pDatabase->HostChanged(m_pHostContextMenu);
	}
	UpdateWholeGUI();
}

void CNetworkAnalyzerApp::OnHostLookTraffic() 
{
	CHostTrafficDialog dlg;
	dlg.DoModal();
}

void CNetworkAnalyzerApp::OnUpdateHostConfigure(CCmdUI *pCmdUI) 
{
}

void CNetworkAnalyzerApp::OnSubnetConfigure() 
{
	CSubnetConfigurator configurator(m_pSubnetContextMenu, "Subnet configurator", m_pMainWnd);
	if (configurator.DoModal() == IDOK)
	{
		SubnetChanged(m_pSubnetContextMenu);
		if (m_pDatabase)
			m_pDatabase->SubnetChanged(m_pSubnetContextMenu);
	}
	UpdateWholeGUI();
}

void CNetworkAnalyzerApp::OnUpdateSubnetConfigure(CCmdUI *pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CNetworkAnalyzerApp::OnSubnetDelete() 
{
	// TODO: Add your command handler code here
	
}

void CNetworkAnalyzerApp::OnUpdateSubnetDelete(CCmdUI *pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CNetworkAnalyzerApp::UpdateWholeGUI()
{
	CUpdateGuiVisitor v;
	VisitAllDocs(&v, TRUE);
}

