// TelnetForm.cpp : implementation file
//

#include "stdafx.h"
#include "ArmHub.h"
#include "TelnetForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"
#include "StationsView.h"

/////////////////////////////////////////////////////////////////////////////
// CTelnetForm

IMPLEMENT_DYNCREATE(CTelnetForm, CAbstractForm)

CTelnetForm::CTelnetForm()
	: CAbstractForm(CTelnetForm::IDD)
{
	//{{AFX_DATA_INIT(CTelnetForm)
	m_Command = _T("");
	m_Telnet = _T("");
	//}}AFX_DATA_INIT
}

CTelnetForm::~CTelnetForm()
{
}

void CTelnetForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTelnetForm)
	DDX_CBString(pDX, IDC_COMMAND_COMBO, m_Command);
	DDX_Text(pDX, IDC_TELNET_EDIT, m_Telnet);
	//}}AFX_DATA_MAP
}

//virtual
BOOL CTelnetForm::DoTimer()
{
	if (!m_pStation->TelnetTextChanged())
		return TRUE;
	UpdateData(TRUE);
	m_Telnet = m_pStation->GetTextFromTelnet();
	UpdateData(FALSE);
	return TRUE;
}

//virtual
BOOL CTelnetForm::SetupControls()
{
	m_pTelnetSocket = m_pStation->GetTelnetSocket();
	m_Telnet = m_pStation->GetTextFromTelnet();
	UpdateData(FALSE);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CTelnetForm, CAbstractForm)
	//{{AFX_MSG_MAP(CTelnetForm)
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, OnConnectButton)
	ON_BN_CLICKED(IDC_DISCONNECT_BUTTON, OnDisconnectButton)
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTelnetForm diagnostics

#ifdef _DEBUG
void CTelnetForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CTelnetForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTelnetForm message handlers

void CTelnetForm::OnConnectButton() 
{
	unsigned long IpAddress = m_pStation->GetIpAddressOfSP();
	if (!m_pTelnetSocket->Connect(IpAddress))
		MessageBox("Not connected - try again, please", "Error", MB_ICONEXCLAMATION);
}

void CTelnetForm::OnDisconnectButton() 
{
	m_pTelnetSocket->Disconnect();
}

void CTelnetForm::OnRunButton() 
{
	UpdateData(TRUE);
	static char buf[2048];
	strcpy(buf, (LPCTSTR)m_Command);
	strcat(buf, "\r\n");
	unsigned int nBytes;
	m_pTelnetSocket->Write(buf, strlen(buf), nBytes);
}
