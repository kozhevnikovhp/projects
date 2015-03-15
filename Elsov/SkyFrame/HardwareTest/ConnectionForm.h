#if !defined(AFX_CONNECTIONFORM_H__71CF7705_3091_11D4_BBDF_D3D22F8DA54E__INCLUDED_)
#define AFX_CONNECTIONFORM_H__71CF7705_3091_11D4_BBDF_D3D22F8DA54E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectionForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectionForm form view

class CConnectionForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CConnectionForm)

// Form Data
public:
	//{{AFX_DATA(CConnectionForm)
	enum { IDD = IDD_CONNECTION_FORM };
	CSpinButtonCtrl	m_BusAddressSpin;
	CComboBox	m_PortComboBox;
	CString	m_TcpIpAdress;
	UINT	m_BusAddress;
	//}}AFX_DATA

// Attributes
public:
	CConnectionForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CConnectionForm)
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CConnectionForm();
	void EnableControls();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CConnectionForm)
	afx_msg void OnApplyButton();
	afx_msg void OnDisconnectButton();
	afx_msg void OnTelnetRadio();
	afx_msg void OnSerialPortRadio();
	afx_msg void OnSetBusAddressButton();
	afx_msg void OnScanButton();
	afx_msg void OnSnmpRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CScanMCBusAddressDialog dialog

class CScanMCBusAddressDialog : public CDialog
{
// Construction
public:
	CScanMCBusAddressDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScanMCBusAddressDialog)
	enum { IDD = IDD_SCAN_MC_BUS_ADDRESS_DIALOG };
	CSpinButtonCtrl	m_StopScanningSpin;
	CSpinButtonCtrl	m_StartScanningSpin;
	UINT	m_StartScanning;
	UINT	m_StopScanning;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CScanMCBusAddressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetRange(UINT Start, UINT Stop);

protected:

	//{{AFX_MSG(CScanMCBusAddressDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIONFORM_H__71CF7705_3091_11D4_BBDF_D3D22F8DA54E__INCLUDED_)
