#if !defined(AFX_PARTICLEPOSDIALOG_H__1F050BDF_5001_421B_8AE2_184A6E2E6FDE__INCLUDED_)
#define AFX_PARTICLEPOSDIALOG_H__1F050BDF_5001_421B_8AE2_184A6E2E6FDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParticlePosDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParticlePosDialog dialog

class CParticlePosDialog : public CDialog
{
// Construction
public:
	CParticlePosDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParticlePosDialog)
	enum { IDD = IDD_PARTICLE_POS_DIALOG };
	double	m_X;
	double	m_Y;
	double	m_Fi;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticlePosDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParticlePosDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLEPOSDIALOG_H__1F050BDF_5001_421B_8AE2_184A6E2E6FDE__INCLUDED_)
