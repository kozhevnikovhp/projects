// ModemBatchTests.h : header file
//

#ifndef __MODEM_BATCH_TESTS_H_INCLUDED__
#define __MODEM_BATCH_TESTS_H_INCLUDED__

#include "Modem.h"
#include "AutoTestSheet.h"
#include "CommonTests.h"
#include "ModemComboBox.h"

class CProgressDlg;


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPage

class CModemBatchTestsPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CModemBatchTestsPage)

// Construction
public:
	CModemBatchTestsPage(int nIDD);
	CModemBatchTestsPage() {};
	~CModemBatchTestsPage();
protected:
	virtual char *GetRegistrySectionName() { return "ModemBatchTest"; }
// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsPage)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CModemBatchTestsPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsDevicePage property page

class CModemBatchTestsDevicePage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsDevicePage)

// Construction
public:
	CModemBatchTestsDevicePage();
	~CModemBatchTestsDevicePage();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsDevicePage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_DEVICE_PROPPAGE };
	CComboBox	m_FrequencyCounterCombo;
	CComboBox	m_PowerMeterCombo;
	CComboBox	m_EtalonModemCombo;
	CComboBox	m_DeviceCombo;
	CString	m_FilePath;
	//}}AFX_DATA
	CDevice **m_ppDevices;
	int m_nDevices;

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsDevicePage)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CModemBatchTestsDevicePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPortPage dialog

class CModemBatchTestsPortPage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsPortPage)

// Construction
public:
	CModemBatchTestsPortPage();
	~CModemBatchTestsPortPage();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsPortPage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_PORT_PROPPAGE };
	CString	m_ConnectionStatus;
	CString	m_Protocol;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsPortPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CModemBatchTestsPortPage)
	afx_msg void OnRepeatButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsWideFreqRangePage property page

class CModemBatchTestsWideFreqRangePage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsWideFreqRangePage)

// Construction
public:
	CModemBatchTestsWideFreqRangePage();
	~CModemBatchTestsWideFreqRangePage();

// Public methods

// Protected methods
protected:
	void SaveAllParameters();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsWideFreqRangePage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_WIDE_FREQ_RANGE_PROPPAGE };
	CEdit	m_MuteTimeCtrl;
	CEdit	m_HoldTimeCtrl;
	CListBox	m_ProblemFreqsListBox;
	UINT	m_HoldTime;
	BOOL	m_bHold;
	BOOL	m_bMute;
	UINT	m_MuteTime;
	UINT	m_EndFreq;
	UINT	m_StartFreq;
	UINT	m_Step;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsWideFreqRangePage)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CModemBatchTestsWideFreqRangePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRunButton();
	afx_msg void OnWaitCheck();
	afx_msg void OnMuteOutputCheck();
	afx_msg void OnRestoreDefaultsButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsOutputTurnOnOffPage property page

class CModemBatchTestsOutputTurnOnOffPage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsOutputTurnOnOffPage)

// Construction
public:
	CModemBatchTestsOutputTurnOnOffPage();
	~CModemBatchTestsOutputTurnOnOffPage();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsOutputTurnOnOffPage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_OUTPUT_TURN_ON_OFF_PROPPAGE };
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsOutputTurnOnOffPage)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CModemBatchTestsOutputTurnOnOffPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnTurnOnOffButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTests10MHzPage property page

class CModemBatchTests10MHzPage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTests10MHzPage)

// Construction
public:
	CModemBatchTests10MHzPage();
	~CModemBatchTests10MHzPage();
	virtual char *GetDeviceDescription() { return ""; }
	virtual char *GetRegimeDescription() { return ""; }
	virtual int NeedT10MHzMode() { return 0; }
	virtual int NeedR10MHzMode() { return 0; }
	virtual char *GetRegistrySectionName() { return "";	}

