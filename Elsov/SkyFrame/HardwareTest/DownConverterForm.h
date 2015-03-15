#ifndef __DOWN_CONVERTER_FORM_H__INCLUDED__
#define __DOWN_CONVERTER_FORM_H__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DownConverterForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDownConverterForm form view

#include "UpDownConverter.h"

class CDownConverterForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CDownConverterForm)

// Form Data
public:
	//{{AFX_DATA(CUpConverterForm)
	enum { IDD = IDD_DOWN_CONVERTER_FORM };
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
	CDownConverterForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CDownConverterForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();

	// Implementation
protected:
	virtual ~CDownConverterForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

	// Protected members
protected:
	CUpDownConverter *m_pConverter;

	//{{AFX_MSG(CDownConverterForm)
	afx_msg void OnOnOffCheck();
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

#endif // __DOWN_CONVERTER_FORM_H__INCLUDED__
