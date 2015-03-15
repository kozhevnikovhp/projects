#if !defined(AFX_DEMODULATORCARRIERFORM_H__71CF7707_3091_11D4_BBDF_D3D22F8DA54E__INCLUDED_)
#define AFX_DEMODULATORCARRIERFORM_H__71CF7707_3091_11D4_BBDF_D3D22F8DA54E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DemodulatorCarrierForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDemodulatorCarrierForm form view

#include "Modem.h"
#include "ModemComboBox.h"

class CModem;

class CDemodulatorCarrierForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CDemodulatorCarrierForm)

// Form Data
public:
	//{{AFX_DATA(CDemodulatorCarrierForm)
	enum { IDD = IDD_DEMODULATOR_CARRIER_FORM };
	cPowerSupplyCombo	m_PowerSupplyCombo;
	cModulationTypeCombo	m_ModulationTypeCombo;
	CButton	m_SetFineTuneButton;
	CSpinButtonCtrl	m_FineTuneSpin;
	CEdit	m_FineTuneEdit;
	CButton	m_SetShiftButton;
	CEdit	m_ShiftEdit;
	CSpinButtonCtrl	m_ShiftSpin;
	CButton	m_SetSearchRangeButton;
	CEdit	m_SearchRangeCtrl;
	CButton	m_MeasureTimeButton;
	CButton	m_SetFrequencyButton;
	CEdit	m_FrequencyCtrl;
	CButton	m_10MHzSupplierCheck;
	CButton	m_SpectralInvCheck;
	CSpinButtonCtrl	m_SearchRangeSpin;
	CSpinButtonCtrl	m_ReceiverFrequencySpin;
	UINT	m_Frequency;
	UINT	m_SearchRange;
	BOOL	m_bSpectralInvEnabled;
	BOOL	m_b10MHzSupplierEnabled;
	int		m_Shift;
	int		m_FineTune;
	//}}AFX_DATA

// Attributes
public:
	CDemodulatorCarrierForm();

	// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CDemodulatorCarrierForm)
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual int DeltaTimerTime() { return 5; }

	// Implementation
protected:
	virtual ~CDemodulatorCarrierForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:
	void UpdateEbNoAndOffset();
	void UpdateLnbStatus();

	// Protected members
protected:
	CModem *m_pModem;
	BOOL m_bTimerEnabled;

	//{{AFX_MSG(CDemodulatorCarrierForm)
	afx_msg void OnSetFrequencyButton();
	afx_msg void OnSetShiftButton();
	afx_msg void OnSetRangeButton();
	afx_msg void OnSpectralInvCheck();
	afx_msg void OnMeasureButton();
	afx_msg void On10MHzSupplierCheck();
	afx_msg void OnSetFineTuneButton();
	afx_msg void OnSelChangeModulationTypeCombo();
	afx_msg void OnSelChangePowerSupplyCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMODULATORCARRIERFORM_H__71CF7707_3091_11D4_BBDF_D3D22F8DA54E__INCLUDED_)
