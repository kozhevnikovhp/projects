#ifndef __TRANCEIVER_BATCH_TESTS_H_INCLUDED__
#define __TRANCEIVER_BATCH_TESTS_H_INCLUDED__

#include "AutoTestSheet.h"
#include "CommonTests.h"
#include "Modem.h"
#include "ModemComboBox.h"
class CProgressDlg;


/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsPage

class CTranceiverBatchTestsPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CTranceiverBatchTestsPage)

// Construction
public:
	CTranceiverBatchTestsPage(int nIDD);
	CTranceiverBatchTestsPage() {};
	~CTranceiverBatchTestsPage();
protected:
	virtual char *GetRegistrySectionName() { return "TranceiverBatchTest"; }
	BOOL TurnPureCarrier(CModem *pModem, BOOL bOnOff);
	BOOL Set10MHz(CModem *pModem, BOOL bOnOff);
	BOOL Get10MHz(CModem *pModem, BOOL &bOnOff);
	BOOL SetDCVoltage(CModem *pModem, cPowerSupplyCombo &PowerSupplyCombo);
	BOOL SetDCVoltage(CModem *pModem, int &PowerSupplyMode);
	BOOL GetDCVoltage(CModem *pModem, cPowerSupplyCombo &PowerSupplyCombo);
	BOOL GetDCValues(CModem *pModem, double &voltage, double &current);
	BOOL SetModemFrequency(CModem *pModem, unsigned int &Freq);
	BOOL GetModemFrequency(CModem *pModem, unsigned int &Freq);
	BOOL SetModemLevel(CModem *pModem, double &Level);
	BOOL GetModemLevel(CModem *pModem, double &Level);
	BOOL IsModemOutputOn(CModem *pModem, BOOL &bOn);
	BOOL TurnModemOutputOn(CModem *pModem, BOOL &bOn);
	BOOL SetTranceiverFrequency(CTranceiver *pTranceiver, unsigned int &Freq);

	BOOL MeasurePower(CPowerMeter *pPowerMeter, int channel, double &Power);

protected:
	static CModem *m_pModem;
	static CModem *m_pModem2;
	static CTranceiver *m_pTranceiver;
	static CPowerMeter *m_pPowerMeter;
// Overrides
	//{{AFX_VIRTUAL(CTranceiverBatchTestsPage)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTranceiverBatchTestsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsDevicePage property page

class CTranceiverBatchTestsDevicePage : public CTranceiverBatchTestsPage
{
	DECLARE_DYNCREATE(CTranceiverBatchTestsDevicePage)

// Construction
public:
	CTranceiverBatchTestsDevicePage();
	~CTranceiverBatchTestsDevicePage();

// Dialog Data
	//{{AFX_DATA(CTranceiverBatchTestsDevicePage)
	enum { IDD = IDD_TRANCEIVER_BATCH_TESTS_DEVICE_PROPPAGE };
	CComboBox	m_Modem2Combo;
	CComboBox	m_TranceiverCombo;
	CComboBox	m_FrequencyCounterCombo;
	CComboBox	m_PowerMeterCombo;
	CComboBox	m_ModemCombo;
	CString	m_FilePath;
	//}}AFX_DATA
	CDevice **m_ppDevices;
	int m_nDevices;

// Overrides
	//{{AFX_VIRTUAL(CTranceiverBatchTestsDevicePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CTranceiverBatchTestsDevicePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsRemoteControlPage property page

class CTranceiverBatchTestsRemoteControlPage : public CTranceiverBatchTestsPage
{
	DECLARE_DYNCREATE(CTranceiverBatchTestsRemoteControlPage)

// Construction
public:
	CTranceiverBatchTestsRemoteControlPage();
	~CTranceiverBatchTestsRemoteControlPage();

// Dialog Data
	//{{AFX_DATA(CTranceiverBatchTestsRemoteControlPage)
	enum { IDD = IDD_TRANCEIVER_BATCH_TESTS_REMOTE_CONTROL_PROPPAGE };
	cPowerSupplyCombo	m_PowerSupplyCombo;
	CEdit	m_ProtocolCtrl;
	CString	m_Protocol;
	double	m_InputLevelFrom;
	double	m_InputLevelStep;
	double	m_InputLevelTo;
	UINT	m_FreqFrom;
	UINT	m_FreqStep;
	UINT	m_FreqTo;
	double	m_Correction1;
	double	m_Correction2;
	UINT	m_Frequency1;
	UINT	m_Frequency2;
	double	m_10MHzLevel;
	BOOL	m_b10MHzEnabled;
	double	m_OutputCorrection;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CTranceiverBatchTestsRemoteControlPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL DoTest(unsigned int Frequency, CProgressDlg *pDlg);
	void UpdateDCValues();
	void UpdateModemControls();

