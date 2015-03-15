#if !defined(AFX_STATICROUTEPROPERTIESDIALOG_H__92E8C780_46DE_11D4_BBDF_D5D5B4FD5F27__INCLUDED_)
#define AFX_STATICROUTEPROPERTIESDIALOG_H__92E8C780_46DE_11D4_BBDF_D5D5B4FD5F27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticRoutePropertiesDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStaticRoutePropertiesDialog dialog

class CStaticRoutePropertiesDialog : public CDialog
{
// Construction
public:
	CStaticRoutePropertiesDialog(char *pszNet, char *pszMask, char *pszGateway, char *pszMetric, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStaticRoutePropertiesDialog)
	enum { IDD = IDD_STATIC_ROUTE_PROPERTIES_DIALOG };
	UINT	m_Metric;
	CString	m_GW;
	CString	m_Mask;
	CString	m_Net;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticRoutePropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	char *m_pszNet;
	char *m_pszMask;
	char *m_pszGateway;
	char *m_pszMetric;

	// Generated message map functions
	//{{AFX_MSG(CStaticRoutePropertiesDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICROUTEPROPERTIESDIALOG_H__92E8C780_46DE_11D4_BBDF_D5D5B4FD5F27__INCLUDED_)
