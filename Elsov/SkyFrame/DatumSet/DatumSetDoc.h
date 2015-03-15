// DatumSetDoc.h : interface of the CDatumSetDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATUMSETDOC_H__44E8537C_82C1_46D3_BCBD_1F93A8E2B433__INCLUDED_)
#define AFX_DATUMSETDOC_H__44E8537C_82C1_46D3_BCBD_1F93A8E2B433__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDatumSetDoc : public CDocument
{
protected: // create from serialization only
	CDatumSetDoc();
	DECLARE_DYNCREATE(CDatumSetDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatumSetDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDatumSetDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDatumSetDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATUMSETDOC_H__44E8537C_82C1_46D3_BCBD_1F93A8E2B433__INCLUDED_)
