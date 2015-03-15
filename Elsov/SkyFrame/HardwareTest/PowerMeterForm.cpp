// PowerMeterForm.cpp : implementation file
//

#include "stdafx.h"
#include "PowerMeterForm.h"
#include "PowerMeter.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPowerMeterForm

IMPLEMENT_DYNCREATE(CPowerMeterForm, CAbstractForm)

CPowerMeterForm::CPowerMeterForm()
	: CAbstractForm(CPowerMeterForm::IDD)
{
	//{{AFX_DATA_INIT(CPowerMeterForm)
	m_HeaderString = _T("");
	m_Power = _T("");
	//}}AFX_DATA_INIT
}

CPowerMeterForm::~CPowerMeterForm()
{
}

void CPowerMeterForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPowerMeterForm)
	DDX_Text(pDX, IDC_HEADER_STATIC, m_HeaderString);
	DDX_Text(pDX, IDC_MEASUREMENT_EDIT, m_Power);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPowerMeterForm, CAbstractForm)
	//{{AFX_MSG_MAP(CPowerMeterForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPowerMeterForm diagnostics

#ifdef _DEBUG
void CPowerMeterForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CPowerMeterForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CPowerMeterForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pPowerMeter = (CPowerMeter *)pDevice;

	// Header string
	m_HeaderString.Format("Periodic measurements of power (channel %c) are displayed here", 'A' + (char)m_DeviceNumber - 1);

	UpdateAllControls();

	return TRUE;
}

BOOL CPowerMeterForm::DoTimer()
{
	UpdateAllControls();
	return TRUE;
}

void CPowerMeterForm::UpdateAllControls()
{
	double Power = 0;
	MC_ErrorCode EC = m_pPowerMeter->MeasurePower(Power, m_DeviceNumber);
	if (EC == MC_OK)
		m_Power.Format("%7.2lf", Power);
	else
		m_Power = "Error!";
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CPowerMeterForm message handlers

