// GranSim2D_TestDoc.cpp : implementation of the CGranSim2D_TestDoc class
//

#include "stdafx.h"
#include "GranSim2D_Test.h"

#include "GranSim2D_TestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestDoc

IMPLEMENT_DYNCREATE(CGranSim2D_TestDoc, CDocument)

BEGIN_MESSAGE_MAP(CGranSim2D_TestDoc, CDocument)
	//{{AFX_MSG_MAP(CGranSim2D_TestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestDoc construction/destruction

CGranSim2D_TestDoc::CGranSim2D_TestDoc()
{
	// TODO: add one-time construction code here

}

CGranSim2D_TestDoc::~CGranSim2D_TestDoc()
{
}

BOOL CGranSim2D_TestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestDoc serialization

void CGranSim2D_TestDoc::Serialize(CArchive& ar)
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
// CGranSim2D_TestDoc diagnostics

#ifdef _DEBUG
void CGranSim2D_TestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGranSim2D_TestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGranSim2D_TestDoc commands
