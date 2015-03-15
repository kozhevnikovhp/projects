#ifndef __MODEM_SDMS_BATCH_TESTS_H_INCLUDED__
#define __MODEM_SDMS_BATCH_TESTS_H_INCLUDED__

#include "ModemBatchTests.h"
#include "SdmsBatchTests.h"

/////////////////////////////////////////////////////////////////////////////
// CModemSdmsBatchTestsDevicePage property page

class CModemSdmsBatchTestsDevicePage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CModemSdmsBatchTestsDevicePage)

// Construction
public:
	CModemSdmsBatchTestsDevicePage();
	~CModemSdmsBatchTestsDevicePage();

// Dialog Data
	//{{AFX_DATA(CModemSdmsBatchTestsDevicePage)
	enum { IDD = IDD_MODEM_SDMS_BATCH_TESTS_DEVICE_PROPPAGE };
	CComboBox	m_EtalonSdmsCombo;
	CComboBox	m_EtalonModemCombo;
	CComboBox	m_FrequencyCounterCombo;
	CComboBox	m_PowerMeterCombo;
	CComboBox	m_StelaMCombo;
	CComboBox	m_ModemCombo;
	CString	m_FilePath;
	//}}AFX_DATA
	CDevice **m_ppDevices;
	int m_nDevices;

// Overrides
	//{{AFX_VIRTUAL(CModemSdmsBatchTestsDevicePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CModemSdmsBatchTestsDevicePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModemSdmsBatchTestsDataThroughPage property page

class CModemSdmsBatchTestsDataThroughPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CModemSdmsBatchTestsDataThroughPage)

// Construction
public:
	CModemSdmsBatchTestsDataThroughPage();
	~CModemSdmsBatchTestsDataThroughPage();

// Protected overridables
protected:

// Dialog Data
	//{{AFX_DATA(CModemSdmsBatchTestsDataThroughPage)
	enum { IDD = IDD_MODEM_SDMS_BATCH_TESTS_DATA_THROUGH_PROPPAGE };
	CString	m_Protocol;
	CString	m_EtalonEthernetIP;
	CString	m_EtalonSerialIP;
	CString	m_LocalHostIP;
	CString	m_TestEthernetIP;
	CString	m_TestSerialIP;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CModemSdmsBatchTestsDataThroughPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DoTest();
	//{{AFX_MSG(CModemSdmsBatchTestsDataThroughPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRunButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
// Protected members
};


/////////////////////////////////////////////////////////////////////////////
// CModemSdmsBatchTestsPropertySheet

class CModemSdmsBatchTestsPropertySheet : public CAutoTestSheet
{
	DECLARE_DYNAMIC(CModemSdmsBatchTestsPropertySheet)

// Construction
public:
	CModemSdmsBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent = NULL);

// Attributes
public:
	CModemSdmsBatchTestsDevicePage m_DevicePage;
	CModemBatchTestsPortPage m_ModemPortPage;
	CSdmsBatchTestsPortsPage m_SdmsPortsPage;
	CSdmsBatchTestsEthernetPage m_EthernetPage;
	CSdmsBatchTestsSoftwareVersionPage m_SoftwareVersionPage;
	CSdmsBatchTestsShellSerialPortPage m_SdmsShellSerialPortPage;
	CSdmsBatchTestsShellTelnetPage m_SdmsShellTelnetPage;
	CSdmsBatchTestsStartupConfigPage m_StartupConfigPage;
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
	CModemSdmsBatchTestsDataThroughPage m_DataThroughPage;
	CModemBatchTestsBerTestPage m_BerTestPage;
	CCommonTestsCongratulationsPage m_CongratulationPage;
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CModemSdmsBatchTestsPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModemSdmsBatchTestsPropertySheet();

protected:
	//{{AFX_MSG(CModemSdmsBatchTestsPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__MODEM_SDMS_BATCH_TESTS_H_INCLUDED__
