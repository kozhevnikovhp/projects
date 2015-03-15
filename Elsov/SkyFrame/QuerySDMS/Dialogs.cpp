// Dialogs.cpp : implementation file
//

#include "stdafx.h"
#include "querysdms.h"
#include "Dialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL ChangeUnsignedIntValue(unsigned int &value, unsigned int min_value, unsigned int max_value)
{
	CUnsignedIntDialog dlg;
	dlg.m_Value = value;
	dlg.m_Min = min_value;
	dlg.m_Max = max_value;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	value = dlg.m_Value;
	return TRUE;
}


BOOL ChangeIntValue(int &value, int min_value, int max_value)
{
	CIntDialog dlg;
	dlg.m_Value = value;
	dlg.m_Min = min_value;
	dlg.m_Max = max_value;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	value = dlg.m_Value;
	return TRUE;
}

BOOL ChangeDoubleValue(double &value, double min_value, double max_value)
{
	CDoubleDialog dlg;
	dlg.m_Value = value;
	dlg.m_Min = min_value;
	dlg.m_Max = max_value;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	value = dlg.m_Value;
	return TRUE;
}

BOOL ChangeModulation(BOOL &bBPSK)
{
	CModulationDialog dlg;
	dlg.m_bBPSK = bBPSK;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	bBPSK = dlg.m_bBPSK;
	return TRUE;
}

BOOL OnOff(BOOL &bOFF)
{
	COnOffDialog dlg;
	dlg.m_bOFF = bOFF;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	bOFF = dlg.m_bOFF;
	return TRUE;
}

BOOL ChangeSpectrum(BOOL &bNormal)
{
	CSpectrumDialog dlg;
	dlg.m_bNormal = bNormal;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	bNormal = dlg.m_bNormal;
	return TRUE;
}

BOOL ChangeCodeRate(int &CodeRate)
{
	CCodeRateDialog dlg;
	dlg.m_CodeRate = CodeRate;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	CodeRate = dlg.m_CodeRate;
	return TRUE;
}

BOOL ChangeScrambler(int &Scrambler)
{
	CScramblerDialog dlg;
	dlg.m_Scrambler = Scrambler;
	if (dlg.DoModal() != IDOK)
		return FALSE;
	Scrambler = dlg.m_Scrambler;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUnsignedIntDialog dialog


CUnsignedIntDialog::CUnsignedIntDialog(CWnd *pParent /*=NULL*/)
	: CDialog(CUnsignedIntDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUnsignedIntDialog)
	m_Value = 0;
	//}}AFX_DATA_INIT
}


void CUnsignedIntDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnsignedIntDialog)
	DDX_Control(pDX, IDC_VALUE_SPIN, m_Spin);
	DDX_Text(pDX, IDC_VALUE_EDIT, m_Value);
	DDV_MinMaxUInt(pDX, m_Value, m_Min, m_Max);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUnsignedIntDialog, CDialog)
	//{{AFX_MSG_MAP(CUnsignedIntDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnsignedIntDialog message handlers

BOOL CUnsignedIntDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Spin.SetBuddy(GetDescendantWindow(IDC_VALUE_EDIT));
	m_Spin.SetRange32(m_Min, m_Max);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CIntDialog dialog

CIntDialog::CIntDialog(CWnd *pParent /*=NULL*/)
	: CDialog(CIntDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIntDialog)
	m_Value = 0;
	//}}AFX_DATA_INIT
}


void CIntDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntDialog)
	DDX_Control(pDX, IDC_VALUE_SPIN, m_Spin);
	DDX_Text(pDX, IDC_VALUE_EDIT, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIntDialog, CDialog)
	//{{AFX_MSG_MAP(CIntDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntDialog message handlers

BOOL CIntDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Spin.SetBuddy(GetDescendantWindow(IDC_VALUE_EDIT));
	m_Spin.SetRange32(m_Min, m_Max);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CDoubleDialog dialog


CDoubleDialog::CDoubleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDoubleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDoubleDialog)
	m_Value = 0.0;
	//}}AFX_DATA_INIT
}


void CDoubleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoubleDialog)
	DDX_Control(pDX, IDC_VALUE_SPIN, m_Spin);
	DDX_Text(pDX, IDC_VALUE_EDIT, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDoubleDialog, CDialog)
	//{{AFX_MSG_MAP(CDoubleDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleDialog message handlers

BOOL CDoubleDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Spin.SetBuddy(GetDescendantWindow(IDC_VALUE_EDIT));
	m_Spin.SetRange32(m_Min, m_Max);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CModulationDialog dialog


CModulationDialog::CModulationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CModulationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModulationDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CModulationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModulationDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModulationDialog, CDialog)
	//{{AFX_MSG_MAP(CModulationDialog)
	ON_BN_CLICKED(IDC_BPSK_RADIO, OnBpskRadio)
	ON_BN_CLICKED(IDC_QPSK_RADIO, OnQpskRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModulationDialog message handlers

BOOL CModulationDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_bBPSK)
		CheckRadioButton(IDC_BPSK_RADIO, IDC_QPSK_RADIO, IDC_BPSK_RADIO);
	else
		CheckRadioButton(IDC_BPSK_RADIO, IDC_QPSK_RADIO, IDC_QPSK_RADIO);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModulationDialog::OnBpskRadio() 
{
	m_bBPSK = TRUE;
}

void CModulationDialog::OnQpskRadio() 
{
	m_bBPSK = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// COnOffDialog dialog

COnOffDialog::COnOffDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COnOffDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COnOffDialog)
	//}}AFX_DATA_INIT
}


void COnOffDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnOffDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COnOffDialog, CDialog)
	//{{AFX_MSG_MAP(COnOffDialog)
	ON_BN_CLICKED(IDC_OFF_RADIO, OnOffRadio)
	ON_BN_CLICKED(IDC_ON_RADIO, OnOnRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnOffDialog message handlers

BOOL COnOffDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_bOFF)
		CheckRadioButton(IDC_OFF_RADIO, IDC_ON_RADIO, IDC_OFF_RADIO);
	else
		CheckRadioButton(IDC_OFF_RADIO, IDC_ON_RADIO, IDC_ON_RADIO);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COnOffDialog::OnOffRadio() 
{
	m_bOFF = TRUE;	
}

void COnOffDialog::OnOnRadio() 
{
	m_bOFF = FALSE;	
}
/////////////////////////////////////////////////////////////////////////////
// CSpectrumDialog dialog


CSpectrumDialog::CSpectrumDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSpectrumDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpectrumDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSpectrumDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpectrumDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpectrumDialog, CDialog)
	//{{AFX_MSG_MAP(CSpectrumDialog)
	ON_BN_CLICKED(IDC_NORMAL_RADIO, OnNormalRadio)
	ON_BN_CLICKED(IDC_INVERTED_RADIO, OnInvertedRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpectrumDialog message handlers

void CSpectrumDialog::OnNormalRadio() 
{
	m_bNormal = TRUE;	
}

void CSpectrumDialog::OnInvertedRadio() 
{
	m_bNormal = FALSE;	
}

BOOL CSpectrumDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_bNormal)
		CheckRadioButton(IDC_NORMAL_RADIO, IDC_INVERTED_RADIO, IDC_NORMAL_RADIO);
	else
		CheckRadioButton(IDC_NORMAL_RADIO, IDC_INVERTED_RADIO, IDC_INVERTED_RADIO);

	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CCodeRateDialog dialog


CCodeRateDialog::CCodeRateDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCodeRateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCodeRateDialog)
	//}}AFX_DATA_INIT
}


void CCodeRateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodeRateDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCodeRateDialog, CDialog)
	//{{AFX_MSG_MAP(CCodeRateDialog)
	ON_BN_CLICKED(IDC_12_RADIO, On12Radio)
	ON_BN_CLICKED(IDC_34_RADIO, On34Radio)
	ON_BN_CLICKED(IDC_78_RADIO, On78Radio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodeRateDialog message handlers

void CCodeRateDialog::On12Radio() 
{
	m_CodeRate = 1;
}

void CCodeRateDialog::On34Radio() 
{
	m_CodeRate = 2;
}

void CCodeRateDialog::On78Radio() 
{
	m_CodeRate = 3;
}

BOOL CCodeRateDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_CodeRate == 1)
		CheckRadioButton(IDC_12_RADIO, IDC_78_RADIO, IDC_12_RADIO);
	else if (m_CodeRate == 2)
		CheckRadioButton(IDC_12_RADIO, IDC_78_RADIO, IDC_34_RADIO);
	else if (m_CodeRate == 3)
		CheckRadioButton(IDC_12_RADIO, IDC_78_RADIO, IDC_78_RADIO);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CScramblerDialog dialog


CScramblerDialog::CScramblerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScramblerDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScramblerDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CScramblerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScramblerDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScramblerDialog, CDialog)
	//{{AFX_MSG_MAP(CScramblerDialog)
	ON_BN_CLICKED(IDC_DISABLE_RADIO, OnDisableRadio)
	ON_BN_CLICKED(IDC_ALT_V35_RADIO, OnAltV35Radio)
	ON_BN_CLICKED(IDC_ALT_INTELSAT_RADIO, OnAltIntelsatRadio)
	ON_BN_CLICKED(IDC_IESS308_RADIO, OnIess308Radio)
	ON_BN_CLICKED(IDC_IESS309_RADIO, OnIess309Radio)
	ON_BN_CLICKED(IDC_INTELSAT_RADIO, OnIntelsatRadio)
	ON_BN_CLICKED(IDC_TPC_SYNC_RADIO, OnTpcSyncRadio)
	ON_BN_CLICKED(IDC_V35_RADIO, OnV35Radio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScramblerDialog message handlers

void CScramblerDialog::OnDisableRadio() 
{
	m_Scrambler = 1;
}

void CScramblerDialog::OnAltV35Radio() 
{
	m_Scrambler = 5;
}

void CScramblerDialog::OnAltIntelsatRadio() 
{
	m_Scrambler = 7;
}

void CScramblerDialog::OnIess308Radio() 
{
	m_Scrambler = 2;
}

void CScramblerDialog::OnIess309Radio() 
{
	m_Scrambler = 3;
}

void CScramblerDialog::OnIntelsatRadio() 
{
	m_Scrambler = 6;
}

void CScramblerDialog::OnTpcSyncRadio() 
{
	m_Scrambler = 8;
}

void CScramblerDialog::OnV35Radio() 
{
	m_Scrambler = 4;
}

BOOL CScramblerDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_Scrambler == 1)
		CheckRadioButton(IDC_DISABLE_RADIO, IDC_TPC_SYNC_RADIO, IDC_DISABLE_RADIO);
	else if (m_Scrambler == 2)
		CheckRadioButton(IDC_DISABLE_RADIO, IDC_TPC_SYNC_RADIO, IDC_IESS308_RADIO);
	else if (m_Scrambler == 3)
		CheckRadioButton(IDC_DISABLE_RADIO, IDC_TPC_SYNC_RADIO, IDC_IESS309_RADIO);
	else if (m_Scrambler == 4)
		CheckRadioButton(IDC_DISABLE_RADIO, IDC_TPC_SYNC_RADIO, IDC_V35_RADIO);
	else if (m_Scrambler == 5)
		CheckRadioButton(IDC_DISABLE_RADIO, IDC_TPC_SYNC_RADIO, IDC_ALT_V35_RADIO);
	else if (m_Scrambler == 6)
		CheckRadioButton(IDC_DISABLE_RADIO, IDC_TPC_SYNC_RADIO, IDC_INTELSAT_RADIO);
	else if (m_Scrambler == 7)
		CheckRadioButton(IDC_DISABLE_RADIO, IDC_TPC_SYNC_RADIO, IDC_ALT_INTELSAT_RADIO);
	else if (m_Scrambler == 8)
		CheckRadioButton(IDC_DISABLE_RADIO, IDC_TPC_SYNC_RADIO, IDC_TPC_SYNC_RADIO);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
