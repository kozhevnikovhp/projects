#if !defined(AFX_PVCPROPERTIESDIALOG_H__365F9920_4B62_11D4_BBDF_93062C648B7F__INCLUDED_)
#define AFX_PVCPROPERTIESDIALOG_H__365F9920_4B62_11D4_BBDF_93062C648B7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PvcPropertiesDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPvcPropertiesDialog dialog

class CPvcPropertiesDialog : public CDialog
{
// Construction
public:
	CPvcPropertiesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPvcPropertiesDialog)
	enum { IDD = IDD_PVC_PROPERTIES_DIALOG };
	CString	m_Port1;
	CString	m_Port2;
	CString	m_Channel1;
	CString	m_Channel2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPvcPropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPvcPropertiesDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PVCPROPERTIESDIALOG_H__365F9920_4B62_11D4_BBDF_93062C648B7F__INCLUDED_)