// Dialog Data
	//{{AFX_DATA(CModemBatchTests10MHzPage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_10MHZ_PROPPAGE };
	double	m_10MhzLevel;
	CString	m_DeviceName;
	double	m_Correction;
	double	m_Frequency;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTests10MHzPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CModemBatchTests10MHzPage)
	afx_msg void OnChangeShiftButton();
	afx_msg void OnMeasureButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsModulator10MHzOnPage property page

class CModemBatchTestsModulator10MHzOnPage : public CModemBatchTests10MHzPage
{
public:
	virtual char *GetDeviceDescription() { return "Modulator"; }
	virtual char *GetRegimeDescription() { return "ON"; }
	virtual int NeedT10MHzMode() { return 1; }
	virtual int NeedR10MHzMode() { return 0; }
	virtual char *GetRegistrySectionName() { return "Modulator10MHzShift";	}
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsModulator10MHzOffPage property page

class CModemBatchTestsModulator10MHzOffPage : public CModemBatchTests10MHzPage
{
public:
	virtual char *GetDeviceDescription() { return "Modulator"; }
	virtual char *GetRegimeDescription() { return "OFF"; }
	virtual int NeedT10MHzMode() { return 0; }
	virtual int NeedR10MHzMode() { return 0; }
	virtual char *GetRegistrySectionName() { return "Modulator10MHzShift";	}
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsDemodulator10MHzOnPage property page

class CModemBatchTestsDemodulator10MHzOnPage : public CModemBatchTests10MHzPage
{
public:
	virtual char *GetDeviceDescription() { return "Demodulator"; }
	virtual char *GetRegimeDescription() { return "ON"; }
	virtual int NeedT10MHzMode() { return 0; }
	virtual int NeedR10MHzMode() { return 1; }
	virtual char *GetRegistrySectionName() { return "Demodulator10MHzShift";	}
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsDemodulator10MHzOffPage property page

class CModemBatchTestsDemodulator10MHzOffPage : public CModemBatchTests10MHzPage
{
public:
	virtual char *GetDeviceDescription() { return "Demodulator"; }
	virtual char *GetRegimeDescription() { return "OFF"; }
	virtual int NeedT10MHzMode() { return 0; }
	virtual int NeedR10MHzMode() { return 0; }
	virtual char *GetRegistrySectionName() { return "Demodulator10MHzShift";	}
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPowerSupplyPage property page

class CModemBatchTestsPowerSupplyPage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsPowerSupplyPage)

// Construction
public:
	CModemBatchTestsPowerSupplyPage();
	~CModemBatchTestsPowerSupplyPage();
	virtual char *GetDeviceDescription() { return ""; }
	virtual char *GetVoltageDescription() { return ""; }
	virtual int GetTPowerSupplyMode() { return 0; }
	virtual int GetRPowerSupplyMode() { return 0; }
	virtual double GetCurrent() { return 0; }

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsPowerSupplyPage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_POWER_SUPPLY_PROPPAGE };
	double	m_MeasuredValue;
	double	m_Current;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsPowerSupplyPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CModemBatchTestsPowerSupplyPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPowerSupplyModulator0VoltsPage property page

class CModemBatchTestsPowerSupplyModulator0VoltsPage : public CModemBatchTestsPowerSupplyPage
{
public:
	virtual char *GetDeviceDescription() { return "Modulator"; }
	virtual char *GetVoltageDescription() { return "0 Volts"; }
	virtual int GetTPowerSupplyMode() { return 0; }
	virtual int GetRPowerSupplyMode() { return 0; } 
	virtual double GetCurrent();
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPowerSupplyModulator24VoltsPage property page

class CModemBatchTestsPowerSupplyModulator24VoltsPage : public CModemBatchTestsPowerSupplyPage
{
public:
	virtual char *GetDeviceDescription() { return "Modulator"; }
	virtual char *GetVoltageDescription() { return "24 Volts"; }
	virtual int GetTPowerSupplyMode() { return 1; }
	virtual int GetRPowerSupplyMode() { return 0; } 
	virtual double GetCurrent();
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPowerSupplyDemodulator0VoltsPage property page

class CModemBatchTestsPowerSupplyDemodulator0VoltsPage : public CModemBatchTestsPowerSupplyPage
{
public:
	virtual char *GetDeviceDescription() { return "Demodulator"; }
	virtual char *GetVoltageDescription() { return "0 Volts"; }
	virtual int GetTPowerSupplyMode() { return 0; }
	virtual int GetRPowerSupplyMode() { return 0; } 
	virtual double GetCurrent();
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPowerSupplyDemodulator13VoltsPage property page

class CModemBatchTestsPowerSupplyDemodulator13VoltsPage : public CModemBatchTestsPowerSupplyPage
{
public:
	virtual char *GetDeviceDescription() { return "Demodulator"; }
	virtual char *GetVoltageDescription() { return "13 Volts"; }
	virtual int GetTPowerSupplyMode() { return 0; }
	virtual int GetRPowerSupplyMode() { return 1; } 
	virtual double GetCurrent();
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPowerSupplyDemodulator18VoltsPage property page

class CModemBatchTestsPowerSupplyDemodulator18VoltsPage : public CModemBatchTestsPowerSupplyPage
{
public:
	virtual char *GetDeviceDescription() { return "Demodulator"; }
	virtual char *GetVoltageDescription() { return "18 Volts"; }
	virtual int GetTPowerSupplyMode() { return 0; }
	virtual int GetRPowerSupplyMode() { return 2; } 
	virtual double GetCurrent();
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsVariousOutputPage property page

/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsVariousOutputPage property page

class CModemBatchTestsVariousOutputPage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsVariousOutputPage)

// Construction
public:
	CModemBatchTestsVariousOutputPage();
	~CModemBatchTestsVariousOutputPage();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsVariousOutputPage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_VARIOUS_OUTPUT_PROPPAGE };
	cPowerSupplyCombo	m_PowerSupplyCombo;
	cFecModeCombo	m_FecModeCombo;
	CButton	m_SetDataRateButton;
	CSpinButtonCtrl	m_DataRateSpin;
	CEdit	m_DataRateEdit;
	CButton	m_SetOutputLevelButton;
	CEdit	m_OutputLevelEdit;
	cModulationTypeCombo m_ModulationTypeCombo;
	CButton	m_OutputOnOffCheck;
	CButton	m_SetFrequencyButton;
	CEdit	m_FrequencyCtrl;
	CSpinButtonCtrl	m_OutputLevelSpin;
	CSpinButtonCtrl	m_FrequencySpin;
	UINT	m_Frequency;
	BOOL	m_bOutputOn;
	double	m_OutputLevel;
	UINT	m_DataRate;
	CString	m_Report;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsVariousOutputPage)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateLnbStatus();
	CString GetSignalTextDescription(CModem *pModem);

