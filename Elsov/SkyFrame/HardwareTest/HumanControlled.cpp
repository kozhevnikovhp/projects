#include "stdafx.h"
#include "HumanControlled.h"

//  HumanPowerMeter implementation

char *CHumanPowerMeter::m_pszName = "Operator-aided (hand-made) power meter";

CHumanPowerMeter::CHumanPowerMeter()
{
	int a = 0;
}

//virtual
CHumanPowerMeter::~CHumanPowerMeter()
{
}

//virtual
MC_ErrorCode CHumanPowerMeter::MeasurePower(double &Power, int Channel)
{
	CHumanPowerMeterDialog dlg;
	if (dlg.DoModal() != IDOK)
		return MC_COMMAND_NOT_SUPPORTED;
	Power = dlg.m_Power;
	return MC_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CHumanPowerMeterDialog dialog

CHumanPowerMeterDialog::CHumanPowerMeterDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CHumanPowerMeterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHumanPowerMeterDialog)
	m_Power = 0.0;
	//}}AFX_DATA_INIT
}

void CHumanPowerMeterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHumanPowerMeterDialog)
	DDX_Text(pDX, IDC_POWER_EDIT, m_Power);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHumanPowerMeterDialog, CDialog)
	//{{AFX_MSG_MAP(CHumanPowerMeterDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHumanPowerMeterDialog message handlers


//  CHumanFrequencyCounter implementation

char *CHumanFrequencyCounter::m_pszName = "Operator-aided (hand-made) frequency counter";

CHumanFrequencyCounter::CHumanFrequencyCounter()
{
	int a = 0;
}

//virtual
CHumanFrequencyCounter::~CHumanFrequencyCounter()
{
}

//virtual
MC_ErrorCode CHumanFrequencyCounter::MeasureFrequency(double &Frequency, int Channel)
{
	CHumanFrequencyCounterDialog dlg;
	if (dlg.DoModal() != IDOK)
		return MC_COMMAND_NOT_SUPPORTED;
	Frequency = dlg.m_Frequency;
	return MC_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CHumanFrequencyCounterDialog dialog

CHumanFrequencyCounterDialog::CHumanFrequencyCounterDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CHumanFrequencyCounterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHumanFrequencyCounterDialog)
	m_Frequency = 0.0;
	//}}AFX_DATA_INIT
}

void CHumanFrequencyCounterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHumanFrequencyCounterDialog)
	DDX_Text(pDX, IDC_FREQUENCY_EDIT, m_Frequency);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHumanFrequencyCounterDialog, CDialog)
	//{{AFX_MSG_MAP(CHumanFrequencyCounterDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHumanFrequencyCounterDialog message handlers
