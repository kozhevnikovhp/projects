//  CarrierCaptureProgressDlg.cpp : implementation file
// CG: This file was added by the Progress Dialog component

#include "stdafx.h"
#include "resource.h"
#include "CarrierCaptureProgressDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCarrierCaptureProgressDlg dialog

/*CCarrierCaptureProgressDlg::CCarrierCaptureProgressDlg(UINT nCaptionID)
{
	m_nCaptionID = CG_IDS_PROGRESS_CAPTION;
	if (nCaptionID != 0)
		m_nCaptionID = nCaptionID;

    m_bCancel=FALSE;
    m_nLower=0;
    m_nUpper=60000;
    m_nStep=1;
    //{{AFX_DATA_INIT(CCarrierCaptureProgressDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_bParentDisabled = FALSE;
}

CCarrierCaptureProgressDlg::~CCarrierCaptureProgressDlg()
{
    if(m_hWnd!=NULL)
      DestroyWindow();
}

BOOL CCarrierCaptureProgressDlg::DestroyWindow()
{
    ReEnableParent();
    return CDialog::DestroyWindow();
}

void CCarrierCaptureProgressDlg::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}

BOOL CCarrierCaptureProgressDlg::Create(CWnd *pParent)
{
    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.

    if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
    {
      m_pParentWnd->EnableWindow(FALSE);
      m_bParentDisabled = TRUE;
    }

    if(!CDialog::Create(CCarrierCaptureProgressDlg::IDD,pParent))
    {
      ReEnableParent();
      return FALSE;
    }

    return TRUE;
}

void CCarrierCaptureProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCarrierCaptureProgressDlg)
    DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS, m_Progress);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCarrierCaptureProgressDlg, CDialog)
    //{{AFX_MSG_MAP(CCarrierCaptureProgressDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCarrierCaptureProgressDlg::OnCancel()
{
    m_bCancel=TRUE;
}

void CCarrierCaptureProgressDlg::SetRange(int nLower,int nUpper)
{
    m_nLower = nLower;
    m_nUpper = nUpper;
    m_Progress.SetRange(nLower,nUpper);
}
  
int CCarrierCaptureProgressDlg::SetPos(int nPos)
{
    PumpMessages();
    int iResult = m_Progress.SetPos(nPos);
    UpdatePercent(nPos);
    return iResult;
}

int CCarrierCaptureProgressDlg::SetStep(int nStep)
{
    m_nStep = nStep; // Store for later use in calculating percentage
    return m_Progress.SetStep(nStep);
}

int CCarrierCaptureProgressDlg::OffsetPos(int nPos)
{
    PumpMessages();
    int iResult = m_Progress.OffsetPos(nPos);
    UpdatePercent(iResult+nPos);
    return iResult;
}

int CCarrierCaptureProgressDlg::StepIt()
{
    PumpMessages();
    int iResult = m_Progress.StepIt();
    UpdatePercent(iResult+m_nStep);
    return iResult;
}

void CCarrierCaptureProgressDlg::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}

BOOL CCarrierCaptureProgressDlg::CheckCancelButton()
{
    // Process all pending messages
    PumpMessages();

    // Reset m_bCancel to FALSE so that
    // CheckCancelButton returns FALSE until the user
    // clicks Cancel again. This will allow you to call
    // CheckCancelButton and still continue the operation.
    // If m_bCancel stayed TRUE, then the next call to
    // CheckCancelButton would always return TRUE

    BOOL bResult = m_bCancel;

    return bResult;
}

void CCarrierCaptureProgressDlg::UpdatePercent(int nNewPos)
{
    CWnd *pWndPercent = GetDlgItem(CG_IDC_PROGDLG_PERCENT);

    CString strBuf;
    strBuf.Format("Elapsed %d msec", nNewPos);

	pWndPercent->SetWindowText(strBuf);
}
    
/////////////////////////////////////////////////////////////////////////////
// CCarrierCaptureProgressDlg message handlers

BOOL CCarrierCaptureProgressDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    m_Progress.SetRange(m_nLower,m_nUpper);
    m_Progress.SetStep(m_nStep);
    m_Progress.SetPos(m_nLower);

	CString strCaption;
	VERIFY(strCaption.LoadString(m_nCaptionID));
    SetWindowText(strCaption);

    return TRUE;  
}
*/