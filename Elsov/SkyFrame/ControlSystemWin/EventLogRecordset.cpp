// EventLogRecordset.cpp : implementation file
//

#include "stdafx.h"
#include "ControlSystemWin.h"
#include "EventLogRecordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventLogRecordset

IMPLEMENT_DYNAMIC(CEventLogRecordset, CControlSystemRecordset)

CEventLogRecordset::CEventLogRecordset(CDatabase *pdb)
	: CControlSystemRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CEventLogRecordset)
	m_EventID = 0;
	m_EventType = 0;
	m_ObjectID = 0;
	m_CommentID = 0;
	m_EventTime = CTime::GetCurrentTime();
	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}

CString CEventLogRecordset::GetDefaultSQL()
{
	return _T("[EventLog]");
}

void CEventLogRecordset::DoFieldExchange(CFieldExchange *pFX)
{
	//{{AFX_FIELD_MAP(CEventLogRecordset)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[EventID]"), m_EventID);
	RFX_Long(pFX, _T("[EventType]"), m_EventType);
	RFX_Long(pFX, _T("[ObjectID]"), m_ObjectID);
	RFX_Date(pFX, _T("[EventTime]"), m_EventTime);
	RFX_Long(pFX, _T("[CommentID]"), m_CommentID);
	//}}AFX_FIELD_MAP
}

void CEventLogRecordset::OperationStarted()
{
	if (!IsOpen() && !Open())
		return;
	AddNew();
	m_EventTime = CTime::GetCurrentTime();
	m_EventType = EVENT_START;
	m_ObjectID = GetTargetInstanceID();
	m_CommentID = 0;
	Update();
}

void CEventLogRecordset::OperationStopped()
{
	if (!IsOpen() && !Open())
		return;
	AddNew();
	m_EventTime = CTime::GetCurrentTime();
	m_EventType = EVENT_STOP;
	m_ObjectID = GetTargetInstanceID();
	m_CommentID = 0;
	Update();
}

void CEventLogRecordset::ObjectCreated(long ObjectID)
{
	if (!IsOpen() && !Open())
		return;
	AddNew();
	m_EventTime = CTime::GetCurrentTime();
	m_EventType = EVENT_NEW_OBJECT_CREATED;
	m_ObjectID = ObjectID;
	m_CommentID = 0;
	Update();
}

void CEventLogRecordset::ObjectDeleted(long ObjectID)
{
	if (!IsOpen() && !Open())
		return;
	AddNew();
	m_EventTime = CTime::GetCurrentTime();
	m_EventType = EVENT_OBJECT_DELETED;
	m_ObjectID = ObjectID;
	m_CommentID = 0;
	Update();
}

/////////////////////////////////////////////////////////////////////////////
// CEventLogRecordset diagnostics

#ifdef _DEBUG
void CEventLogRecordset::AssertValid() const
{
	CControlSystemRecordset::AssertValid();
}

void CEventLogRecordset::Dump(CDumpContext &dc) const
{
	CControlSystemRecordset::Dump(dc);
}
#endif //_DEBUG
