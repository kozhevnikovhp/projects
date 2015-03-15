#if !defined(AFX_DEVICEFORM_H__B3522D40_321B_11D4_BBDF_B00DFC8983C3__INCLUDED_)
#define AFX_DEVICEFORM_H__B3522D40_321B_11D4_BBDF_B00DFC8983C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeviceForm.h : header file
//

// Forward declarations
class CDevice;

/////////////////////////////////////////////////////////////////////////////
// CDeviceForm form view

class CDeviceForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CDeviceForm)

// Form Data
public:
	//{{AFX_DATA(CDeviceForm)
	enum { IDD = IDD_DEVICE_FORM };
	CButton	m_SetUnitNameButton;
	CEdit	m_UnitNameCtrl;
	CButton	m_SaveInNvRamOnTheFlyCheck;
	CButton	m_FactoryPresetButton;
	CButton	m_SaveButton;
	CButton	m_WarmResetButton;
	CButton	m_TotalResetButton;
	CButton	m_NVRAMCheck;
	BOOL	m_bNVRAMEnabled;
	CString	m_StatusString;
	CString	m_VersionString;
	BOOL	m_bSaveInNvRamOnTheFlyEnabled;
	CString	m_UnitName;
	//}}AFX_DATA

// Attributes
public:
	CDeviceForm();

// Operations
public:
	void RefreshStatus();

// Overrides
	//{{AFX_VIRTUAL(CDeviceForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL CanBeDeleted() { return TRUE; }
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual int DeltaTimerTime() { return 5; }

// Implementation
protected:
	virtual ~CDeviceForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:
	void UpdateVersionString();

	// Protected members
protected:
	CDevice *m_pDevice;

	//{{AFX_MSG(CDeviceForm)
	afx_msg void OnWarmResetButton();
	afx_msg void OnTotalResetButton();
	afx_msg void OnNVRAMEnabledCheck();
	afx_msg void OnSaveOnTheFlyCheck();
	afx_msg void OnSaveSettingsButton();
	afx_msg void OnFactoryPresetButton();
	afx_msg void OnSetUnitNameButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICEFORM_H__B3522D40_321B_11D4_BBDF_B00DFC8983C3__INCLUDED_)
