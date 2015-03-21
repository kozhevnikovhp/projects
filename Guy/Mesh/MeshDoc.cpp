// MeshDoc.cpp : implementation of the CMeshDoc class
//

#include "stdafx.h"
#include "Mesh.h"

#include "MeshDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeshDoc

IMPLEMENT_DYNCREATE(CMeshDoc, CDocument)

BEGIN_MESSAGE_MAP(CMeshDoc, CDocument)
	//{{AFX_MSG_MAP(CMeshDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshDoc construction/destruction

CMeshDoc::CMeshDoc()
{
	// TODO: add one-time construction code here

}

CMeshDoc::~CMeshDoc()
{
}

BOOL CMeshDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMeshDoc serialization

void CMeshDoc::Serialize(CArchive& ar)
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
// CMeshDoc diagnostics

#ifdef _DEBUG
void CMeshDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMeshDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMeshDoc commands
