// IPStaticRoutesForm.cpp : implementation file
//

#include "stdafx.h"
#include "StaticRoutePropertiesDialog.h"
#include "IPStaticRoutesForm.h"
#include "Attached.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static unsigned char CR=0x0D;
static unsigned char LF=0x0A;

/////////////////////////////////////////////////////////////////////////////
// CIPStaticRoutesForm

IMPLEMENT_DYNCREATE(CIPStaticRoutesForm, CAbstractForm)

CIPStaticRoutesForm::CIPStaticRoutesForm()
	: CAbstractForm(CIPStaticRoutesForm::IDD)
{
	//{{AFX_DATA_INIT(CIPStaticRoutesForm)
	//}}AFX_DATA_INIT
	m_pNsgFrad=NULL;
}

CIPStaticRoutesForm::~CIPStaticRoutesForm()
{
}

void CIPStaticRoutesForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPStaticRoutesForm)
	DDX_Control(pDX, IDC_STATIC_ROUTES_LIST, m_StaticRoutesCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPStaticRoutesForm, CAbstractForm)
	//{{AFX_MSG_MAP(CIPStaticRoutesForm)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_MODIFY_BUTTON, OnModifyButton)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPStaticRoutesForm diagnostics

#ifdef _DEBUG
void CIPStaticRoutesForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CIPStaticRoutesForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CIPStaticRoutesForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	m_pNsgFrad=(CNsgFrad *)pDevice;

	RefreshTable();
	return TRUE;
}

void CIPStaticRoutesForm::RefreshTable()
{
	CWaitCursor cursor;
	m_StaticRoutesCtrl.DeleteAllItems();
	LVITEM lvitem;
	lvitem.mask=LVIF_TEXT;

	static char pszNet[32], pszMask[32], pszGateway[32], pszMetric[32];
	char *pszStaticRoutes;
	m_pNsgFrad->GetIPStaticRoutesString(pszStaticRoutes);

	char *pszDefaultRoute = NULL, *pszRoute = strstr(pszStaticRoutes, "metric");
	if (pszRoute)
		pszDefaultRoute = strstr(pszRoute, "Default"); // find second "metric"
	while (pszRoute)
	{
		// go forward until CRLF
		while (*pszRoute!=CR)
			pszRoute++;
		// now go forward until first digit
		while (!isdigit(*pszRoute))
		{
			pszRoute++;
			if (pszRoute>=pszDefaultRoute)
				break;
		}
		if (pszRoute>=pszDefaultRoute)
			break;
		if (sscanf(pszRoute, "%s %s %s %s", pszNet, pszMask, pszGateway, pszMetric)!=4)
			break; 

		int ItemIndex=m_StaticRoutesCtrl.GetItemCount();
		lvitem.iItem=m_StaticRoutesCtrl.InsertItem(ItemIndex, pszNet);
		lvitem.iSubItem=1;
		lvitem.pszText=pszMask;
		m_StaticRoutesCtrl.SetItem(&lvitem);
		lvitem.iSubItem=2;
		lvitem.pszText=pszGateway;
		m_StaticRoutesCtrl.SetItem(&lvitem);
		lvitem.iSubItem=3;
		lvitem.pszText=pszMetric;
		m_StaticRoutesCtrl.SetItem(&lvitem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CIPStaticRoutesForm message handlers

void CIPStaticRoutesForm::OnInitialUpdate() 
{
	CAbstractForm::OnInitialUpdate();
	CRect rect;
	m_StaticRoutesCtrl.GetWindowRect(&rect);
	m_StaticRoutesCtrl.InsertColumn(0, "net", LVCFMT_LEFT, rect.Width()/4);
	m_StaticRoutesCtrl.InsertColumn(1, "mask", LVCFMT_LEFT, rect.Width()/4);
	m_StaticRoutesCtrl.InsertColumn(2, "gateway", LVCFMT_LEFT, rect.Width()/4);
	m_StaticRoutesCtrl.InsertColumn(3, "metric", LVCFMT_LEFT, rect.Width()/8);
}

void CIPStaticRoutesForm::OnDeleteButton() 
{
	static char pszNet[32], pszMask[32];

	int index=m_StaticRoutesCtrl.GetSelectionMark();
	if (index==-1) return;

	m_StaticRoutesCtrl.GetItemText(index, 0, pszNet, sizeof(pszNet));
	m_StaticRoutesCtrl.GetItemText(index, 1, pszMask, sizeof(pszMask));
	CString Warning="Do yo really wish to delete static route for net:";
	Warning+=pszNet;
	Warning+=" and mask:";
	Warning+=pszMask;
	if (MessageBox(Warning+" ?", "Attention!!!", MB_ICONQUESTION | MB_YESNO)!=IDYES) return;
	m_pNsgFrad->DeleteIPStaticRoute(pszNet, pszMask);
	RefreshTable();
}

void CIPStaticRoutesForm::OnModifyButton() 
{
	static char pszNet[32], pszMask[32], pszGateway[32], pszMetric[32];
	static char pszDeletedNet[32], pszDeletedMask[32];
	int index=m_StaticRoutesCtrl.GetSelectionMark();
	if (index==-1) return;

	m_StaticRoutesCtrl.GetItemText(index, 0, pszNet, sizeof(pszNet));
	strcpy(pszDeletedNet, pszNet);
	m_StaticRoutesCtrl.GetItemText(index, 1, pszMask, sizeof(pszMask));
	strcpy(pszDeletedMask, pszMask);
	m_StaticRoutesCtrl.GetItemText(index, 2, pszGateway, sizeof(pszGateway));
	m_StaticRoutesCtrl.GetItemText(index, 3, pszMetric, sizeof(pszMetric));
	
	CStaticRoutePropertiesDialog dialog(pszNet, pszMask, pszGateway, pszMetric, this);
	if (dialog.DoModal()!=IDOK) return;

	m_pNsgFrad->DeleteIPStaticRoute(pszDeletedNet, pszDeletedMask);
	m_pNsgFrad->AddIPStaticRoute(pszNet, pszMask, pszGateway, pszMetric);
	RefreshTable();
}

void CIPStaticRoutesForm::OnAddButton() 
{
	static char pszNet[32], pszMask[32], pszGateway[32], pszMetric[32];
	pszNet[0]=pszMask[0]=pszGateway[0]=pszMetric[0]=0;
	CStaticRoutePropertiesDialog dialog(pszNet, pszMask, pszGateway, pszMetric, this);
	if (dialog.DoModal()!=IDOK) return;
	m_pNsgFrad->AddIPStaticRoute(pszNet, pszMask, pszGateway, pszMetric);

	RefreshTable();
}
