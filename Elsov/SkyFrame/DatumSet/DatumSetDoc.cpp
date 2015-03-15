// DatumSetDoc.cpp : implementation of the CDatumSetDoc class
//

#include "stdafx.h"
#include "DatumSet.h"

#include "DatumSetDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatumSetDoc

IMPLEMENT_DYNCREATE(CDatumSetDoc, CDocument)

BEGIN_MESSAGE_MAP(CDatumSetDoc, CDocument)
	//{{AFX_MSG_MAP(CDatumSetDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatumSetDoc construction/destruction

CDatumSetDoc::CDatumSetDoc()
{
	// TODO: add one-time construction code here

}

CDatumSetDoc::~CDatumSetDoc()
{
}

BOOL CDatumSetDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDatumSetDoc serialization

void CDatumSetDoc::Serialize(CArchive& ar)
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
// CDatumSetDoc diagnostics

#ifdef _DEBUG
void CDatumSetDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDatumSetDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDatumSetDoc commands
