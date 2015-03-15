#if !defined(AFX_IPSTATICROUTESFORM_H__77ED9D81_46AB_11D4_BBDF_FC2420967B6D__INCLUDED_)
#define AFX_IPSTATICROUTESFORM_H__77ED9D81_46AB_11D4_BBDF_FC2420967B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IPStaticRoutesForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIPStaticRoutesForm form view

class CNsgFrad;

class CIPStaticRoutesForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CIPStaticRoutesForm)

// Form Data
public:
	//{{AFX_DATA(CIPStaticRoutesForm)
	enum { IDD = IDD_IP_STATIC_ROUTES_FORM };
	CListCtrl	m_StaticRoutesCtrl;
	//}}AFX_DATA

// Attributes
public:
	CIPStaticRoutesForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CIPStaticRoutesForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CIPStaticRoutesForm();
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
	//{{AFX_MSG(CIPStaticRoutesForm)
	afx_msg void OnDeleteButton();
	afx_msg void OnModifyButton();
	afx_msg void OnAddButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPSTATICROUTESFORM_H__77ED9D81_46AB_11D4_BBDF_FC2420967B6D__INCLUDED_)
