// CarrierCaptureAutoTestPages.h : header file
//

#ifndef __AUTOTESTPAGES_H__
#define __AUTOTESTPAGES_H__

// Forward declarations
class CDevice;
class CModem;
class CPowerMeter;
class CFrequencyCounter;
class CSdmsAPI;
class CTranceiver;
class CProgressDlg;

const int MAX_TRANSMITTER = 10;
const int MAX_RECEIVER = 10;

typedef struct
{
	int m_nNumberOfTransmitters;
	CModem *m_pModulator[MAX_TRANSMITTER];
	CModem *m_pTestModem;
	CModem *m_pEtalonModem;
	CPowerMeter *m_pPowerMeter;
	CPowerMeter *m_pHumanPowerMeter;
	CFrequencyCounter *m_pFrequencyCounter;
	CFrequencyCounter *m_pHumanFrequencyCounter;
	CSdmsAPI *m_pTestSDMS;
	CSdmsAPI *m_pEtalonSDMS;
	CTranceiver *m_pTestTranceiver;
	int m_nModulator[MAX_TRANSMITTER];
	int m_ModulatorFreq;
	int m_nNumberOfReceivers;
	CModem *m_pDemodulator[MAX_RECEIVER];
	int m_nDemodulator[MAX_RECEIVER];
	int m_DemodulatorFreq;
	int m_DemodulatorSpareFreq;
	unsigned int m_StartFreq2, m_EndFreq2, m_StepFreq2;
	int m_CurrentTransmitter, m_CurrentReceiver;
	int m_ModulationType;
	int m_ModulationType2;
	int m_StartDataRate, m_EndDataRate;
	int m_StartDataRate2, m_EndDataRate2;
	double m_StartLevel, m_EndLevel, m_StepLevel;
	int m_nTests;
	BOOL m_bTurnModOffOn;
	char m_pszFilePath[256];
	CStdioFile *m_pReportFile;
} SAutoTestConfig;

extern char *pszMeasurementsSectionName;

double CalcCorrection(unsigned int Freq1, double Correction1, unsigned int Freq2, double Correction2, unsigned int Freq);

class CAutoTestPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAutoTestPropertyPage)

// Construction
public:
	CAutoTestPropertyPage(int nIDD);
	CAutoTestPropertyPage() {};
	~CAutoTestPropertyPage();

// Dialog Data
	//{{AFX_DATA(CAutoTestPropertyPage)
	//}}AFX_DATA
	static SAutoTestConfig *m_pAutoTestConfig;
// Overrides
	//{{AFX_VIRTUAL(CAutoTestPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnUpdateLevels(double OutputLevel, double InputLevel, double SN) {}

// Implementation
protected:
	virtual char *GetRegistrySectionName() { return "Unknown"; }
	int ReadRegistryInt(const char *pszParamName, int nDefaultValue);
	void WriteRegistryInt(const char *pszParamName, int nValue);
	void WriteString(CStdioFile *pFile, const char *pszStr);
	BOOL AdjustSignal(CModem *pEtalonModem, CModem *pModem, double DesiredSNFrom, double DesiredSNTo, CProgressDlg *pDlg = NULL);
	CStdioFile *GetReportFile();
	BOOL ChangeCorrection(double &Correction);
	BOOL ChangeFrequencyDependentCorrection(unsigned int &Freq1, double &Correction1, unsigned int &Freq2, double &Correction2);
	//{{AFX_MSG(CAutoTestPropertyPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDevicesPage dialog

class CDevicesPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CDevicesPage)

// Construction
public:
	CDevicesPage();
	~CDevicesPage();

// Dialog Data
	//{{AFX_DATA(CDevicesPage)
	enum { IDD = IDD_DEVICES_PROPPAGE };
	CTabCtrl	m_ReceiverTab;
	CTabCtrl	m_TransmitterTab;
	CSpinButtonCtrl	m_ModulatorNoSpin;
	CSpinButtonCtrl	m_DemodulatorNoSpin;
	CComboBox	m_ReceiverCombo;
	CComboBox	m_TransmitterCombo;
	int		m_DemodulatorNo;
	int		m_ModulatorNo;
	//}}AFX_DATA
	CDevice **m_ppDevices;
	int m_nDevices;
	int m_MaxModulator, m_MaxDemodulator;

