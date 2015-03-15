// AllNetForm.h : header file
//

#ifndef __ALL_NET_FORM_H_INCLUDED__
#define __ALL_NET_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


////////////////////////////////////////////////////////////////////////////
// CAllNetForm form view

class CAllNetForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CAllNetForm)

// Form Data
public:
	//{{AFX_DATA(CAllNetForm)
	enum { IDD = IDD_ALL_NET_FORM };
	//}}AFX_DATA

// Attributes
public:
	CAllNetForm();

	// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CAllNetForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CAllNetForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Protected methods
protected:

// Protected members
protected:

	//{{AFX_MSG(CAllNetForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __ALL_NET_FORM_H_INCLUDED__
