// TmiOptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "armhub.h"
#include "TmiOptionsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmiOptionsDialog dialog


CTmiOptionsDialog::CTmiOptionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTmiOptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTmiOptionsDialog)
	m_IpPort = 0;
	//}}AFX_DATA_INIT
}


void CTmiOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiOptionsDialog)
	DDX_Text(pDX, IDC_IP_PORT_EDIT, m_IpPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTmiOptionsDialog, CDialog)
	//{{AFX_MSG_MAP(CTmiOptionsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiOptionsDialog message handlers
