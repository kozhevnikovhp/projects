// ComPortHelperFrame.cpp : implementation of the CComPortHelperFrame class
//

#include "stdafx.h"
#include "ComPortHelper.h"

#include "ComPortHelperFrame.h"
#include "ComPortHelperView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperFrame

IMPLEMENT_DYNCREATE(CComPortHelperFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CComPortHelperFrame, CFrameWnd)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SENT_BYTES, OnUpdateSentBytes)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_READ_BYTES, OnUpdateReadBytes)
	//{{AFX_MSG_MAP(CComPortHelperFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_SENT_BYTES,
	ID_INDICATOR_READ_BYTES
};


/////////////////////////////////////////////////////////////////////////////
// CComPortHelperFrame construction/destruction

CComPortHelperFrame::CComPortHelperFrame()
{
	// TODO: add member initialization code here
	
}

CComPortHelperFrame::~CComPortHelperFrame()
{
}

int CComPortHelperFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_TimerID = SetTimer(0, 1000, NULL);

	return 0;
}

BOOL CComPortHelperFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperFrame diagnostics

#ifdef _DEBUG
void CComPortHelperFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CComPortHelperFrame::Dump(CDumpContext &dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CComPortHelperFrame message handlers

void CComPortHelperFrame::OnUpdateSentBytes(CCmdUI *pCmdUI)
{
	CComPortHelperView *pView = (CComPortHelperView *)GetActiveView();
	CSerialPort *pPort = pView->GetPort();
	CString str;
	str.Format("%ld bytes sent", pPort->GetSentBytesCount());
	CSize size;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont != NULL) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(str);
	if (hOldFont != NULL) 
		dc.SelectObject(hOldFont);

	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, size.cx);
	pCmdUI->SetText(str);
	pCmdUI->Enable(TRUE);
}

void CComPortHelperFrame::OnUpdateReadBytes(CCmdUI *pCmdUI)
{
	CComPortHelperView *pView = (CComPortHelperView *)GetActiveView();
	CSerialPort *pPort = pView->GetPort();
	CString str;
	str.Format("%ld bytes received", pPort->GetReceivedBytesCount());
	CSize size;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont != NULL) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(str);
	if (hOldFont != NULL) 
		dc.SelectObject(hOldFont);

	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(2, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(2, nID, nStyle, size.cx);
	pCmdUI->SetText(str);
	pCmdUI->Enable(TRUE);
}

void CComPortHelperFrame::OnDestroy() 
{
	KillTimer(m_TimerID);
	CFrameWnd::OnDestroy();
}

