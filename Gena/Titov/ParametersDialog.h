#if !defined(AFX_PAREMETERSDIALOG_H__8CB21F9D_6F05_4BFB_A648_8ABE97F1609B__INCLUDED_)
#define AFX_PAREMETERSDIALOG_H__8CB21F9D_6F05_4BFB_A648_8ABE97F1609B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParametersDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParametersDialog dialog

class CParametersDialog : public CDialog
{
// Construction
public:
	CParametersDialog(CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParametersDialog)
	enum { IDD = IDD_PARAMETERS_DIALOG };
	int		m_nParticles;
	double	m_G;
	double	m_LossRatio;
	double	m_RParticle;
	double	m_Xleft;
	double	m_Xright;
	double	m_Ybottom;
	double	m_Ytop;
	double	m_WallXPosition;
	double	m_HolePosition;
	double	m_HoleSize;
	double	m_Vinit;
	double	m_BounceBottom;
	double	m_BounceTop;
	double	m_BounceSide;
	CString	m_FileName;
	//}}AFX_DATA
	BOOL m_bFileNameSpecified;

// Overrides
	//{{AFX_VIRTUAL(CParametersDialog)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CParametersDialog)
	afx_msg void OnBrowseButton();
	virtual void OnOK();
	afx_msg void OnAboutButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAREMETERSDIALOG_H__8CB21F9D_6F05_4BFB_A648_8ABE97F1609B__INCLUDED_)
