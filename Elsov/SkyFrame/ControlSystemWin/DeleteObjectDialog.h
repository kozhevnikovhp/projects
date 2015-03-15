#if !defined(AFX_DELETEOBJECTDIALOG_H__0CB83007_B571_43DD_A210_F8F9BD28B814__INCLUDED_)
#define AFX_DELETEOBJECTDIALOG_H__0CB83007_B571_43DD_A210_F8F9BD28B814__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeleteObjectDialog.h : header file
//
// Forward declarations
class CControlObject;

enum DeletionTypeEnum
{
	DT_DELETE,
	DT_DESTROY,
	DT_UNKNOWN
};

/////////////////////////////////////////////////////////////////////////////
// CDeleteObjectDialog dialog

class CDeleteObjectDialog : public CDialog
{
// Construction
public:
	CDeleteObjectDialog(CControlObject *pObject, CWnd *pParent = NULL);   // standard constructor
	DeletionTypeEnum GetDeletionType() { return m_DeletionType; }

// Dialog Data
	//{{AFX_DATA(CDeleteObjectDialog)
	enum { IDD = IDD_DELETE_OBJECT_DIALOG };
	//}}AFX_DATA
	CControlObject *m_pObject;
	DeletionTypeEnum m_DeletionType;

// Overrides
	//{{AFX_VIRTUAL(CDeleteObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CDeleteObjectDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEOBJECTDIALOG_H__0CB83007_B571_43DD_A210_F8F9BD28B814__INCLUDED_)
