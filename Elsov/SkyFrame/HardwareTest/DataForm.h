#if !defined(AFX_DATAFORM_H__9BD13480_31A9_11D4_BBDF_CDE359D3E5B3__INCLUDED_)
#define AFX_DATAFORM_H__9BD13480_31A9_11D4_BBDF_CDE359D3E5B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataForm form view

#include "Modem.h"
#include "ModemComboBox.h"

class CDataForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CDataForm)

// Form Data
public:
	//{{AFX_DATA(CDataForm)
	enum { IDD = IDD_DATA_FORM };
	cDifferentialModeCombo	m_DifferentialCombo;
	cFecModeCombo	m_FecModeCombo;
	cClockSourceComboBox	m_ClockSourceCombo;
	cScramblerCombo	m_ScramblerModeCombo;
	CButton	m_DataInvCheck;
	CButton	m_DataClockInvCheck;
	CSpinButtonCtrl	m_DataRateSpin;
	UINT	m_DataRate;
	BOOL	m_bDataInvEnabled;
	BOOL	m_bDataClockInvEnabled;
	CString	m_HeaderString;
	UINT	m_DopplerBufferSize;
	//}}AFX_DATA

// Attributes
public:
	CDataForm();
	CDataForm(UINT nIDTemplate);

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CDataForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Overridables
public:

// Implementation
protected:
	virtual ~CDataForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:
	void SetControls(CModemDataParams &Params);
	virtual BOOL CanDataRate() { return m_pModem->CanDataRate() && m_pModem->IsLocallyControlled(); }
	virtual BOOL CanDataClockInternal() { return FALSE; }
	virtual BOOL CanDataClockExternal() { return FALSE; }
	virtual BOOL CanDataClockInversion() { return FALSE; }
	virtual BOOL CanDiff() { return FALSE; }
	virtual BOOL CanScrambleDescramble() { return FALSE; }
	virtual BOOL CanDataInversion() { return FALSE; }

	// Protected members
protected:
	CModem *m_pModem;

	// Generated message map functions
	//{{AFX_MSG(CDataForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAFORM_H__9BD13480_31A9_11D4_BBDF_CDE359D3E5B3__INCLUDED_)
