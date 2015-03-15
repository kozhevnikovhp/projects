// EventLogForm.cpp : implementation file
//

#include "stdafx.h"
#include "ControlSystemWin.h"
#include "ControlSystemView.h"
#include "ControlSystemDoc.h"
#include "TargetInstance.h"
#include "EventLogForm.h"
#include "Attached.h"
#include "ControlTargetAPI.h"
#include "ControlObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int COLUMN_EVENTTYPE =	0;
const int COLUMN_TIME =			1;
const int COLUMN_SOURCE =		2;
const int COLUMN_COMMENT =		3;

/////////////////////////////////////////////////////////////////////////////
// CEventLogForm

IMPLEMENT_DYNCREATE(CEventLogForm, CAbstractForm)

CEventLogForm::CEventLogForm()
	: CAbstractForm(CEventLogForm::IDD)
{
	//{{AFX_DATA_INIT(EventLogForm)
	//}}AFX_DATA_INIT
	m_pObject = NULL;
//	m_pTarget = NULL;
//	m_pTargetInstance = NULL;
}

CEventLogForm::~CEventLogForm()
{
}

void CEventLogForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventLogForm)
	DDX_Control(pDX, IDC_EVENT_LIST, m_EventList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventLogForm, CAbstractForm)
	//{{AFX_MSG_MAP(CEventLogForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventLogForm diagnostics

#ifdef _DEBUG
void CEventLogForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CEventLogForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEventLogForm message handlers

void CEventLogForm::OnInitialUpdate() 
{
	CAbstractForm::OnInitialUpdate();

	CString ColumnName;
	ColumnName.LoadString(IDS_EVENTLOG_EVENTTYPE);
	m_EventList.InsertColumn(COLUMN_EVENTTYPE, ColumnName);
	ColumnName.LoadString(IDS_EVENTLOG_TIME);
	m_EventList.InsertColumn(COLUMN_TIME, ColumnName);
	ColumnName.LoadString(IDS_EVENTLOG_SOURCE);
	m_EventList.InsertColumn(COLUMN_SOURCE, ColumnName);
	ColumnName.LoadString(IDS_EVENTLOG_COMMENT);
	m_EventList.InsertColumn(COLUMN_COMMENT, ColumnName);
}


//virtual
BOOL CEventLogForm::SetupControls()
{
	m_EventList.DeleteAllItems();

	for (int i = 0; i<10; i++)
	{
		CString str;
		str.Format("Event %d", i);
		m_EventList.InsertItem(LVIF_TEXT|LVIF_STATE, i, str, 0, LVIS_SELECTED, 0, 0);
		m_EventList.SetItemText(i, COLUMN_TIME, "Time");
		m_EventList.SetItemText(i, COLUMN_SOURCE, "Source");
		m_EventList.SetItemText(i, COLUMN_COMMENT, "Comment");
	}

	return TRUE;
}

//virtual
BOOL CEventLogForm::BeforeHiding()
{
	return TRUE;
}

