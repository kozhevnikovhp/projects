// MibViewerDoc.h : interface of the CMibViewerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIBVIEWERDOC_H__8B28B374_7549_46D2_AF4B_24DCE01A25B0__INCLUDED_)
#define AFX_MIBVIEWERDOC_H__8B28B374_7549_46D2_AF4B_24DCE01A25B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MibParser.h"

// Forward declarations
class CMibViewerView;

class CMibViewerDoc : public CDocument
{
protected: // create from serialization only
	CMibViewerDoc();
	DECLARE_DYNCREATE(CMibViewerDoc)

// Public methods
	void SetMibTreeView(CMibViewerView *pView) { m_pTreeView = pView; }

public:

// Overrides
	//{{AFX_VIRTUAL(CMibViewerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMibViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	CMibTree m_Tree;
	CMibViewerView *m_pTreeView;
	
	//{{AFX_MSG(CMibViewerDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIBVIEWERDOC_H__8B28B374_7549_46D2_AF4B_24DCE01A25B0__INCLUDED_)
