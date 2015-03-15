// PointToPointDoc.h : interface of the CPointToPointDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTTOPOINTDOC_H__0DA57542_93B7_474F_87A1_D13B863BFF31__INCLUDED_)
#define AFX_POINTTOPOINTDOC_H__0DA57542_93B7_474F_87A1_D13B863BFF31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPointToPointDoc : public CDocument
{
protected: // create from serialization only
	CPointToPointDoc();
	DECLARE_DYNCREATE(CPointToPointDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointToPointDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPointToPointDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPointToPointDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTTOPOINTDOC_H__0DA57542_93B7_474F_87A1_D13B863BFF31__INCLUDED_)
