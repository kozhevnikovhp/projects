// GLTestDoc.cpp : implementation of the CGLTestDoc class
//

#include "stdafx.h"
#include "GLTest.h"

#include "GLTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGLTestDoc

IMPLEMENT_DYNCREATE(CGLTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CGLTestDoc, CDocument)
	//{{AFX_MSG_MAP(CGLTestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLTestDoc construction/destruction

CGLTestDoc::CGLTestDoc()
{
	// TODO: add one-time construction code here

}

CGLTestDoc::~CGLTestDoc()
{
}

BOOL CGLTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CGLTestDoc serialization

void CGLTestDoc::Serialize(CArchive& ar)
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
// CGLTestDoc diagnostics

#ifdef _DEBUG
void CGLTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGLTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGLTestDoc commands
