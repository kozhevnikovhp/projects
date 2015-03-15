#if !defined(AFX_NEWCONTROLTARGETDIALOG_H__DA4886C0_A467_42BC_8E3D_5304EEFD7A5E__INCLUDED_)
#define AFX_NEWCONTROLTARGETDIALOG_H__DA4886C0_A467_42BC_8E3D_5304EEFD7A5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// NewControlTargetDialog.h : header file
//

// Forward declarations
class IControlTargetAPI;

/////////////////////////////////////////////////////////////////////////////
// CNewControlTargetDialog dialog

class CNewControlTargetDialog : public CDialog
{
// Construction
public:
	CNewControlTargetDialog(CWnd* pParent = NULL);   // standard constructor
	CString GetLibraryFileName();

// Dialog Data
	//{{AFX_DATA(CNewControlTargetDialog)
	enum { IDD = IDD_NEW_CONTROL_TARGET_DIALOG };
	CListBox	m_TargetsListbox;
	//}}AFX_DATA
	CString m_FileName;


// Overrides
	//{{AFX_VIRTUAL(CNewControlTargetDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CNewControlTargetDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeTargetListbox();
	afx_msg void OnDestroy();
	afx_msg void OnDblclkTargetListbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCONTROLTARGETDIALOG_H__DA4886C0_A467_42BC_8E3D_5304EEFD7A5E__INCLUDED_)
