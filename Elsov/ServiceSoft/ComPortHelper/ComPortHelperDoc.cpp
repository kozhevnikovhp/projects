// ComPortHelperDoc.cpp : implementation of the CComPortHelperDoc class
//

#include "stdafx.h"
#include "ComPortHelper.h"

#include "ComPortHelperDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperDoc

IMPLEMENT_DYNCREATE(CComPortHelperDoc, CDocument)

BEGIN_MESSAGE_MAP(CComPortHelperDoc, CDocument)
	//{{AFX_MSG_MAP(CComPortHelperDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperDoc construction/destruction

CComPortHelperDoc::CComPortHelperDoc()
{
	// TODO: add one-time construction code here

}

CComPortHelperDoc::~CComPortHelperDoc()
{
}

BOOL CComPortHelperDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CComPortHelperDoc serialization

void CComPortHelperDoc::Serialize(CArchive& ar)
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
// CComPortHelperDoc diagnostics

#ifdef _DEBUG
void CComPortHelperDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CComPortHelperDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperDoc commands
