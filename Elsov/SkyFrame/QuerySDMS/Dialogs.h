#ifndef __DIALOGS_H_INCLUDED__
#define __DIALOGS_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialogs.h : header file
//

BOOL ChangeUnsignedIntValue(unsigned int &value, unsigned int min_value, unsigned int max_value);
BOOL ChangeIntValue(int &value, int min_value, int max_value);
BOOL ChangeDoubleValue(double &value, double min_value, double max_value);
BOOL ChangeModulation(BOOL &bBPSK);
BOOL OnOff(BOOL &bOFF);
BOOL ChangeSpectrum(BOOL &bNormal);
BOOL ChangeCodeRate(int &CodeRate);
BOOL ChangeScrambler(int &Scrambler);

/////////////////////////////////////////////////////////////////////////////
// CUnsignedIntDialog dialog

class CUnsignedIntDialog : public CDialog
{
// Construction
public:
	CUnsignedIntDialog(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUnsignedIntDialog)
	enum { IDD = IDD_UINT_DIALOG };
	CSpinButtonCtrl	m_Spin;
	UINT	m_Value;
	//}}AFX_DATA
	UINT m_Min, m_Max;


// Overrides
	//{{AFX_VIRTUAL(CUnsignedIntDialog)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CUnsignedIntDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CIntDialog dialog

class CIntDialog : public CDialog
{
// Construction
public:
	CIntDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIntDialog)
	enum { IDD = IDD_INT_DIALOG };
	CSpinButtonCtrl	m_Spin;
	int		m_Value;
	//}}AFX_DATA
	int m_Min, m_Max;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIntDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIntDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CDoubleDialog dialog

class CDoubleDialog : public CDialog
{
// Construction
public:
	CDoubleDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDoubleDialog)
	enum { IDD = IDD_DOUBLE_DIALOG };
	CSpinButtonCtrl	m_Spin;
	double	m_Value;
	//}}AFX_DATA
	double m_Min, m_Max;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDoubleDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDoubleDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CModulationDialog dialog

class CModulationDialog : public CDialog
{
// Construction
public:
	CModulationDialog(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModulationDialog)
	enum { IDD = IDD_MODULATION_DIALOG };
	//}}AFX_DATA
	BOOL m_bBPSK;


// Overrides
	//{{AFX_VIRTUAL(CModulationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CModulationDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnBpskRadio();
	afx_msg void OnQpskRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COnOffDialog dialog

class COnOffDialog : public CDialog
{
// Construction
public:
	COnOffDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COnOffDialog)
	enum { IDD = IDD_ONOFF_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	BOOL m_bOFF;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnOffDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COnOffDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnOffRadio();
	afx_msg void OnOnRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSpectrumDialog dialog

class CSpectrumDialog : public CDialog
{
// Construction
public:
	CSpectrumDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpectrumDialog)
	enum { IDD = IDD_SPECTRUM_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	BOOL m_bNormal;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpectrumDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpectrumDialog)
	afx_msg void OnNormalRadio();
	afx_msg void OnInvertedRadio();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CCodeRateDialog dialog

class CCodeRateDialog : public CDialog
{
// Construction
public:
	CCodeRateDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCodeRateDialog)
	enum { IDD = IDD_CODERATE_DIALOG };
	//}}AFX_DATA
	int m_CodeRate;


// Overrides
	//{{AFX_VIRTUAL(CCodeRateDialog)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCodeRateDialog)
	afx_msg void On12Radio();
	afx_msg void On34Radio();
	afx_msg void On78Radio();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CScramblerDialog dialog

class CScramblerDialog : public CDialog
{
// Construction
public:
	CScramblerDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScramblerDialog)
	enum { IDD = IDD_SCRAMBLER_DIALOG };
	//}}AFX_DATA
	int m_Scrambler;


// Overrides
	//{{AFX_VIRTUAL(CScramblerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScramblerDialog)
	afx_msg void OnDisableRadio();
	afx_msg void OnAltV35Radio();
	afx_msg void OnAltIntelsatRadio();
	afx_msg void OnIess308Radio();
	afx_msg void OnIess309Radio();
	afx_msg void OnIntelsatRadio();
	afx_msg void OnTpcSyncRadio();
	afx_msg void OnV35Radio();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __DIALOGS_H_INCLUDED__
