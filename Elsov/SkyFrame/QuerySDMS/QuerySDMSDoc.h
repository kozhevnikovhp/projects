// QuerySDMSDoc.h : interface of the CQuerySDMSDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYSDMSDOC_H__6F94FF44_6257_4A0E_A329_0913284C40F4__INCLUDED_)
#define AFX_QUERYSDMSDOC_H__6F94FF44_6257_4A0E_A329_0913284C40F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CQuerySDMSDoc : public CDocument
{
protected: // create from serialization only
	CQuerySDMSDoc();
	DECLARE_DYNCREATE(CQuerySDMSDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuerySDMSDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQuerySDMSDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CQuerySDMSDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYSDMSDOC_H__6F94FF44_6257_4A0E_A329_0913284C40F4__INCLUDED_)
