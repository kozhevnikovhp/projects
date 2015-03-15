// FrequencyCounterForm.cpp : implementation file
//

#include "stdafx.h"
#include "FrequencyCounterForm.h"
#include "FrequencyCounter.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrequencyCounterForm

IMPLEMENT_DYNCREATE(CFrequencyCounterForm, CAbstractForm)

CFrequencyCounterForm::CFrequencyCounterForm()
	: CAbstractForm(CFrequencyCounterForm::IDD)
{
	//{{AFX_DATA_INIT(CFrequencyCounterForm)
	m_HeaderString = _T("");
	m_Frequency = _T("");
	//}}AFX_DATA_INIT
}

CFrequencyCounterForm::~CFrequencyCounterForm()
{
}

void CFrequencyCounterForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrequencyCounterForm)
	DDX_Text(pDX, IDC_HEADER_STATIC, m_HeaderString);
	DDX_Text(pDX, IDC_MEASUREMENT_EDIT, m_Frequency);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFrequencyCounterForm, CAbstractForm)
	//{{AFX_MSG_MAP(CFrequencyCounterForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPowerMeterForm diagnostics

#ifdef _DEBUG
void CFrequencyCounterForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CFrequencyCounterForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CFrequencyCounterForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pFrequencyCounter = (CFrequencyCounter *)pDevice;

	// Header string
	m_HeaderString.Format("Periodic measurements of frequency (channel %c) are displayed here", 'A' + (char)m_DeviceNumber - 1);

	UpdateAllControls();

	return TRUE;
}

BOOL CFrequencyCounterForm::DoTimer()
{
	UpdateAllControls();
	return TRUE;
}

void CFrequencyCounterForm::UpdateAllControls()
{
	double Frequency = 0;
	MC_ErrorCode EC = m_pFrequencyCounter->MeasureFrequency(Frequency, m_DeviceNumber);
	if (EC == MC_OK)
		m_Frequency.Format("%.3lf", Frequency/1000);
	else
		m_Frequency = "Error!";
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CFrequencyCounterForm message handlers

