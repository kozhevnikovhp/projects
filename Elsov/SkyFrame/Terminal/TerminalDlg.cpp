// TerminalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Terminal.h"
#include "TerminalDlg.h"

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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalDlg dialog

static char CR=0x0D;
static char LF=0x0A;

CTerminalDlg::CTerminalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTerminalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerminalDlg)
	m_Command = _T("");
	m_Reply = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTerminalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerminalDlg)
	DDX_Text(pDX, IDC_COMMAND_EDIT, m_Command);
	DDX_Text(pDX, IDC_REPLY_EDIT, m_Reply);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTerminalDlg, CDialog)
	//{{AFX_MSG_MAP(CTerminalDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_BN_CLICKED(IDC_SENDCRLF_BUTTON, OnSendCRLFButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalDlg message handlers

BOOL CTerminalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CheckRadioButton(IDC_COM1, IDC_COM4, IDC_COM2);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTerminalDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTerminalDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTerminalDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTerminalDlg::OnOpen() 
{
	int PortNumber=GetCheckedRadioButton(IDC_COM1, IDC_COM4)-IDC_COM1+1;
	m_Port.Open(PortNumber);
}

void CTerminalDlg::OnClose() 
{
	m_Port.Close();	
}

void CTerminalDlg::OnSend() 
{
	CWaitCursor cursor;
	UpdateData(TRUE);
	char pszBuffer[256];
	strcpy(pszBuffer, LPCTSTR(m_Command));
	int len=strlen(pszBuffer);
	pszBuffer[len]=CR;
	pszBuffer[len+1]=LF;
	pszBuffer[len+2]=0;

	m_Reply.Empty();

	m_Port.ClearAllBuffers();
	if (!m_Port.Send(pszBuffer, len+1))
		return;

	char pszReadBuffer=0;
	while (1)
	{
		if (!m_Port.Read(&pszReadBuffer, 1)) break;
		if (pszReadBuffer!=0)
			m_Reply+=pszReadBuffer;
		if (pszReadBuffer=='>') break;
	}
	UpdateData(FALSE);

}

void CTerminalDlg::OnSendCRLFButton() 
{
	CWaitCursor cursor;
	UpdateData(TRUE);
	m_Reply.Empty();

	m_Port.ClearAllBuffers();
	if (!m_Port.Send(&CR, 1))
		return;
	if (!m_Port.Send(&LF, 1))
		return;

	char pszReadBuffer=0;
	while (1)
	{
		if (!m_Port.Read(&pszReadBuffer, 1)) break;
		if (pszReadBuffer!=0)
			m_Reply+=pszReadBuffer;
		if (pszReadBuffer=='>') break;
	}
	UpdateData(FALSE);

}
