// TmiUdpStatisticsForm.cpp : implementation file
//

#include "stdafx.h"
#include "ArmHub.h"
#include "TmiUdpStatisticsForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"

/////////////////////////////////////////////////////////////////////////////
// CTmiUdpStatisticsForm

IMPLEMENT_DYNCREATE(CTmiUdpStatisticsForm, CTmiStatisticsForm)

CTmiUdpStatisticsForm::CTmiUdpStatisticsForm()
	: CTmiStatisticsForm(CTmiUdpStatisticsForm::IDD)
{
	//{{AFX_DATA_INIT(CTmiUdpStatisticsForm)
	//}}AFX_DATA_INIT
}

CTmiUdpStatisticsForm::~CTmiUdpStatisticsForm()
{
}

void CTmiUdpStatisticsForm::DoDataExchange(CDataExchange* pDX)
{
	CTmiStatisticsForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiUdpStatisticsForm)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTmiUdpStatisticsForm, CTmiStatisticsForm)
	//{{AFX_MSG_MAP(CTmiUdpStatisticsForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiUdpStatisticsForm diagnostics

#ifdef _DEBUG
void CTmiUdpStatisticsForm::AssertValid() const
{
	CTmiStatisticsForm::AssertValid();
}

void CTmiUdpStatisticsForm::Dump(CDumpContext& dc) const
{
	CTmiStatisticsForm::Dump(dc);
}
#endif //_DEBUG

//virtual
CString CTmiUdpStatisticsForm::GetStatisticsText(BOOL bIncremental)
{
	return m_pStation->GetUdpStatisticsText(bIncremental);
}
/////////////////////////////////////////////////////////////////////////////
// CTmiUdpStatisticsForm message handlers


