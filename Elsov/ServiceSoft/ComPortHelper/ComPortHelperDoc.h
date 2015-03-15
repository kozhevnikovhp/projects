// ComPortHelperDoc.h : interface of the CComPortHelperDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPORTHELPERDOC_H__5861FC9B_027A_4DBD_BC47_7CE9A608348D__INCLUDED_)
#define AFX_COMPORTHELPERDOC_H__5861FC9B_027A_4DBD_BC47_7CE9A608348D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CComPortHelperDoc : public CDocument
{
protected: // create from serialization only
	CComPortHelperDoc();
	DECLARE_DYNCREATE(CComPortHelperDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComPortHelperDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComPortHelperDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CComPortHelperDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPORTHELPERDOC_H__5861FC9B_027A_4DBD_BC47_7CE9A608348D__INCLUDED_)
