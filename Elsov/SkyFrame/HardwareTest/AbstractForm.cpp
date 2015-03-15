// AbstractForm.cpp : implementation file
//

#include "stdafx.h"
#include "HardwareTestDoc.h"

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
	m_DeviceNumber=0;
	m_pDoc = NULL;
}

CAbstractForm::CAbstractForm(UINT nIDTemplate)
	: CFormView(nIDTemplate)
{
	m_LastTimerTime = 0;
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

void CAbstractForm::Timer()
{
	if (!m_pDoc->IsAbstractFormUpdateTimerEnabled())
		return;
	CTime Time=CTime::GetCurrentTime();
	CTimeSpan DeltaTime=Time-m_LastTimerTime;
	if (DeltaTime.GetTotalSeconds()>DeltaTimerTime())
	{
		m_LastTimerTime=Time;
		CWaitCursor cursor;
		DoTimer();
	}
}

void CAbstractForm::ReportCommandExecution(MC_ErrorCode EC, CDevice *pDevice)
{
	if (EC == MC_OK) return; // do nothing

	CString str;
	switch (EC)
	{
	case MC_COMMAND_NOT_SUPPORTED:
		str.Format("This command is not supported for the device '%s'", pDevice->GetName());
		break;
	case MC_DEVICE_NOT_RESPONDING:
		str.Format("No reply from device '%s'", pDevice->GetName());
		break;
	}
	if (!str.IsEmpty())
		MessageBox(str, "Command execution report", MB_ICONEXCLAMATION);
}


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
