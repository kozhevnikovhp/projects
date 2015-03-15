// AbstractForm.cpp : implementation file
//

#include "stdafx.h"
#include "AbstractForm.h"
#include "ControlSystemDoc.h"

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
	m_pAttached = NULL;
	m_pDoc = NULL;
}

CAbstractForm::CAbstractForm(UINT nIDTemplate)
	: CFormView(nIDTemplate)
{
}

CAbstractForm::~CAbstractForm()
{
}

void CAbstractForm::DoDataExchange(CDataExchange *pDX)
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

void CAbstractForm::Timer()
{
	if (!m_pDoc->IsAbstractFormUpdateTimerEnabled())
		return;
	CTime Time=CTime::GetCurrentTime();
	CTimeSpan DeltaTime=Time-m_LastTimerTime;
	if (DeltaTime.GetTotalSeconds() > DeltaTimerTime())
	{
		m_LastTimerTime=Time;
		CWaitCursor cursor;
		DoTimer();
	}
}

CControlSystemView *CAbstractForm::GetTreeView()
{
	return m_pDoc->m_pTreeView;
}

CControlSystemDoc *CAbstractForm::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CControlSystemDoc)));
	return (CControlSystemDoc *)m_pDocument;
}

#ifdef _DEBUG
void CAbstractForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CAbstractForm::Dump(CDumpContext &dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAbstractForm message handlers
