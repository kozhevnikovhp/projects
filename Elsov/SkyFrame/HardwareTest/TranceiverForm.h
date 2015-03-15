#if !defined(AFX_TRANCEIVERFORM_H__060F523C_5696_43A4_A0A0_0613CB0C9565__INCLUDED_)
#define AFX_TRANCEIVERFORM_H__060F523C_5696_43A4_A0A0_0613CB0C9565__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TranceiverForm.h : header file
//

// Forward declarations
class CTranceiver;

/////////////////////////////////////////////////////////////////////////////
// CTranceiverForm form view

class CTranceiverForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CTranceiverForm)

// Form Data
public:
	//{{AFX_DATA(CTranceiverForm)
	enum { IDD = IDD_TRANCEIVER_FORM };
	CEdit	m_GainEdit;
	CStatic	m_AlarmWnd;
	CEdit	m_FrequencyEdit;
	CButton	m_bOutputControl;
	BOOL	m_bOutputEnabled;
	UINT	m_Frequency;
	double	m_Gain;
	//}}AFX_DATA

// Attributes
public:
	CTranceiverForm(); 
// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CTranceiverForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();

// Implementation
protected:
	virtual ~CTranceiverForm();
	void UpdateAllFields();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected members
protected:
	CTranceiver *m_pTranceiver;

	//{{AFX_MSG(CTranceiverForm)
	afx_msg void OnOutputOnOffCheck();
	afx_msg void OnSetButton();
	afx_msg void OnSetGainButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANCEIVERFORM_H__060F523C_5696_43A4_A0A0_0613CB0C9565__INCLUDED_)
