#if !defined(AFX_NODEFORM_H__57EF1581_F4B8_4AB8_9DB7_0468B3A70C5F__INCLUDED_)
#define AFX_NODEFORM_H__57EF1581_F4B8_4AB8_9DB7_0468B3A70C5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NodeForm.h : header file
//

// Forward declarations
class CMibNode;

/////////////////////////////////////////////////////////////////////////////
// CNodeForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CNodeForm : public CFormView
{
protected:
	CNodeForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNodeForm)

// Form Data
public:
	//{{AFX_DATA(CNodeForm)
	enum { IDD = IDD_NODE_FORM };
	CString	m_FullSpec;
	CString	m_FullDecimalSpec;
	CString	m_Description;
	//}}AFX_DATA

// Public methods
public:
	void SetupControls(CMibNode *pNode);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNodeForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CNodeForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Protected members
	CMibNode *m_pNode;

	// Generated message map functions
	//{{AFX_MSG(CNodeForm)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NODEFORM_H__57EF1581_F4B8_4AB8_9DB7_0468B3A70C5F__INCLUDED_)
