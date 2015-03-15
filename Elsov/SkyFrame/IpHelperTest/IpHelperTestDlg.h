// IpHelperTestDlg.h : header file
//

#if !defined(AFX_IPHELPERTESTDLG_H__BB123541_0225_4C93_B3E0_ED27895D1821__INCLUDED_)
#define AFX_IPHELPERTESTDLG_H__BB123541_0225_4C93_B3E0_ED27895D1821__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IpHelper.h"

/////////////////////////////////////////////////////////////////////////////
// CIpHelperTestDlg dialog

class CIpHelperTestDlg : public CDialog
{
// Construction
public:
	CIpHelperTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIpHelperTestDlg)
	enum { IDD = IDD_IPHELPERTEST_DIALOG };
	CComboBox	m_AdaptersIndexCombo;
	CIPAddressCtrl	m_PingIpAddressCtrl;
	CIPAddressCtrl	m_SubnetMaskCtrl;
	CIPAddressCtrl	m_IpAddressCtrl;
	CComboBox	m_IpAddressIndexCombo;
	BOOL	m_bLocalAddress;
	CString	m_MacAddress;
	CString	m_InterfaceDesc;
	CString	m_AdapterName;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CIpHelperTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CIpHelper m_Helper;

	//{{AFX_MSG(CIpHelperTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnInitializeButton();
	afx_msg void OnSelChangeIpAddressIndexCombo();
	afx_msg void OnRunPingButton();
	afx_msg void OnAddIpAddressButton();
	afx_msg void OnSelChangeAdaptersIndexCombo();
	afx_msg void OnAboutButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPHELPERTESTDLG_H__BB123541_0225_4C93_B3E0_ED27895D1821__INCLUDED_)