	UINT m_TimerID;
	//{{AFX_MSG(CModemBatchTestsVariousOutputPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetFrequencyButton();
	afx_msg void OnOnoffCheck();
	afx_msg void OnDeltaposOutputLevelSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetOutputLevelButton();
	afx_msg void OnSelChangeModulationTypeCombo();
	afx_msg void OnSetDataRateButton();
	afx_msg void OnSelChangeFecModeCombo();
	afx_msg void OnClearReportButton();
	afx_msg void OnTestOkButton();
	afx_msg void OnTestFailedButton();
	afx_msg void OnSelChangePowerSupplyCombo();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsOutputLevelPage property page

typedef struct
{
	unsigned int m_Freq;
	unsigned int m_SA_Span;
	int m_SA_RefLevel;
	unsigned int m_SA_DbDiv;
	unsigned int m_RealFreq;
	void FormatShortDesc(char *pszDesc)
	{
		sprintf(pszDesc, "%7.3f MHz", m_Freq/1000.);
	}
} SOutputLevelTestDesc;


class CModemBatchTestsOutputLevelPage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsOutputLevelPage)

// Construction
public:
	CModemBatchTestsOutputLevelPage();
	~CModemBatchTestsOutputLevelPage();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsOutputLevelPage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_OUTPUT_LEVEL_PROPPAGE };
	CListBox	m_SignalListBox;
	BOOL	m_bAutomaticFlow;
	double	m_FromLevel;
	double	m_StepLevel;
	double	m_ToLevel;
	UINT	m_WaitTime;
	BOOL	m_bIntegralCheck;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsOutputLevelPage)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL oneFrequency();

protected:
	//{{AFX_MSG(CModemBatchTestsOutputLevelPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeSignalParametersList();
	afx_msg void OnNextButton();
	afx_msg void OnRunButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchSecondaryLevelsPage property page

typedef struct
{
	unsigned int m_Freq;
	int m_SA_RefLevel;
	unsigned int m_SA_DbDiv;
	void FormatShortDesc(char *pszDesc)
	{
		sprintf(pszDesc, "%7.3f MHz", m_Freq/1000.);
	}
} SSecondaryLevelsTestDesc;

class CModemBatchTestsSecondaryLevelsPage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsSecondaryLevelsPage)

