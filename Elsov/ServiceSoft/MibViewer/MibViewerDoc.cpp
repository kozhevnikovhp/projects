// MibViewerDoc.cpp : implementation of the CMibViewerDoc class
//

#include "stdafx.h"
#include "MibViewer.h"

#include "MibViewerDoc.h"
#include "MibViewerDocView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMibViewerDoc

IMPLEMENT_DYNCREATE(CMibViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CMibViewerDoc, CDocument)
	//{{AFX_MSG_MAP(CMibViewerDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMibViewerDoc construction/destruction

CMibViewerDoc::CMibViewerDoc()
{
	// TODO: add one-time construction code here

}

CMibViewerDoc::~CMibViewerDoc()
{
}

BOOL CMibViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;


	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMibViewerDoc diagnostics

#ifdef _DEBUG
void CMibViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMibViewerDoc::Dump(CDumpContext &dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMibViewerDoc commands

BOOL CMibViewerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	if (IsModified())
		TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");

	DeleteContents();

	TRY
	{
		CMibFileParser parser;
		parser.ParseMib(lpszPathName);
		m_pTreeView->LoadMibTree(m_Tree);
	}
	CATCH_ALL(e)
	{
		MessageBox(NULL, "Fatal error happened during file reading", "Error", MB_ICONSTOP);
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;
}
