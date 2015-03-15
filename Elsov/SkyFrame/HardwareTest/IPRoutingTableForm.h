#if !defined(AFX_IPROUTINGTABLEFORM_H__77ED9D80_46AB_11D4_BBDF_FC2420967B6D__INCLUDED_)
#define AFX_IPROUTINGTABLEFORM_H__77ED9D80_46AB_11D4_BBDF_FC2420967B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IPRoutingTableForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIPRoutingTableForm form view

class CNsgFrad;

class CIPRoutingTableForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CIPRoutingTableForm)

// Form Data
public:
	//{{AFX_DATA(CIPRoutingTableForm)
	enum { IDD = IDD_IP_ROUTING_TABLE_FORM };
	CString	m_TableString;
	CString	m_DefaultGW;
	//}}AFX_DATA

// Attributes
public:
	CIPRoutingTableForm(); 

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPRoutingTableForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual int DeltaTimerTime() { return 5; }

	// Implementation
protected:
	virtual ~CIPRoutingTableForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:
	void RefreshTable();

	// Protected members
protected:
	CNsgFrad *m_pNsgFrad;

	// Generated message map functions
	//{{AFX_MSG(CIPRoutingTableForm)
	afx_msg void OnSetDefaulGatewayButton();
	afx_msg void OnWarmButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPROUTINGTABLEFORM_H__77ED9D80_46AB_11D4_BBDF_FC2420967B6D__INCLUDED_)
