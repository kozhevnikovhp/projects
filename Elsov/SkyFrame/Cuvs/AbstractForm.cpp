// AbstractForm.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm

IMPLEMENT_DYNCREATE(CAbstractForm, CFormView)

CAbstractForm::CAbstractForm()
	: CFormView(CAbstractForm::IDD)
{
	//{{AFX_DATA_INIT(CAbstractForm)
	//}}AFX_DATA_INIT
	m_pAttached=NULL;
}

CAbstractForm::CAbstractForm(UINT nIDTemplate)
	: CFormView(nIDTemplate)
{
}

CAbstractForm::~CAbstractForm()
{
}

void CAbstractForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbstractForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbstractForm, CFormView)
	//{{AFX_MSG_MAP(CAbstractForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm diagnostics

#ifdef _DEBUG
void CAbstractForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CAbstractForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm message handlers
