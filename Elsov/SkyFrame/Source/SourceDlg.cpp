// SourceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Source.h"
#include "SourceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSourceDlg dialog

CSourceDlg::CSourceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSourceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSourceDlg)
	m_Address = _T("");
	m_DateRate = 0;
	m_PacketSize = 0;
	m_PortNo = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSourceDlg)
	DDX_Text(pDX, IDC_ADDRESS_EDIT, m_Address);
	DDX_Text(pDX, IDC_DATARATE_EDIT, m_DateRate);
	DDX_Text(pDX, IDC_PACKET_SIZE_EDIT, m_PacketSize);
	DDX_Text(pDX, IDC_PORT_NO_EDIT, m_PortNo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSourceDlg, CDialog)
	//{{AFX_MSG_MAP(CSourceDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSourceDlg message handlers

BOOL CSourceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSourceDlg::OnPaint() 
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
HCURSOR CSourceDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSourceDlg::OnApplyButton() 
{
	UpdateData();
	if (m_DataRate==0)
	{ // only one datagram
	}
	else
	{
		int DeltaTime=8*1000*m_PacketSize/m_DataRate;
	}
}
