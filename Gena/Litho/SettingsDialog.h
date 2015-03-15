#if !defined(AFX_SETTINGSDIALOG_H__C56B1C09_ECFA_4377_AACA_ECFE5ACD71F0__INCLUDED_)
#define AFX_SETTINGSDIALOG_H__C56B1C09_ECFA_4377_AACA_ECFE5ACD71F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog dialog

class CSettingsDialog : public CDialog
{
// Construction
public:
	CSettingsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsDialog)
	enum { IDD = IDD_SETTINGS_DIALOG };
	double	m_Distance;
	double	m_WaveLength;
	double	m_MaskSize;
	BOOL	m_bPositiveMask;
	double	m_ImageAccuracy;
	double	m_ImageSize;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CSettingsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDIALOG_H__C56B1C09_ECFA_4377_AACA_ECFE5ACD71F0__INCLUDED_)
