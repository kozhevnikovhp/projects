// StationDevicesForm.h : header file
//

#ifndef __STATION_DEVICES_FORM_H_INCLUDED__
#define __STATION_DEVICES_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CStationDevicesForm form view

class CStationDevicesForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CStationDevicesForm)

// Form Data
public:
	//{{AFX_DATA(CStationDevicesForm)
	enum { IDD = IDD_STATION_DEVICES_FORM };
	CComboBox	m_DeviceTypeCombo;
	CEdit	m_NameEditCtrl;
	CIPAddressCtrl	m_IpAddressCtrl;
	CTabCtrl	m_DevicesTabCtrl;
	CString	m_Name;
	//}}AFX_DATA

// Attributes
public:
	CStationDevicesForm();

	// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CStationDevicesForm)
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CStationDevicesForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

// Protected members
protected:

	//{{AFX_MSG(CStationDevicesForm)
	afx_msg void OnAddButton();
	afx_msg void OnSelChangeDevicesTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnApplyButton();
	afx_msg void OnDeleteButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __STATION_DEVICES_FORM_H_INCLUDED__
