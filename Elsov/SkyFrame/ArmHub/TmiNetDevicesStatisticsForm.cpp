// TmiNetDevicesStatisticsForm.cpp : implementation file
//

#include "stdafx.h"
#include "ArmHub.h"
#include "TmiNetDeviceStatisticsForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"

/////////////////////////////////////////////////////////////////////////////
// CTmiNetDeviceStatisticsForm

IMPLEMENT_DYNCREATE(CTmiNetDeviceStatisticsForm, CTmiStatisticsForm)

CTmiNetDeviceStatisticsForm::CTmiNetDeviceStatisticsForm()
	: CTmiStatisticsForm(CTmiNetDeviceStatisticsForm::IDD)
{
	//{{AFX_DATA_INIT(CTmiNetDeviceStatisticsForm)
	//}}AFX_DATA_INIT
}

CTmiNetDeviceStatisticsForm::~CTmiNetDeviceStatisticsForm()
{
}

void CTmiNetDeviceStatisticsForm::DoDataExchange(CDataExchange* pDX)
{
	CTmiStatisticsForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiNetDeviceStatisticsForm)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTmiNetDeviceStatisticsForm, CTmiStatisticsForm)
	//{{AFX_MSG_MAP(CTmiNetDeviceStatisticsForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiNetDeviceStatisticsForm diagnostics

#ifdef _DEBUG
void CTmiNetDeviceStatisticsForm::AssertValid() const
{
	CTmiStatisticsForm::AssertValid();
}

void CTmiNetDeviceStatisticsForm::Dump(CDumpContext& dc) const
{
	CTmiStatisticsForm::Dump(dc);
}
#endif //_DEBUG

//virtual
BOOL CTmiNetDeviceStatisticsForm::SetupControls()
{
	m_Tab.DeleteAllItems();
	for (int i=0; i<m_pStation->GetNetDeviceCount(); i++)
	{
		CString ItemText = m_pStation->GetNetDeviceName(i);
		m_Tab.InsertItem(i, ItemText);
	}
	
	return CTmiStatisticsForm::SetupControls();
}

//virtual
CString CTmiNetDeviceStatisticsForm::GetStatisticsText
(
	unsigned nTab,
	BOOL bIncremental
)
{
	int InterfaceNo = nTab;
	return m_pStation->GetNetDeviceStatisticsText(InterfaceNo, bIncremental);
}
/////////////////////////////////////////////////////////////////////////////
// CTmiNetDeviceStatisticsForm message handlers


