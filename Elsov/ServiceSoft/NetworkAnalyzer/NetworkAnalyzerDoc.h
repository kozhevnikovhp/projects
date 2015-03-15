// NetworkAnalyzerDoc.h : interface of the CNetworkAnalyzerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKANALYZERDOC_H__7E0B222F_6DD1_4DBF_B530_44C289C21EDF__INCLUDED_)
#define AFX_NETWORKANALYZERDOC_H__7E0B222F_6DD1_4DBF_B530_44C289C21EDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetworkDocAPI.h"

// Forward declarations
class CNetworkAnalyzerView;


class CNetworkAnalyzerDoc : public CNetworkDocAPI
{
protected: // create from serialization only
	CNetworkAnalyzerDoc();
	DECLARE_DYNCREATE(CNetworkAnalyzerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CNetworkAnalyzerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive &ar);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetworkAnalyzerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CNetworkAnalyzerDoc)
	afx_msg void OnTestPing();
	afx_msg void OnTestBroadcastPing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETWORKANALYZERDOC_H__7E0B222F_6DD1_4DBF_B530_44C289C21EDF__INCLUDED_)
