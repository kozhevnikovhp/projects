/* GranSim2D.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	21 Jan 2008 - initial creation
	13 Oct 2008 - object selection changed
	16 Oct 2008 - particle types introduced
	13 Nov 2008 - materials
	10 Dec 2008 - "cannot simulate" diagnostics
	05 Jan 2009 - objects deletion
*/

#include "stdafx.h"
#include "GranSim2D.h"

#include "GranSim2DDoc.h"
#include "GranSim2DProjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGranSim2DApp

BEGIN_MESSAGE_MAP(CGranSim2DApp, CWinApp)
	//{{AFX_MSG_MAP(CGranSim2DApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DApp construction

CGranSim2DApp::CGranSim2DApp()
{
	m_pAppFrameWnd = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGranSim2DApp object

CGranSim2DApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DApp initialization

BOOL CGranSim2DApp::InitInstance()
{
	g_pGui = this;

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

	LoadStdProfileSettings(16);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGranSim2DDoc),
		RUNTIME_CLASS(CGranSim2DFrame),       // main SDI frame window
		RUNTIME_CLASS(CGranSim2DProjectView));
	AddDocTemplate(m_pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pAppFrameWnd = (CGranSim2DFrame *)m_pMainWnd;
	m_pAppFrameWnd->DesignCleaned();
	
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

//virtual
void CGranSim2DApp::DesignCleaned()
{
	if (m_pAppFrameWnd)
		m_pAppFrameWnd->DesignCleaned();
}

//virtual
void CGranSim2DApp::DesignChanged()
{
	POSITION pos = m_pDocTemplate->GetFirstDocPosition();
	CDocument *pDoc = m_pDocTemplate->GetNextDoc(pos);
	pDoc->SetModifiedFlag();
	m_pAppFrameWnd->DesignChanged();
}

//virtual
void CGranSim2DApp::ReflectingWallCreated(CReflectingWall *pWall, void *pWhere)
{
	m_pAppFrameWnd->ReflectingWallCreated(pWall, pWhere);
}

//virtual
void CGranSim2DApp::ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere)
{
	m_pAppFrameWnd->ReflectingRectangleCreated(pRectangle, pWhere);
}

//virtual
void CGranSim2DApp::MaterialCreated(CMaterial *pMaterial, void *pWhere)
{
	m_pAppFrameWnd->MaterialCreated(pMaterial, pWhere);
}

//virtual
void CGranSim2DApp::ParticleTypeCreated(CParticleType *pType, void *pWhere)
{
	m_pAppFrameWnd->ParticleTypeCreated(pType, pWhere);
}

//virtual
void CGranSim2DApp::ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere)
{
	m_pAppFrameWnd->ParticlePointSourceCreated(pSource, pWhere);
}

//virtual
void CGranSim2DApp::ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere)
{
	m_pAppFrameWnd->ParticleLinearSourceCreated(pSource, pWhere);
}

//virtual
void CGranSim2DApp::ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere)
{
	m_pAppFrameWnd->ObjectSelectionChanged(pObject, pWhere);
}

//virtual
void CGranSim2DApp::ObjectDeleted(CGranSim2DObject *pObject, void *pWhere)
{
	m_pAppFrameWnd->ObjectDeleted(pObject, pWhere);
}

//virtual
void CGranSim2DApp::CannotSimulate(const char *pszWhy)
{
	m_pAppFrameWnd->MessageBox(pszWhy, "Cannot simulate", MB_ICONSTOP);
}

/////////////////////////////////////////////////////////////////////////////
// CGranSim2DApp message handlers

void CGranSim2DApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CGranSim2DApp::OnIdle(LONG lCount) 
{
	CWinApp::OnIdle(lCount);
	return m_pAppFrameWnd->Idle();
}


///////////////////////////////////////////////////////////////////
// CAboutDlg

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


