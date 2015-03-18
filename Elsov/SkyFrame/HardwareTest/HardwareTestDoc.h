// HardwareTestDoc.h : interface of the CHardwareTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HARDWARETESTDOC_H__1C2CEEFE_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
#define AFX_HARDWARETESTDOC_H__1C2CEEFE_300A_11D4_BBDF_E46DF892F25A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAbstractForm;
class CHardwareTestView;
class CDevice;

const int MAX_DEVICES=32;

enum FI_FormIndex
{
	ABSTRACT_INDEX,
	TERMINAL_INDEX,
	DEVICE_INDEX,
	CONNECTION_INDEX,
	RECEIVER_CARRIER_INDEX,
	RECEIVER_DATA_INDEX,
	RECEIVER_LNB_STATUS_INDEX,
	RECEIVER_BERTEST_INDEX,
	TRANSMITTER_CARRIER_INDEX,
	TRANSMITTER_DATA_INDEX,
	TRANSMITTER_BUC_STATUS_INDEX,
	TRANSMITTER_BERTEST_INDEX,
	TRANCEIVER_INDEX,
	NSG_SCRIPT_INDEX,
	PHYSICAL_PORTS_INDEX,
	FRAME_RELAY_STATIONS_INDEX,
	ETHERNET_STATIONS_INDEX,
	TELNET_STATIONS_INDEX,
	IP_INTERFACES_INDEX,
	IP_STATIC_ROUTES_INDEX,
	IP_ROUTING_TABLE_INDEX,
	X25_ROUTING_INDEX,
	PVC_INDEX,
	UP_CONVERTER_INDEX,
	DOWN_CONVERTER_INDEX,
	SOFTWARE_VERSION_INDEX,
	STARTUP_CONFIG_INDEX,
	ETHERNET_PORT_INDEX,
	ETHERNET_STATUS_INDEX,
	SERIAL_PORT_INDEX,
	SERIAL_STATUS_INDEX,
	DEVICE_SUBSTITUTOR_INDEX,
	POWER_METER_INDEX,
	FREQUENCY_COUNTER_INDEX,
	STOP_FORM_INDEX
};

class CHardwareTestDoc : public CDocument
{
protected: // create from serialization only
	CHardwareTestDoc();

	DECLARE_DYNCREATE(CHardwareTestDoc)

// Attributes
public:
	CAbstractForm *m_pForms[STOP_FORM_INDEX+1];
	CHardwareTestView *m_pTreeView;
	BOOL m_bWriteLog;
	CString m_LogFileName;
	CString m_LogFileDir;
	FILE *m_pLogFile;
// Operations
public:
	void AddNewForm(FI_FormIndex IndexInViewArray, CView *pViewToAdd, CWnd *pParent);
	BOOL IsAbstractFormUpdateTimerEnabled() { return m_bAbstractFormUpdateTimerEnabled;	}

// Overrides
	//{{AFX_VIRTUAL(CHardwareTestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHardwareTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void OpenLogFile();
	void CloseLogFile();
	void ClearDevices();
	void FreeDevice(int index);
	void FreeDevices();
	int FindPlaceForDevice();
	int AddDevice(CDevice *pDevice);

	// Protected members
protected:
	CDevice *m_pDevices[MAX_DEVICES];
	int m_DeviceCount;
	BOOL m_bAbstractFormUpdateTimerEnabled;

// Generated message map functions
protected:
	//{{AFX_MSG(CHardwareTestDoc)
	afx_msg void OnOptionsLogfile();
	afx_msg void OnUpdateOptionsLogfile(CCmdUI* pCmdUI);
	afx_msg void OnDeviceCarrierCaptureAutoTest();
	afx_msg void OnDeviceIntervalTest();
	afx_msg void OnDeviceModulatorCarrierTest();
	afx_msg void OnDeviceDelete();
	afx_msg void OnDeviceAddModemDatumPsm4900_70MHz();
	afx_msg void OnDeviceAddModemDatumPsm4900140MHz();
	afx_msg void OnDeviceAddModemDatumPsm4900_Hybrid();
	afx_msg void OnDeviceAddAccessDeviceNsgNps3e();
	afx_msg void OnDeviceAddAccessDeviceNsgNx300();
	afx_msg void OnDeviceAddTranceiverWaveSatWtr70();
	afx_msg void OnDeviceAddTranceiverAdvantechWty1();
	afx_msg void OnDeviceAddModemDatumPsm4900_Lband();
	afx_msg void OnTestsModemTest();
	afx_msg void OnTestsStelaMTests();
	afx_msg void OnDeviceAddTranceiverWavesatSubstitutor();
	afx_msg void OnDeviceAddPowerMeterAgilentE4418B();
	afx_msg void OnTestsModemSdmsTests();
	afx_msg void OnTestsSdmsTests();
	afx_msg void OnDeviceAddUpConverterKvv140Ver2002();
	afx_msg void OnDeviceAddUpConverterKvv140Ver2004();
	afx_msg void OnDeviceAddrDownConverterKvn140Ver2002();
	afx_msg void OnDeviceAddDownConverterKvn140Ver2004();
	afx_msg void OnDeviceAddFrequencyCounterAgilent53150A();
	afx_msg void OnTestsTranceiverTests();
	afx_msg void OnDeviceAddTranceiverVertex50watt();
	afx_msg void OnDeviceAddTranceiverVertexSubstitutor();
	afx_msg void OnDeviceAddModemDatumPSM500Lband();
	afx_msg void OnDeviceAddDownConverterKvn140Ver2011();
	afx_msg void OnDeviceAddUpConverterKvv140Ver2011();
	afx_msg void OnDeviceAddSdmsSdmsVsat4900l();
	afx_msg void OnDeviceAddSdmsSdmsVsatM500l();
	afx_msg void OnDeviceAddModemDatumM7LModLDem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HARDWARETESTDOC_H__1C2CEEFE_300A_11D4_BBDF_E46DF892F25A__INCLUDED_)
