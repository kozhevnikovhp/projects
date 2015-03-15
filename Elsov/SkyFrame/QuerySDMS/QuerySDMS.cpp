// QuerySDMS.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "QuerySDMS.h"

#include "QuerySDMSFrame.h"
#include "QuerySDMSDoc.h"
#include "QuerySDMSView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSApp

BEGIN_MESSAGE_MAP(CQuerySDMSApp, CWinApp)
	//{{AFX_MSG_MAP(CQuerySDMSApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSApp construction

CQuerySDMSApp::CQuerySDMSApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CQuerySDMSApp object

CQuerySDMSApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSApp initialization

BOOL CQuerySDMSApp::InitInstance()
{
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

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CQuerySDMSDoc),
		RUNTIME_CLASS(CQuerySDMSFrame),       // main SDI frame window
		RUNTIME_CLASS(CParamsForm));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	m_IpAddress = 0;
	m_pszName = NULL;
	m_bWriteEnabled = FALSE;
	for (int i = 1; i < __argc; i++)
	{
		switch (i)
		{
		case 1:	m_pszName = strdup(__targv[1]); break;
		case 2: 
			if (stricmp(__targv[2], "adm") == 0)
				m_bWriteEnabled = TRUE;
			break;
		}
	}

	struct  hostent *pHost = gethostbyname(m_pszName);
	if (pHost)
	{
		char *pIpAddress = pHost->h_addr;
		BYTE b1 = *(pIpAddress+0);
		BYTE b2 = *(pIpAddress+1);
		BYTE b3 = *(pIpAddress+2);
		BYTE b4 = *(pIpAddress+3);
		m_IpAddress = MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4));
	}
	else
	{
		CString str;
		str.Format("Имя '%s' не может быть разрешено в IP-адрес\n", m_pszName);
		MessageBox(NULL, str, "ERROR", MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if (!m_IpAddress || !m_pszName)
	{
		MessageBox(NULL, "Valid name and adm. priveleges needed in command line", "ERROR", MB_ICONEXCLAMATION);
		return FALSE;
	}


	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	POSITION pos = pDocTemplate->GetFirstDocPosition();
	CDocument *pDoc = pDocTemplate->GetNextDoc(pos);
	pDoc->SetTitle(m_pszName);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
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
void CQuerySDMSApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSApp message handlers


BOOL CQuerySDMSApp::OnIdle(LONG lCount) 
{
	m_pFrame->OnIdle();
	CWinApp::OnIdle(lCount);
	return 1;
}

int CQuerySDMSApp::ExitInstance() 
{
	if (m_pszName)
		free(m_pszName);
	return CWinApp::ExitInstance();
}
