// LithoDoc.cpp : implementation of the CLithoDoc class
//

#include "stdafx.h"
#include "Litho.h"

#include "LithoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLithoDoc

IMPLEMENT_DYNCREATE(CLithoDoc, CDocument)

BEGIN_MESSAGE_MAP(CLithoDoc, CDocument)
	//{{AFX_MSG_MAP(CLithoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLithoDoc construction/destruction

CLithoDoc::CLithoDoc()
{
	// TODO: add one-time construction code here

}

CLithoDoc::~CLithoDoc()
{
}

BOOL CLithoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CLithoDoc serialization

void CLithoDoc::Serialize(CArchive& ar)
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
// CLithoDoc diagnostics

#ifdef _DEBUG
void CLithoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLithoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLithoDoc commands
