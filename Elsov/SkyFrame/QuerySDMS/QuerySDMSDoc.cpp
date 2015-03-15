// QuerySDMSDoc.cpp : implementation of the CQuerySDMSDoc class
//

#include "stdafx.h"
#include "QuerySDMS.h"

#include "QuerySDMSDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSDoc

IMPLEMENT_DYNCREATE(CQuerySDMSDoc, CDocument)

BEGIN_MESSAGE_MAP(CQuerySDMSDoc, CDocument)
	//{{AFX_MSG_MAP(CQuerySDMSDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSDoc construction/destruction

CQuerySDMSDoc::CQuerySDMSDoc()
{
	// TODO: add one-time construction code here

}

CQuerySDMSDoc::~CQuerySDMSDoc()
{
}

BOOL CQuerySDMSDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSDoc serialization

void CQuerySDMSDoc::Serialize(CArchive& ar)
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
// CQuerySDMSDoc diagnostics

#ifdef _DEBUG
void CQuerySDMSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CQuerySDMSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQuerySDMSDoc commands