	//{{AFX_MSG(CTranceiverBatchTestsRemoteControlPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRunTestButton();
	afx_msg void OnChangeAmendmentButton();
	afx_msg void On10MHzEnabledCheck();
	afx_msg void OnChangeOutputCorrectionButton();
	afx_msg void OnSelChangePowerSupplyCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsAmplitudeFrequencyPage property page

class CTranceiverBatchTestsAmplitudeFrequencyPage : public CTranceiverBatchTestsPage
{
	DECLARE_DYNCREATE(CTranceiverBatchTestsAmplitudeFrequencyPage)

// Construction
public:
	CTranceiverBatchTestsAmplitudeFrequencyPage();
	~CTranceiverBatchTestsAmplitudeFrequencyPage();

// Dialog Data
	//{{AFX_DATA(CTranceiverBatchTestsAmplitudeFrequencyPage)
	enum { IDD = IDD_TRANCEIVER_BATCH_TESTS_AMPLITUDE_FREQUENCY_PROPPAGE };
	cPowerSupplyCombo	m_PowerSupplyCombo;
	CEdit	m_ProtocolCtrl;
	CString	m_Protocol;
	double	m_InputLevelFrom;
	double	m_InputLevelStep;
	double	m_InputLevelTo;
	UINT	m_FreqFrom;
	UINT	m_FreqStep;
	UINT	m_FreqTo;
	double	m_Correction1;
	double	m_Correction2;
	UINT	m_Frequency1;
	UINT	m_Frequency2;
	BOOL	m_b10MHzEnabled;
	double	m_10MHzLevel;
	double	m_OutputCorrection;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CTranceiverBatchTestsAmplitudeFrequencyPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL DoTest(double Level, CProgressDlg *pDlg);
	void UpdateDCValues();
	void UpdateModemControls();

	//{{AFX_MSG(CTranceiverBatchTestsAmplitudeFrequencyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRunTestButton();
	afx_msg void OnChangeAmendmentButton();
	afx_msg void On10MHzEnabledCheck();
	afx_msg void OnChangeOutputCorrectionButton();
	afx_msg void OnSelChangePowerSupplyCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsSpuriousLevelPage dialog

class CTranceiverBatchTestsSpuriousLevelPage : public CTranceiverBatchTestsPage
{
// Construction
public:
	CTranceiverBatchTestsSpuriousLevelPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTranceiverBatchTestsSpuriousLevelPage)
	enum { IDD = IDD_TRANCEIVER_BATCH_TESTS_SPURIOUS_LEVEL_PROPPAGE };
	cPowerSupplyCombo	m_PowerSupplyCombo;
	CSpinButtonCtrl	m_InputLevelSpin;
	UINT	m_InputFreq;
	double	m_InputLevel;
	double	m_SpuriousLevel;
	CString	m_Protocol;
	UINT	m_OutputFreq;
	double	m_OutputLevel;
	BOOL	m_b10MHzEnabled;
	double	m_10MHzLevel;
	//}}AFX_DATA
	DWORD m_TimerID;


// Overrides
	//{{AFX_VIRTUAL(CTranceiverBatchTestsSpuriousLevelPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdatePowerMeter();
	void UpdateDCValues();
	void UpdateModemControls();

