// CommonTests.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CommonTests.h"
#include "Device.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommonTestsEthernetPage property page

IMPLEMENT_DYNCREATE(CCommonTestsEthernetPage, CAutoTestPropertyPage)

CCommonTestsEthernetPage::CCommonTestsEthernetPage()
	: CAutoTestPropertyPage(CCommonTestsEthernetPage::IDD)
{
	//{{AFX_DATA_INIT(CCommonTestsEthernetPage)
	m_Report = _T("");
	//}}AFX_DATA_INIT
}

CCommonTestsEthernetPage::~CCommonTestsEthernetPage()
{
}

void CCommonTestsEthernetPage::DoDataExchange(CDataExchange *pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommonTestsEthernetPage)
	DDX_Text(pDX, IDC_REPORT_EDIT, m_Report);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCommonTestsEthernetPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CCommonTestsEthernetPage)
	ON_BN_CLICKED(IDC_RUN_BUTTON, OnRunButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCommonTestsEthernetPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	
	return CAutoTestPropertyPage::OnSetActive();
}

void CCommonTestsEthernetPage::OnRunButton() 
{
	// Zhenya, uncomment it!!!
/*	CWaitCursor cursor;
	m_Report.Empty();
	CString str;
	DWORD IPAddress = m_pAutoTestConfig->m_pTestDevice->GetMcIpAddress();
	CIcmpSocket sock;
	int rcvd_bytes = 0;
	int sent = 0, rcvd = 0, bad = 0, timed_out = 0;
	int total_delay = 0;
	for (int i = 0; i<100; i++)
	{
		sock.SendEchoRequest(i*10, IPAddress);
		sent++;
		if (!sock.ReceiveEchoReply())
		{
			MessageBox("Error in echo reply reading!!!", "Error", MB_ICONEXCLAMATION);
			break;
		}

		if (sock.IsTimedOut())
		{
			timed_out++;
			str = "Timeout expired\r\n";
		}
		else if (sock.IsBad())
		{
			bad++;
			str = "Bad echo reply\r\n";
		}
		else
		{
			rcvd++;
			rcvd_bytes = sock.GetReceivedDataLength()+32;
			total_delay += sock.GetDelayTime();
			str.Format("%d bytes from %d.%d.%d.%d delay=%d msec\r\n", rcvd_bytes,
				LOBYTE(LOWORD(IPAddress)),
				HIBYTE(LOWORD(IPAddress)),
				LOBYTE(HIWORD(IPAddress)),
				HIBYTE(HIWORD(IPAddress)),
				total_delay);
		}
		m_Report+=str;
		UpdateData(FALSE);
		Sleep(100);
	}
	if (rcvd == 0)
		rcvd = 1;
	str.Format("\r\n%d packets sent, %d received, %d bad, %d timed out, delay = %d msec", sent, rcvd, bad, timed_out, total_delay/rcvd);
	m_Report+=str;*/
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CCommonTestsCongratulationsPage property page

IMPLEMENT_DYNCREATE(CCommonTestsCongratulationsPage, CAutoTestPropertyPage)

CCommonTestsCongratulationsPage::CCommonTestsCongratulationsPage()
	: CAutoTestPropertyPage(CCommonTestsCongratulationsPage::IDD)
{
	//{{AFX_DATA_INIT(CCommonTestsCongratulationsPage)
	//}}AFX_DATA_INIT
}

CCommonTestsCongratulationsPage::~CCommonTestsCongratulationsPage()
{
}

void CCommonTestsCongratulationsPage::DoDataExchange(CDataExchange* pDX)
{
	CAutoTestPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommonTestsCongratulationsPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCommonTestsCongratulationsPage, CAutoTestPropertyPage)
	//{{AFX_MSG_MAP(CCommonTestsCongratulationsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCommonTestsCongratulationsPage::OnSetActive() 
{
	CPropertySheet *pSheet = (CPropertySheet *)GetParent();   
	pSheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK | PSWIZB_FINISH);
	
	return CAutoTestPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CFrequencyDependentCorrectionDialog dialog

CFrequencyDependentCorrectionDialog::CFrequencyDependentCorrectionDialog(CWnd *pParent /*=NULL*/)
	: CDialog(CFrequencyDependentCorrectionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrequencyDependentCorrectionDialog)
	m_Correction1 = 0.0;
	m_Correction2 = 0.0;
	m_Freq1 = 0;
	m_Freq2 = 0;
	//}}AFX_DATA_INIT
}


void CFrequencyDependentCorrectionDialog::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrequencyDependentCorrectionDialog)
	DDX_Text(pDX, IDC_CORRECTION1_EDIT, m_Correction1);
	DDX_Text(pDX, IDC_CORRECTION2_EDIT, m_Correction2);
	DDX_Text(pDX, IDC_FREQUENCY1_EDIT, m_Freq1);
	DDX_Text(pDX, IDC_FREQUENCY2_EDIT, m_Freq2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrequencyDependentCorrectionDialog, CDialog)
	//{{AFX_MSG_MAP(CFrequencyDependentCorrectionDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrequencyDependentCorrectionDialog message handlers

void CFrequencyDependentCorrectionDialog::OnOK() 
{
	UpdateData();
	if (m_Freq1 == m_Freq2)
	{
		MessageBox("Please enter two DIFFERENT frequencies", pszError, MB_ICONEXCLAMATION);
		return;
	}
	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CCorrectionDialog dialog


CCorrectionDialog::CCorrectionDialog(CWnd *pParent /*=NULL*/)
	: CDialog(CCorrectionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCorrectionDialog)
	m_Correction = 0.0;
	//}}AFX_DATA_INIT
}


void CCorrectionDialog::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCorrectionDialog)
	DDX_Text(pDX, IDC_CORRECTION_EDIT, m_Correction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCorrectionDialog, CDialog)
	//{{AFX_MSG_MAP(CCorrectionDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCorrectionDialog message handlers
