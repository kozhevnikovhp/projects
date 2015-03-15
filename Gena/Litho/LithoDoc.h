// LithoDoc.h : interface of the CLithoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __LITHO_DOC_H_INCLUDED__
#define __LITHO_DOC_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CLithoDoc : public CDocument
{
protected: // create from serialization only
	CLithoDoc();
	DECLARE_DYNCREATE(CLithoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CLithoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive &ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLithoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLithoDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __LITHO_DOC_H_INCLUDED__