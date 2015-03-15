// HardwareTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "HardwareTestFrame.h"
#include "HardwareTestDoc.h"
#include "HardwareTestView.h"
#include "Hyperlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	~CAboutDlg();
	BOOL ShowAtStartup() { return !m_bNeverShowAtStartup; }

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_EMailHyperlink;
	BOOL	m_bNeverShowAtStartup;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_bNeverShowAtStartup = FALSE;;
	//}}AFX_DATA_INIT
	m_bNeverShowAtStartup = AfxGetApp()->GetProfileInt("Startup", "NeverShowAboutBox", 0);
}

CAboutDlg::~CAboutDlg()
{
	AfxGetApp()->WriteProfileInt("Startup", "NeverShowAboutBox", m_bNeverShowAtStartup);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_EMAIL_HYPERLINK_STATIC, m_EMailHyperlink);
	DDX_Check(pDX, IDC_DONT_SHOW_AT_STARTUP_CHECK, m_bNeverShowAtStartup);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_EMailHyperlink.SetURL("mailto:ekozhevnikov@cts.ru; evgeny@kozhevnikov.org");	
	m_EMailHyperlink.SetUnderline(TRUE);
	m_EMailHyperlink.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_HAND_FOR_HYPERLINK));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestApp

BEGIN_MESSAGE_MAP(CHardwareTestApp, CWinApp)
	//{{AFX_MSG_MAP(CHardwareTestApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestApp construction

CHardwareTestApp::CHardwareTestApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHardwareTestApp object

CHardwareTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestApp initialization

BOOL CHardwareTestApp::InitInstance()
{
	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	SetRegistryKey(_T("Kozhevnikov"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CHardwareTestDoc),
		RUNTIME_CLASS(CHardwareTestFrame),       // main SDI frame window
		RUNTIME_CLASS(CHardwareTestView));
	AddDocTemplate(pDocTemplate);

	CAboutDlg dlg;
	if (dlg.ShowAtStartup())
		dlg.DoModal();

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

// App command to run the dialog
void CHardwareTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestApp message handlers

