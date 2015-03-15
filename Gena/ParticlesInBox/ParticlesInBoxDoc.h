// ParticlesInBoxDoc.h : interface of the CParticlesInBoxDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLESINBOXDOC_H__83B445B1_F29B_4456_8D49_D7CC520FBB7D__INCLUDED_)
#define AFX_PARTICLESINBOXDOC_H__83B445B1_F29B_4456_8D49_D7CC520FBB7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "World.h"

class CParticlesInBoxDoc : public CDocument
{
protected: // create from serialization only
	CParticlesInBoxDoc();
	DECLARE_DYNCREATE(CParticlesInBoxDoc)

// Attributes
public:

// Operations
public:
	void OnIdle();
	CWorld *GetWorld() { return &m_World; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticlesInBoxDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CParticlesInBoxDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:
	CWorld m_World;

// Generated message map functions
protected:
	//{{AFX_MSG(CParticlesInBoxDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLESINBOXDOC_H__83B445B1_F29B_4456_8D49_D7CC520FBB7D__INCLUDED_)
