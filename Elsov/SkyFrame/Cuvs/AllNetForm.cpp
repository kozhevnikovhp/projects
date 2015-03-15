// AllNetForm.cpp : implementation file
//

#include "stdafx.h"
#include "AllNetForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAllNetForm

IMPLEMENT_DYNCREATE(CAllNetForm, CAbstractForm)

CAllNetForm::CAllNetForm()
	: CAbstractForm(CAllNetForm::IDD)
{
	//{{AFX_DATA_INIT(CAllNetForm)
	//}}AFX_DATA_INIT
}

CAllNetForm::~CAllNetForm()
{
}

void CAllNetForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAllNetForm)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAllNetForm, CAbstractForm)
	//{{AFX_MSG_MAP(CAllNetForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllNetForm diagnostics

#ifdef _DEBUG
void CAllNetForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CAllNetForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CAllNetForm::SetupControls()
{
	UpdateData(FALSE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAllNetForm message handlers

