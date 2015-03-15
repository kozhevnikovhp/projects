// IpHelperTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IpHelperTest.h"
#include "IpHelperTestDlg.h"
#include "AddNewIpAddressDialog.h"
#include "IcmpSockets.h"

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
// CIpHelperTestDlg dialog

CIpHelperTestDlg::CIpHelperTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIpHelperTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIpHelperTestDlg)
	m_bLocalAddress = FALSE;
	m_MacAddress = _T("");
	m_InterfaceDesc = _T("");
	m_AdapterName = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIpHelperTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIpHelperTestDlg)
	DDX_Control(pDX, IDC_ADAPTERS_INDEX_COMBO, m_AdaptersIndexCombo);
	DDX_Control(pDX, IDC_PING_IPADDRESS, m_PingIpAddressCtrl);
	DDX_Control(pDX, IDC_IPADDRESS_SUBNET_MASK, m_SubnetMaskCtrl);
	DDX_Control(pDX, IDC_IPADDRESS_ADDRESS, m_IpAddressCtrl);
	DDX_Control(pDX, IDC_IP_ADDRESS_INDEX_COMBO, m_IpAddressIndexCombo);
	DDX_Check(pDX, IDC_LOCAL_ADDRESS_CHECK, m_bLocalAddress);
	DDX_Text(pDX, IDC_MAC_ADDRESS_EDIT, m_MacAddress);
	DDX_Text(pDX, IDC_INTERFACE_DESC_EDIT, m_InterfaceDesc);
	DDX_Text(pDX, IDC_ADAPTER_NAME_EDIT, m_AdapterName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIpHelperTestDlg, CDialog)
	//{{AFX_MSG_MAP(CIpHelperTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_INITIALIZE_BUTTON, OnInitializeButton)
	ON_CBN_SELCHANGE(IDC_IP_ADDRESS_INDEX_COMBO, OnSelChangeIpAddressIndexCombo)
	ON_BN_CLICKED(IDC_RUN_PING_BUTTON, OnRunPingButton)
	ON_BN_CLICKED(IDC_ADD_IPADDRESS_BUTTON, OnAddIpAddressButton)
	ON_CBN_SELCHANGE(IDC_ADAPTERS_INDEX_COMBO, OnSelChangeAdaptersIndexCombo)
	ON_BN_CLICKED(IDC_ABOUT_BUTTON, OnAboutButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIpHelperTestDlg message handlers

BOOL CIpHelperTestDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	OnInitializeButton();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIpHelperTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CIpHelperTestDlg::OnPaint() 
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
HCURSOR CIpHelperTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CIpHelperTestDlg::OnInitializeButton() 
{
	CWaitCursor cursor;
	CString str;
	m_IpAddressIndexCombo.ResetContent();
	for (unsigned int i = 0; i < m_Helper.GetIpAddressesCount(); i++)
	{
		str.Format("%d", i+1);
		int index = m_IpAddressIndexCombo.AddString(str);
		m_IpAddressIndexCombo.SetItemData(index, i);		
	}
	m_IpAddressIndexCombo.SetCurSel(0);
	OnSelChangeIpAddressIndexCombo();

	m_AdaptersIndexCombo.ResetContent();
	for (i = 0; i < m_Helper.GetAdaptersCount(); i++)
	{
		str = m_Helper.GetAdapterDescription(i);
		int index = m_AdaptersIndexCombo.AddString(str);
		m_AdaptersIndexCombo.SetItemData(index, i);		
	}
	m_AdaptersIndexCombo.SetCurSel(0);
	OnSelChangeAdaptersIndexCombo();
}

void CIpHelperTestDlg::OnSelChangeIpAddressIndexCombo() 
{
	static char pszBuffer[32];
	int index = m_IpAddressIndexCombo.GetCurSel();
	if (index == CB_ERR)
		return;
	unsigned int i = m_IpAddressIndexCombo.GetItemData(index);
	m_bLocalAddress = m_Helper.IsIpAddressLocal(i);
	DWORD IpAddress = m_Helper.GetIpAddress(i);
	m_IpAddressCtrl.SetAddress(LOBYTE(LOWORD(IpAddress)), HIBYTE(LOWORD(IpAddress)), LOBYTE(HIWORD(IpAddress)), HIBYTE(HIWORD(IpAddress)));
	DWORD SubnetMask = m_Helper.GetIpSubnetMask(i);
	m_SubnetMaskCtrl.SetAddress(LOBYTE(LOWORD(SubnetMask)), HIBYTE(LOWORD(SubnetMask)), LOBYTE(HIWORD(SubnetMask)), HIBYTE(HIWORD(SubnetMask)));

	unsigned long Iface = m_Helper.GetIfaceIpAddressIndex(i);
	m_InterfaceDesc = m_Helper.GetIfaceDesc(Iface);
	m_Helper.GetIfacePhysAddressStr(Iface, pszBuffer);
	m_MacAddress = pszBuffer;
	UpdateData(FALSE);
}

void CIpHelperTestDlg::OnRunPingButton() 
{
	CWaitCursor cursor;
	UpdateData();
	CIcmpSocket ping;
	ping.SetPingTimeout(1000);
	CString ResultString;
	unsigned char b1, b2, b3, b4;
	m_PingIpAddressCtrl.GetAddress(b1, b2, b3, b4);
	DWORD DestIP = MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4));

	m_IpAddressCtrl.GetAddress(b1, b2, b3, b4);
	DWORD LocalIfaceIP = MAKELONG(MAKEWORD(b1, b2), MAKEWORD(b3, b4));
	if (!ping.Bind(0, LocalIfaceIP))
	{
		MessageBox("Cannot set local interface!", "Bind error", MB_ICONSTOP);
		return;
	}

	for (int i = 1; i <= 3; i++)
	{
		CString local;
		local.Format("Echo request %d ", i);
		ResultString += local;
		ping.SendEchoRequest(DestIP);
		ping.ReceiveEchoReply();
		if (ping.IsTimedOut())
		{
			ResultString += "timed out!";
		}
		else if (ping.IsBad())
		{
			ResultString += "damaged (reply is bad)";
		}
		else
		{ //OK
			DWORD delay = ping.GetDelayTime();
			local.Format("OK (delay %d msec)", delay);
			ResultString += local;
		}
		ResultString += "\n";

		Sleep(500);
	}
	MessageBox(ResultString, "Ping report", MB_ICONINFORMATION);
}

void CIpHelperTestDlg::OnAddIpAddressButton() 
{
	int index = m_AdaptersIndexCombo.GetCurSel();
	if (index == CB_ERR)
		return;
	unsigned int nAdapter = m_AdaptersIndexCombo.GetItemData(index);
	
	CAddNewIpAddressDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;
	DWORD IpAddress = dlg.GetIpAddress();
	DWORD SubnetMask = dlg.GetIpSubnetMask();

	m_Helper.AddAdapterIpAddress(nAdapter, IpAddress, SubnetMask);
}

void CIpHelperTestDlg::OnSelChangeAdaptersIndexCombo() 
{
	int index = m_AdaptersIndexCombo.GetCurSel();
	if (index == CB_ERR)
		return;
	unsigned int i = m_AdaptersIndexCombo.GetItemData(index);
	m_AdapterName = m_Helper.GetAdapterName(i);
	UpdateData(FALSE);
}

void CIpHelperTestDlg::OnAboutButton() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}