// Overrides
	//{{AFX_VIRTUAL(CDevicesPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDevicesPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeTransmitterCombo();
	afx_msg void OnSelChangeReceiverCombo();
	afx_msg void OnSelChangeTransmitterTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangingTransmitterTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeReceiverTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangingReceiverTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void SaveReceiver();
	void SaveTransmitter();
	void RestoreReceiver();
	void RestoreTransmitter();
};


/////////////////////////////////////////////////////////////////////////////
// CCarrierPage dialog

class CCarrierPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CCarrierPage)

// Construction
public:
	CCarrierPage();
	~CCarrierPage();

// Dialog Data
	//{{AFX_DATA(CCarrierPage)
	enum { IDD = IDD_PROPPAGE2 };
	CComboBox	m_ModulationTypeCombo;
	CSpinButtonCtrl	m_TransmitterFreqSpin;
	CSpinButtonCtrl	m_ReceiverFreq2Spin;
	CSpinButtonCtrl	m_ReceiverFreqSpin;
	int		m_ReceiverFreq;
	int		m_ReceiverSpareFreq;
	int		m_TransmitterFreq;
	//}}AFX_DATA
	int m_MinTransmitterFreq, m_MaxTransmitterFreq;
	int m_MinReceiverFreq, m_MaxReceiverFreq;

// Overrides
	//{{AFX_VIRTUAL(CCarrierPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CCarrierPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CDataPage dialog

class CDataPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CDataPage)

// Construction
public:
	CDataPage();
	~CDataPage();

// Dialog Data
	//{{AFX_DATA(CDataPage)
	enum { IDD = IDD_PROPPAGE3 };
	CComboBox	m_StartDataRateCombo;
	CComboBox	m_EndDataRateCombo;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDataPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDataPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CLevelsPage dialog

class CLevelsPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CLevelsPage)

// Construction
public:
	CLevelsPage();
	~CLevelsPage();

// Dialog Data
	//{{AFX_DATA(CLevelsPage)
	enum { IDD = IDD_PROPPAGE4 };
	CSpinButtonCtrl	m_StepLevelSpin;
	CSpinButtonCtrl	m_StartLevelSpin;
	CSpinButtonCtrl	m_EndLevelSpin;
	double	m_EndLevel;
	double	m_StartLevel;
	double	m_StepLevel;
	//}}AFX_DATA
	double m_MinLevel, m_MaxLevel;


// Overrides
	//{{AFX_VIRTUAL(CLevelsPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CLevelsPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// COtherPage dialog

class COtherPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(COtherPage)

// Construction
public:
	COtherPage();
	~COtherPage();

// Dialog Data
	//{{AFX_DATA(COtherPage)
	enum { IDD = IDD_PROPPAGE5 };
	CSpinButtonCtrl	m_NumTestsSpin;
	int		m_NumTests;
	CString	m_FilePath;
	BOOL	m_bTurnTransmitterOff;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(COtherPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(COtherPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSecondSignalPage dialog

class CSecondSignalPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CSecondSignalPage)

// Construction
public:
	CSecondSignalPage();
	~CSecondSignalPage();

// Dialog Data
	//{{AFX_DATA(CSecondSignalPage)
	enum { IDD = IDD_SECOND_SIGNAL_FORM };
	CComboBox	m_ModulationTypeCombo;
	CComboBox	m_EndDataRateCombo;
	CComboBox	m_StartDataRateCombo;
	CSpinButtonCtrl	m_StepFreqSpin;
	CSpinButtonCtrl	m_StartFreqSpin;
	CSpinButtonCtrl	m_EndFreqSpin;
	int		m_StartFreq;
	int		m_EndFreq;
	int		m_StepFreq;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CSecondSignalPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	//{{AFX_MSG(CSecondSignalPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////////////////////////////
// Misc

BOOL DoYouReallyWantToCancel(CWnd *pParentWnd);

#endif // __AUTOTESTPAGES_H__
