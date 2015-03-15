// TitovDoc.cpp : implementation of the CTitovDoc class
//

#include "stdafx.h"
#include "Titov.h"

#include "TitovDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitovDoc

IMPLEMENT_DYNCREATE(CTitovDoc, CDocument)

BEGIN_MESSAGE_MAP(CTitovDoc, CDocument)
	//{{AFX_MSG_MAP(CTitovDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTitovDoc construction/destruction

CTitovDoc::CTitovDoc()
{
	// TODO: add one-time construction code here

}

CTitovDoc::~CTitovDoc()
{
}

BOOL CTitovDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CTitovApp *pApp = (CTitovApp *)AfxGetApp();
	pApp->m_pDoc = this;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTitovDoc serialization

void CTitovDoc::Serialize(CArchive& ar)
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
// CTitovDoc diagnostics

#ifdef _DEBUG
void CTitovDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTitovDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTitovDoc commands
