// IPRoutingTableForm.cpp : implementation file
//

#include "stdafx.h"
#include "IPRoutingTableForm.h"
#include "Attached.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPRoutingTableForm

IMPLEMENT_DYNCREATE(CIPRoutingTableForm, CAbstractForm)

CIPRoutingTableForm::CIPRoutingTableForm()
	: CAbstractForm(CIPRoutingTableForm::IDD)
{
	//{{AFX_DATA_INIT(CIPRoutingTableForm)
	m_TableString = _T("");
	m_DefaultGW = _T("");
	//}}AFX_DATA_INIT
	m_pNsgFrad=NULL;
}

CIPRoutingTableForm::~CIPRoutingTableForm()
{
}

void CIPRoutingTableForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPRoutingTableForm)
	DDX_Text(pDX, IDC_TABLE_EDIT, m_TableString);
	DDX_Text(pDX, IDC_DEFAULT_GW_EDIT, m_DefaultGW);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPRoutingTableForm, CAbstractForm)
	//{{AFX_MSG_MAP(CIPRoutingTableForm)
	ON_BN_CLICKED(IDC_SET_DEFAUL_GATEWAY_BUTTON, OnSetDefaulGatewayButton)
	ON_BN_CLICKED(IDC_WARM_BUTTON, OnWarmButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPRoutingTableForm diagnostics

#ifdef _DEBUG
void CIPRoutingTableForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CIPRoutingTableForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CIPRoutingTableForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pNsgFrad=(CNsgFrad *)pDevice;

	RefreshTable();
	static char buffer[32]; buffer[0]=0;
	static char tmp_buffer[32]; tmp_buffer[0]=0;
	char *pszDefaultGWString=strstr(LPCTSTR(m_TableString), "Default Route");
	if (pszDefaultGWString)
	{
		pszDefaultGWString=strstr(pszDefaultGWString, "use");
		if (pszDefaultGWString)
		{
			sscanf(pszDefaultGWString, "use %s %s %s",
				tmp_buffer, tmp_buffer, buffer);
			m_DefaultGW=buffer;
		}
	}
	UpdateData(FALSE);
	return TRUE;
}

void CIPRoutingTableForm::RefreshTable()
{
	UpdateData();
	CWaitCursor cursor;
	static char buffer[32]; buffer[0]=0;
	static char tmp_buffer[32]; tmp_buffer[0]=0;
	char *pszTableString;
	m_pNsgFrad->GetIPRoutingTable(pszTableString);
	m_TableString=pszTableString;
	UpdateData(FALSE);
}

BOOL CIPRoutingTableForm::DoTimer()
{
	RefreshTable();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CIPRoutingTableForm message handlers

void CIPRoutingTableForm::OnSetDefaulGatewayButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_pNsgFrad->SetIPDefaultGateway((char *)LPCTSTR(m_DefaultGW));
}

void CIPRoutingTableForm::OnWarmButton() 
{
	CWaitCursor cursor;
	m_pNsgFrad->IPInterfaceWarmStart(0);
	RefreshTable();
}
