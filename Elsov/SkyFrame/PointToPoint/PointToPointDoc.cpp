// PointToPointDoc.cpp : implementation of the CPointToPointDoc class
//

#include "stdafx.h"
#include "PointToPoint.h"

#include "PointToPointDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointToPointDoc

IMPLEMENT_DYNCREATE(CPointToPointDoc, CDocument)

BEGIN_MESSAGE_MAP(CPointToPointDoc, CDocument)
	//{{AFX_MSG_MAP(CPointToPointDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointToPointDoc construction/destruction

CPointToPointDoc::CPointToPointDoc()
{
	// TODO: add one-time construction code here

}

CPointToPointDoc::~CPointToPointDoc()
{
}

BOOL CPointToPointDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPointToPointDoc serialization

void CPointToPointDoc::Serialize(CArchive& ar)
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
// CPointToPointDoc diagnostics

#ifdef _DEBUG
void CPointToPointDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPointToPointDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPointToPointDoc commands
