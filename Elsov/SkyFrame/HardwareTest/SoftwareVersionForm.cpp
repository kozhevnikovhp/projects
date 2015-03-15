// SdmsSoftwareVersionForm.cpp : implementation file
//

#include "stdafx.h"
#include "SoftwareVersionForm.h"
#include "Attached.h"
#include "NetworkMisc.h"
#include "Misc_SoftwareVersions.h"
#include "MiscEthernet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSdmsSoftwareVersionForm

IMPLEMENT_DYNCREATE(CSdmsSoftwareVersionForm, CAbstractForm)

CSdmsSoftwareVersionForm::CSdmsSoftwareVersionForm()
	: CAbstractForm(CSdmsSoftwareVersionForm::IDD)
{
	//{{AFX_DATA_INIT(CSdmsSoftwareVersionForm)
	m_SoftwareVersion = _T("");
	//}}AFX_DATA_INIT
	m_pSDMS = NULL;
}

CSdmsSoftwareVersionForm::~CSdmsSoftwareVersionForm()
{
}

void CSdmsSoftwareVersionForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdmsSoftwareVersionForm)
	DDX_Text(pDX, IDC_SOFTWARE_VERSION_EDIT, m_SoftwareVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdmsSoftwareVersionForm, CAbstractForm)
	//{{AFX_MSG_MAP(CSdmsSoftwareVersionForm)
	ON_BN_CLICKED(IDC_SETUP_NEW_VERSION_BUTTON, OnSetupNewVersionButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdmsSoftwareVersionForm diagnostics

#ifdef _DEBUG
void CSdmsSoftwareVersionForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CSdmsSoftwareVersionForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

//virtual
BOOL CSdmsSoftwareVersionForm::SetupControls()
{
	m_pSDMS = (CSdmsAPI *)m_pAttached->m_pDevice;
	char *pszString;
	MC_ErrorCode EC = m_pSDMS->GetDeviceVersionString(pszString);
	m_SoftwareVersion = pszString;
	UpdateData(FALSE);

	return TRUE;
}

//virtual
BOOL CSdmsSoftwareVersionForm::BeforeHiding()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSdmsSoftwareVersionForm message handlers


void CSdmsSoftwareVersionForm::OnSetupNewVersionButton() 
{
	CFileDialog fd(TRUE, _T("bin"), NULL, 0,
		_T("Файлы прошивок (*bin)|*bin|Все файлы (*.*)|*.*||"));
	if (fd.DoModal()==IDCANCEL) return;
	CString SoftwareFileName = fd.GetPathName();
	
	CWaitCursor cursor;

	// find IP-address of ftp-server
	unsigned long SdmsIpAddress, SdmsIpSubnetMask;
	BOOL bSuccess = MiscGetSdmsEthernetIP(m_pSDMS, SdmsIpAddress, SdmsIpSubnetMask);
	if (!bSuccess)
		return;
	
	char szFtpServerIpAddress[32];
	bSuccess = MiscFindIpAddressForSDMS(SdmsIpAddress, SdmsIpSubnetMask, szFtpServerIpAddress);
	if (!bSuccess)
		return;
	
	bSuccess = Misc_SetNewSdmsSoftware(m_pSDMS, SoftwareFileName, szFtpServerIpAddress);
	if (!bSuccess)
		return;
	SetupControls();
}