// Construction
public:
	CModemBatchTestsSecondaryLevelsPage();
	~CModemBatchTestsSecondaryLevelsPage();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsSecondaryLevelsPage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_SECONDARY_SIGNALS_PROPPAGE };
	CListBox	m_SignalListBox;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsSecondaryLevelsPage)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CModemBatchTestsSecondaryLevelsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnNextButton();
	afx_msg void OnRestartTestsButton();
	afx_msg void OnRunButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsSearchRangePage property page

typedef struct
{
	unsigned int m_Freq;
	unsigned int m_DataRate;
	unsigned int m_SearchRange;		
	unsigned int m_SA_Span;		// spectrum analizer span
	unsigned int m_SA_RefLevel;
	unsigned int m_SA_DbDiv;
	void FormatShortDesc(char *pszDesc)
	{
		sprintf(pszDesc, "%7.3f MHz, %d baud, +/- %d KHz", m_Freq/1000., m_DataRate, m_SearchRange/1000);
	}
} SSearchRangeTestDesc;

class CModemBatchTestsSearchRangePage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsSearchRangePage)

// Construction
public:
	CModemBatchTestsSearchRangePage();
	~CModemBatchTestsSearchRangePage();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsSearchRangePage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_SEARCH_RANGE_PROPPAGE };
	cPowerSupplyCombo	m_PowerSupplyCombo;
	CListBox	m_SignalListBox;
	BOOL	m_bAutomaticFlow;
	//}}AFX_DATA
	UINT m_TimerID;

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsSearchRangePage)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnUpdateLevels(double OutputLevel, double InputLevel, double SN);

// Implementation
protected:
	//{{AFX_MSG(CModemBatchTestsSearchRangePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnNextButton();
	afx_msg void OnRunButton();
	afx_msg void OnSelChangePowerSupplyCombo();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void UpdateLnbStatus();
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsCarrierAcquisitionPage property page

class CModemBatchTestsCarrierAcquisitionPage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsCarrierAcquisitionPage)

// Construction
public:
	CModemBatchTestsCarrierAcquisitionPage();
	~CModemBatchTestsCarrierAcquisitionPage();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsCarrierAcquisitionPage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_CARRIER_ACQUISITION_PROPPAGE };
	CEdit	m_NTestsEdit;
	cFecModeCombo	m_FecModeCombo;
	CCheckListBox	m_DataRatesList;
	cPowerSupplyCombo	m_PowerSupplyCombo;
	CSpinButtonCtrl	m_NTestsSpin;
	cModulationTypeCombo	m_ModulationTypeCombo;
	UINT	m_Frequency;
	UINT	m_NTests;
	CString	m_Protocol;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsCarrierAcquisitionPage)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnUpdateLevels(double OutputLevel, double InputLevel, double SN);

// Implementation
protected:
	//{{AFX_MSG(CModemBatchTestsCarrierAcquisitionPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRunButton();
	afx_msg void OnSetFrequencyButton();
	afx_msg void OnSelChangeFecModeCombo();
	afx_msg void OnSelChangeModulationTypeCombo();
	afx_msg void OnSelChangePowerSupplyCombo();
	//}}AFX_MSG
	BOOL DoTest(CProgressDlg *pDlg);
	void UpdateLnbStatus();

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsBerTestPage property page

