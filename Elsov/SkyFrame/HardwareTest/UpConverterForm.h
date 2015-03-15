#ifndef __UP_CONVERTER_FORM_H__INCLUDED__
#define __UP_CONVERTER_FORM_H__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// UpConverterForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUpConverterForm form view

#include "UpDownConverter.h"

class CUpConverterForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CUpConverterForm)

// Form Data
public:
	//{{AFX_DATA(CUpConverterForm)
	enum { IDD = IDD_UP_CONVERTER_FORM };
	CButton	m_SetGainLossButton;
	CEdit	m_GainLossEdit;
	CEdit	m_OutputAttenuatorEdit;
	CButton	m_SetOutputAttenuatorButton;
	CButton	m_SetInputAttenuatorButton;
	CEdit	m_InputAttenuatorEdit;
	CButton	m_SetFrequencyButton;
	CButton	m_OnOffCheck;
	CButton	m_SpectralInvCheck;
	CSpinButtonCtrl	m_FrequencySpin;
	BOOL	m_bOn;
	BOOL	m_bSpectralInvEnabled;
	UINT	m_Frequency;
	double	m_InputAttenuator;
	double	m_OutputAttenuator;
	double	m_GainLoss;
	//}}AFX_DATA

// Attributes
public:
	CUpConverterForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CUpConverterForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();

	// Implementation
protected:
	virtual ~CUpConverterForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Protected methods
protected:

	// Protected members
protected:
	CUpDownConverter *m_pConverter;

	//{{AFX_MSG(CUpConverterForm)
	afx_msg void OnOutputOnOffCheck();
	afx_msg void OnSpectralInvCheck();
	afx_msg void OnSetFrequencyButton();
	afx_msg void OnSetGainLossButton();
	afx_msg void OnSetOutputAttButton();
	afx_msg void OnSetInputAttButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __UP_CONVERTER_FORM_H__INCLUDED__
