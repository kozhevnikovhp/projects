#ifndef __MODULATOR_CARRIER_FORM_H__INCLUDED__
#define __MODULATOR_CARRIER_FORM_H__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ModulatorCarrierForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModulatorCarrierForm form view

#include "Modem.h"
#include "ModemComboBox.h"

class CModulatorCarrierForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CModulatorCarrierForm)

// Form Data
public:
	//{{AFX_DATA(CModulatorCarrierForm)
	enum { IDD = IDD_MODULATOR_CARRIER_FORM };
	SpectrumCombo m_SpectrumCombo;
	c10MHzCombo	m_Buc10MHzCombo;
	CEdit	m_OutputLevelEdit;
	cPowerSupplyCombo	m_PowerSupplyCombo;
	cModulationTypeCombo	m_ModulationTypeCombo;
	CButton	m_SetFineTuneButton;
	CSpinButtonCtrl	m_FineTuneSpin;
	CEdit	m_FineTuneEdit;
	CSpinButtonCtrl	m_ShiftSpin;
	CEdit	m_ShiftEdit;
	CButton	m_SetShiftButton;
	CButton	m_SetFrequencyButton;
	CEdit	m_FrequencyCtrl;
	CButton	m_ContiniousWaveCtrl;
	CButton	m_OutputOnOffCheck;
	CSpinButtonCtrl	m_OutputLevelSpin;
	CSpinButtonCtrl	m_FrequencySpin;
	UINT	m_Frequency;
	BOOL	m_bOutputOn;
	BOOL	m_bContiniousWaveOn;
	double	m_OutputLevel;
	int		m_Shift;
	int		m_FineTune;
	//}}AFX_DATA

// Attributes
public:
	CModulatorCarrierForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CModulatorCarrierForm)
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();

	// Implementation
protected:
	virtual ~CModulatorCarrierForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:
	void UpdateBucStatus();

	// Protected members
protected:
	CModem *m_pModem;

	// Generated message map functions
	//{{AFX_MSG(CModulatorCarrierForm)
	afx_msg void OnSetFrequencyButton();
	afx_msg void OnSetOutputLevelButton();
	afx_msg void OnOutputOnOffCheck();
	afx_msg void OnDeltaPosOutputLevelSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCwCheck();
	afx_msg void OnSetShiftButton();
	afx_msg void OnSetFineTuneButton();
	afx_msg void OnSelChangeModulationTypeCombo();
	afx_msg void OnSelChangePowerSupplyCombo();
	afx_msg void OnSelChangeBuc10MHzCombo();
	afx_msg void OnSelChangeSpectrumCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __MODULATOR_CARRIER_FORM_H__INCLUDED__
