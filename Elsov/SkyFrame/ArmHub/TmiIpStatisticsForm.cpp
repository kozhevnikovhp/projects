// TmiIpStatisticsForm.cpp : implementation file
//

#include "stdafx.h"
#include "ArmHub.h"
#include "TmiIpStatisticsForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"

/////////////////////////////////////////////////////////////////////////////
// CTmiIpStatisticsForm

IMPLEMENT_DYNCREATE(CTmiIpStatisticsForm, CTmiStatisticsForm)

CTmiIpStatisticsForm::CTmiIpStatisticsForm()
	: CTmiStatisticsForm(CTmiIpStatisticsForm::IDD)
{
	//{{AFX_DATA_INIT(CTmiIpStatisticsForm)
	//}}AFX_DATA_INIT
}

CTmiIpStatisticsForm::~CTmiIpStatisticsForm()
{
}

void CTmiIpStatisticsForm::DoDataExchange(CDataExchange* pDX)
{
	CTmiStatisticsForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiIpStatisticsForm)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTmiIpStatisticsForm, CTmiStatisticsForm)
	//{{AFX_MSG_MAP(CTmiIpStatisticsForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiIpStatisticsForm diagnostics

#ifdef _DEBUG
void CTmiIpStatisticsForm::AssertValid() const
{
	CTmiStatisticsForm::AssertValid();
}

void CTmiIpStatisticsForm::Dump(CDumpContext& dc) const
{
	CTmiStatisticsForm::Dump(dc);
}
#endif //_DEBUG

//virtual
CString CTmiIpStatisticsForm::GetStatisticsText
(
	BOOL bIncremental
)
{
	return m_pStation->GetIpStatisticsText(bIncremental);
}
/////////////////////////////////////////////////////////////////////////////
// CTmiIpStatisticsForm message handlers


