// TmiStationsView.cpp : implementation of the CTmiStationsView class
//

#include "stdafx.h"
#include "TmiDatabase.h"

#include "TmiStationsSet.h"
#include "TmiDatabaseDoc.h"
#include "TmiStationsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmiStationsView

IMPLEMENT_DYNCREATE(CTmiStationsView, CRecordView)

BEGIN_MESSAGE_MAP(CTmiStationsView, CRecordView)
	//{{AFX_MSG_MAP(CTmiStationsView)
	ON_COMMAND(ID_RECORD_FIRST, OnRecordFirst)
	ON_COMMAND(ID_RECORD_LAST, OnRecordLast)
	ON_COMMAND(ID_RECORD_NEXT, OnRecordNext)
	ON_COMMAND(ID_RECORD_PREV, OnRecordPrev)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CRecordView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRecordView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRecordView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiStationsView construction/destruction

CTmiStationsView::CTmiStationsView()
	: CRecordView(CTmiStationsView::IDD)
{
	//{{AFX_DATA_INIT(CTmiStationsView)
	m_pSet = NULL;
	m_StationName = _T("");
	//}}AFX_DATA_INIT
	// TODO: add construction code here

}

CTmiStationsView::~CTmiStationsView()
{
}

void CTmiStationsView::DoDataExchange(CDataExchange* pDX)
{
	CRecordView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiStationsView)
	DDX_Text(pDX, IDC_STATION_NAME_EDIT, m_StationName);
	//}}AFX_DATA_MAP
}

BOOL CTmiStationsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRecordView::PreCreateWindow(cs);
}

void CTmiStationsView::OnInitialUpdate()
{
	m_pSet = &GetDocument()->m_StationSet;
	CRecordView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	m_StationName = m_pSet->m_Name;
	UpdateData(FALSE);	
}

/////////////////////////////////////////////////////////////////////////////
// CTmiStationsView printing

BOOL CTmiStationsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTmiStationsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTmiStationsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTmiStationsView diagnostics

#ifdef _DEBUG
void CTmiStationsView::AssertValid() const
{
	CRecordView::AssertValid();
}

void CTmiStationsView::Dump(CDumpContext& dc) const
{
	CRecordView::Dump(dc);
}

CTmiDatabaseDoc* CTmiStationsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTmiDatabaseDoc)));
	return (CTmiDatabaseDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTmiStationsView database support
CRecordset* CTmiStationsView::OnGetRecordset()
{
	return m_pSet;
}


/////////////////////////////////////////////////////////////////////////////
// CTmiStationsView message handlers


void CTmiStationsView::OnRecordFirst() 
{
	m_pSet->MoveFirst();
	m_StationName = m_pSet->m_Name;
	UpdateData(FALSE);	
}

void CTmiStationsView::OnRecordLast() 
{
	m_pSet->MoveLast();
	m_StationName = m_pSet->m_Name;
	UpdateData(FALSE);	
}

void CTmiStationsView::OnRecordNext() 
{
	m_pSet->MoveNext();
	m_StationName = m_pSet->m_Name;
	UpdateData(FALSE);	
}

void CTmiStationsView::OnRecordPrev() 
{
	m_pSet->MovePrev();
	m_StationName = m_pSet->m_Name;
	UpdateData(FALSE);	
}

