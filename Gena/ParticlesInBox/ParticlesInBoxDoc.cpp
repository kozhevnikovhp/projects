// ParticlesInBoxDoc.cpp : implementation of the CParticlesInBoxDoc class
//

#include "stdafx.h"
#include "ParticlesInBox.h"

#include "ParticlesInBoxDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxDoc

IMPLEMENT_DYNCREATE(CParticlesInBoxDoc, CDocument)

BEGIN_MESSAGE_MAP(CParticlesInBoxDoc, CDocument)
	//{{AFX_MSG_MAP(CParticlesInBoxDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxDoc construction/destruction

CParticlesInBoxDoc::CParticlesInBoxDoc()
{
	// TODO: add one-time construction code here

}

CParticlesInBoxDoc::~CParticlesInBoxDoc()
{
}

BOOL CParticlesInBoxDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if (!m_World.InitialDistribution())
		return FALSE;

	return TRUE;
}

void CParticlesInBoxDoc::OnIdle()
{
	m_World.OnIdle();
//	UpdateAllViews(NULL);
}



/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxDoc serialization

void CParticlesInBoxDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxDoc diagnostics

#ifdef _DEBUG
void CParticlesInBoxDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CParticlesInBoxDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParticlesInBoxDoc commands
