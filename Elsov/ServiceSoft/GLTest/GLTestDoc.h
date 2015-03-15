// GLTestDoc.h : interface of the CGLTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLTESTDOC_H__44E7880F_38BB_49B1_8FF3_B5F027748216__INCLUDED_)
#define AFX_GLTESTDOC_H__44E7880F_38BB_49B1_8FF3_B5F027748216__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGLTestDoc : public CDocument
{
protected: // create from serialization only
	CGLTestDoc();
	DECLARE_DYNCREATE(CGLTestDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLTestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGLTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGLTestDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLTESTDOC_H__44E7880F_38BB_49B1_8FF3_B5F027748216__INCLUDED_)
