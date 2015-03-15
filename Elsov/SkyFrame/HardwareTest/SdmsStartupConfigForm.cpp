// SdmsStartupConfigForm.cpp : implementation file
//

#include "stdafx.h"
#include "SdmsStartupConfigForm.h"
#include "Attached.h"
#include "NetworkMisc.h"
#include "MiscSdmsStartupConfig.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSdmsStartupConfigForm

IMPLEMENT_DYNCREATE(CSdmsStartupConfigForm, CAbstractForm)

CSdmsStartupConfigForm::CSdmsStartupConfigForm()
	: CAbstractForm(CSdmsStartupConfigForm::IDD)
{
	//{{AFX_DATA_INIT(CSdmsStartupConfigForm)
	m_StartupConfig = _T("");
	//}}AFX_DATA_INIT
	m_pSDMS = NULL;
}

CSdmsStartupConfigForm::~CSdmsStartupConfigForm()
{
}

void CSdmsStartupConfigForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSdmsStartupConfigForm)
	DDX_Control(pDX, IDC_STARTUP_CONFIG_EDIT, m_StartupConfigCtrl);
	DDX_Text(pDX, IDC_STARTUP_CONFIG_EDIT, m_StartupConfig);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSdmsStartupConfigForm, CAbstractForm)
	//{{AFX_MSG_MAP(CSdmsStartupConfigForm)
	ON_BN_CLICKED(IDC_GET_CURRENT_CONFIG_BUTTON, OnGetCurrentConfigButton)
	ON_BN_CLICKED(IDC_LOAD_FROM_FILE_BUTTON, OnLoadFromFileButton)
	ON_BN_CLICKED(IDC_SAVE_CONFIG_TO_FILE_BUTTON, OnSaveConfigToFileButton)
	ON_BN_CLICKED(IDC_SET_STARTUP_CONFIG_BUTTON, OnSetStartupConfigButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSdmsStartupConfigForm diagnostics

#ifdef _DEBUG
void CSdmsStartupConfigForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CSdmsStartupConfigForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

//virtual
BOOL CSdmsStartupConfigForm::SetupControls()
{
	m_pSDMS = (CSdmsAPI *)m_pAttached->m_pDevice;
	OnGetCurrentConfigButton();

	return TRUE;
}

//virtual
BOOL CSdmsStartupConfigForm::BeforeHiding()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSdmsStartupConfigForm message handlers


void CSdmsStartupConfigForm::OnGetCurrentConfigButton() 
{
	m_StartupConfig = Misc_GetSdmsStartupConfig(m_pSDMS);
	UpdateData(FALSE);
}

void CSdmsStartupConfigForm::OnLoadFromFileButton() 
{
	Misc_LoadStartupConfigFromFile(m_StartupConfig);
	UpdateData(FALSE);	
}

void CSdmsStartupConfigForm::OnSaveConfigToFileButton() 
{
	UpdateData();
	Misc_SaveStartupConfigToFile(m_StartupConfig);
}

void CSdmsStartupConfigForm::OnSetStartupConfigButton() 
{
	CWaitCursor cursor;
	UpdateData(TRUE);
	Misc_SetStartupConfig(m_StartupConfigCtrl, m_pSDMS);
	OnGetCurrentConfigButton();
}

