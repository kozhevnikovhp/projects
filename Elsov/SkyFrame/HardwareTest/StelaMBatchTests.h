#ifndef __VPPU_BATCH_TESTS_H_INCLUDED__
#define __VPPU_BATCH_TESTS_H_INCLUDED__

#include "CommonTests.h"
#include "ModemBatchTests.h"
#include "ModemSdmsBatchTests.h"
#include "SdmsBatchTests.h"
#include "Ftp.h"
class CProgressDlg;


/////////////////////////////////////////////////////////////////////////////
// CVppuBatchTestsDevicePage property page

class CVppuBatchTestsDevicePage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CVppuBatchTestsDevicePage)

// Construction
public:
	CVppuBatchTestsDevicePage();
	~CVppuBatchTestsDevicePage();

// Dialog Data
	//{{AFX_DATA(CVppuBatchTestsDevicePage)
	enum { IDD = IDD_VPPU_BATCH_TESTS_DEVICE_PROPPAGE };
	CComboBox	m_FrequencyCounterCombo;
	CComboBox	m_PowerMeterCombo;
	CComboBox	m_SdmsCombo;
	CComboBox	m_ModemCombo;
	CString	m_FilePath;
	//}}AFX_DATA
	CDevice **m_ppDevices;
	int m_nDevices;

// Overrides
	//{{AFX_VIRTUAL(CVppuBatchTestsDevicePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CVppuBatchTestsDevicePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CVppuBatchTestsRadioPage property page

class CVppuBatchTestsRadioPage : public CAutoTestPropertyPage
{
	DECLARE_DYNCREATE(CVppuBatchTestsRadioPage)

// Construction
public:
	CVppuBatchTestsRadioPage();
	~CVppuBatchTestsRadioPage();

// Dialog Data
	//{{AFX_DATA(CVppuBatchTestsRadioPage)
	enum { IDD = IDD_VPPU_BATCH_TESTS_RADIO_PROPPAGE };
	CString	m_Protocol;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CVppuBatchTestsRadioPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DoTest();
	//{{AFX_MSG(CVppuBatchTestsRadioPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRepeatButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CVppuBatchTestsPropertySheet

class CVppuBatchTestsPropertySheet : public CAutoTestSheet
{
	DECLARE_DYNAMIC(CVppuBatchTestsPropertySheet)

// Construction
public:
	CVppuBatchTestsPropertySheet(CDevice **ppDevices, int nDevices, CWnd* pWndParent = NULL);

// Attributes
public:
	CVppuBatchTestsDevicePage m_DevicePage;
	CModemBatchTestsPortPage m_ModemPortPage;
	CSdmsBatchTestsPortsPage m_SdmsPortsPage;
	CSdmsBatchTestsEthernetPage m_EthernetPage;
	CSdmsBatchTestsSoftwareVersionPage m_SoftwareVersionPage;
	CSdmsBatchTestsShellSerialPortPage m_SdmsShellSerialPortPage;
	CSdmsBatchTestsShellTelnetPage m_SdmsShellTelnetPage;
	CVppuBatchTestsRadioPage m_RadioPage;
	CModemBatchTestsOutputTurnOnOffPage m_OutputTurnOnOffPage;
	CModemBatchTestsModulator10MHzOnPage m_Modulator10MHzOnPage;
	CModemBatchTestsDemodulator10MHzOnPage m_Demodulator10MHzOnPage;
	CModemBatchTestsPowerSupplyModulator24VoltsPage m_PowerSupplyModulator24VoltsPage;
	CModemBatchTestsPowerSupplyDemodulator13VoltsPage m_PowerSupplyDemodulator13VoltsPage;
	CModemBatchTestsPowerSupplyDemodulator18VoltsPage m_PowerSupplyDemodulator18VoltsPage;
	CModemBatchTestsCarrierAcquisitionPage m_CarrierAcquisitionPage;
	CSdmsBatchTestsStartupConfigPage m_StartupConfigPage;
	CModemBatchTestsBerTestPage m_BerTestPage;
	CCommonTestsCongratulationsPage m_CongratulationPage;
	
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CVppuBatchTestsPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVppuBatchTestsPropertySheet();

protected:
	//{{AFX_MSG(CVppuBatchTestsPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__VPPU_BATCH_TESTS_H_INCLUDED__