	//{{AFX_MSG(CTranceiverBatchTestsSpuriousLevelPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetButton();
	afx_msg void OnWriteButton();
	afx_msg void OnDeltaPosInputLevelSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void On10MHzEnabledCheck();
	afx_msg void OnSelChangePowerSupplyCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsIntermodulationLevelTest dialog

class CTranceiverBatchTestsIntermodulationLevelTest : public CTranceiverBatchTestsPage
{
	DECLARE_DYNCREATE(CTranceiverBatchTestsIntermodulationLevelTest)

// Construction
public:
	CTranceiverBatchTestsIntermodulationLevelTest();
	~CTranceiverBatchTestsIntermodulationLevelTest();

// Dialog Data
	//{{AFX_DATA(CTranceiverBatchTestsIntermodulationLevelTest)
	enum { IDD = IDD_TRANCEIVER_BATCH_TESTS_INTERMODULATION_LEVEL_PROPPAGE };
	cPowerSupplyCombo	m_PowerSupplyCombo2;
	cPowerSupplyCombo	m_PowerSupplyCombo;
	CSpinButtonCtrl	m_InputLevelSpin2;
	CSpinButtonCtrl	m_InputLevelSpin;
	UINT	m_InputFreq;
	double	m_InputLevel;
	BOOL	m_bOnOff;
	UINT	m_OutputFreq;
	double	m_OutputLevel;
	CString	m_Protocol;
	double	m_IntermodulationLevel;
	double	m_CenterOutputFreq;
	BOOL	m_b10MhzEnabled;
	double	m_10MHzLevel;
	BOOL	m_b10MhzEnabled2;
	UINT	m_InputFreq2;
	double	m_InputLevel2;
	BOOL	m_bOnOff2;
	UINT	m_OutputFreq2;
	//}}AFX_DATA
protected:
	DWORD m_TimerID;
//	CModem *m_pModem1, m_pModem2;

// Overrides
	//{{AFX_VIRTUAL(CTranceiverBatchTestsIntermodulationLevelTest)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	double CalcCenterOutputFreq();
	void UpdatePowerMeter();
	void UpdateDCValues();
	void UpdateDCValues2();
	void UpdateModemControls();
	void UpdateModemControls2();

	//{{AFX_MSG(CTranceiverBatchTestsIntermodulationLevelTest)
	afx_msg void OnSetButton();
	afx_msg void OnWriteButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnOnOffCheck();
	afx_msg void OnDeltaPosInputLevelSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void On10MhzEnabledCheck();
	afx_msg void OnSetButton2();
	afx_msg void On10mhzEnabledCheck2();
	afx_msg void OnDeltaPosInputLevelSpin2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOnOffCheck2();
	afx_msg void OnSelChangePowerSupplyCombo();
	afx_msg void OnSelChangePowerSupplyCombo2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CTranceiverBatchTestsPropertySheet

class CTranceiverBatchTestsPropertySheet : public CAutoTestSheet
{
	DECLARE_DYNAMIC(CTranceiverBatchTestsPropertySheet)

// Construction
public:
	CTranceiverBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd *pWndParent = NULL);

// Attributes
public:
	CTranceiverBatchTestsDevicePage m_DevicePage;
	CTranceiverBatchTestsRemoteControlPage m_RemoteControlPage;
	CTranceiverBatchTestsAmplitudeFrequencyPage m_AmplitudeFrequencyPage;
	CTranceiverBatchTestsSpuriousLevelPage m_SpuriousLevelPage;
	CTranceiverBatchTestsIntermodulationLevelTest m_IntermodulationLevelPage;
	CCommonTestsCongratulationsPage m_CongratulationPage;
	
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CTranceiverBatchTestsPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTranceiverBatchTestsPropertySheet();

protected:
	//{{AFX_MSG(CTranceiverBatchTestsPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif //__TRANCEIVER_BATCH_TESTS_H_INCLUDED__