class CModemBatchTestsBerTestPage : public CModemBatchTestsPage
{
	DECLARE_DYNCREATE(CModemBatchTestsBerTestPage)

// Construction
public:
	CModemBatchTestsBerTestPage();
	~CModemBatchTestsBerTestPage();

// Dialog Data
	//{{AFX_DATA(CModemBatchTestsBerTestPage)
	enum { IDD = IDD_MODEM_BATCH_TESTS_BERTEST_PROPPAGE };
	cPowerSupplyCombo	m_PowerSupplyCombo;
	cFecModeCombo	m_FecModeCombo;
	cModulationTypeCombo	m_ModulationTypeCombo;
	CComboBox	m_DataRateCombo;
	CSpinButtonCtrl	m_FrequencySpin;
	CString	m_Protocol;
	UINT	m_Frequency;
	double m_OutputLevel;
	CSpinButtonCtrl	m_OutputLevelSpin;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsBerTestPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DoTest();
	BOOL SetParams();
	void UpdateSNandLevel();
	void UpdateLnbStatus();

	UINT m_TimerID;
	//{{AFX_MSG(CModemBatchTestsBerTestPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRepeatButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDeltaPosOutputLevelSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeDataRateCombo();
	afx_msg void OnSelChangeFecModeCombo();
	afx_msg void OnSelChangePowerSupplyCombo();
	afx_msg void OnFreqButton();
	afx_msg void OnLevelButton();
	afx_msg void OnSelChangeModulationTypeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemBatchTestsPropertySheet

class CModemBatchTestsPropertySheet : public CAutoTestSheet
{
	DECLARE_DYNAMIC(CModemBatchTestsPropertySheet)

// Construction
public:
	CModemBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent = NULL);

// Attributes
public:
	CModemBatchTestsDevicePage m_DevicePage;
	CModemBatchTestsPortPage m_MCPortPage;
	CModemBatchTestsWideFreqRangePage m_WideFreqRangePage;
	CModemBatchTestsOutputLevelPage m_OutputLevelPage;
	CModemBatchTestsSecondaryLevelsPage m_SecondaryLevelsPage;
	CModemBatchTestsOutputTurnOnOffPage m_OutputTurnOnOffPage;
	CModemBatchTestsModulator10MHzOnPage m_Modulator10MHzOnPage;
	CModemBatchTestsModulator10MHzOffPage m_Modulator10MHzOffPage;
	CModemBatchTestsDemodulator10MHzOnPage m_Demodulator10MHzOnPage;
	CModemBatchTestsDemodulator10MHzOffPage m_Demodulator10MHzOffPage;
	CModemBatchTestsPowerSupplyModulator0VoltsPage m_PowerSupplyModulator0VoltsPage;
	CModemBatchTestsPowerSupplyModulator24VoltsPage m_PowerSupplyModulator24VoltsPage;
	CModemBatchTestsPowerSupplyDemodulator0VoltsPage m_PowerSupplyDemodulator0VoltsPage;
	CModemBatchTestsPowerSupplyDemodulator13VoltsPage m_PowerSupplyDemodulator13VoltsPage;
	CModemBatchTestsPowerSupplyDemodulator18VoltsPage m_PowerSupplyDemodulator18VoltsPage;
	CModemBatchTestsVariousOutputPage m_VariousOutputPage;
	CModemBatchTestsSearchRangePage m_SearchRangePage;
	CModemBatchTestsCarrierAcquisitionPage m_CarrierAcquisitionPage;
	CModemBatchTestsBerTestPage m_BerTestPage;
	CCommonTestsCongratulationsPage m_CongratulationPage;
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CModemBatchTestsPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModemBatchTestsPropertySheet();

protected:
	//{{AFX_MSG(CModemBatchTestsPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // __MODEM_BATCH_TESTS_H_INCLUDED__
