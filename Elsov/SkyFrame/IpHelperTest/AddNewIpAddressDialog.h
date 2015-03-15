#if !defined(AFX_ADDNEWIPADDRESSDIALOG_H__6DF8B51B_C3CD_4173_B393_043EA41D4777__INCLUDED_)
#define AFX_ADDNEWIPADDRESSDIALOG_H__6DF8B51B_C3CD_4173_B393_043EA41D4777__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddNewIpAddressDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddNewIpAddressDialog dialog

class CAddNewIpAddressDialog : public CDialog
{
// Construction
public:
	CAddNewIpAddressDialog(CWnd* pParent = NULL);   // standard constructor
	DWORD GetIpAddress() { return m_IpAddress; }
	DWORD GetIpSubnetMask() { return m_IpSubnetMask; }

// Dialog Data
	//{{AFX_DATA(CAddNewIpAddressDialog)
	enum { IDD = IDD_ADD_IPADDRESS_DIALOG };
	CIPAddressCtrl	m_IpSubnetMaskCtrl;
	CIPAddressCtrl	m_IpAddressCtrl;
	//}}AFX_DATA
	DWORD m_IpAddress, m_IpSubnetMask;


// Overrides
	//{{AFX_VIRTUAL(CAddNewIpAddressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAddNewIpAddressDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDNEWIPADDRESSDIALOG_H__6DF8B51B_C3CD_4173_B393_043EA41D4777__INCLUDED_)
