// MeshDoc.h : interface of the CMeshDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHDOC_H__149F2515_C84F_44A8_BFAC_A331BAAD67CE__INCLUDED_)
#define AFX_MESHDOC_H__149F2515_C84F_44A8_BFAC_A331BAAD67CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMeshDoc : public CDocument
{
protected: // create from serialization only
	CMeshDoc();
	DECLARE_DYNCREATE(CMeshDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeshDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMeshDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMeshDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHDOC_H__149F2515_C84F_44A8_BFAC_A331BAAD67CE__INCLUDED_)
