// HostTrafficDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GLTest.h"
#include "HostTrafficDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHostTrafficDialog dialog


CHostTrafficDialog::CHostTrafficDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CHostTrafficDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHostTrafficDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHostTrafficDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostTrafficDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHostTrafficDialog, CDialog)
	//{{AFX_MSG_MAP(CHostTrafficDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostTrafficDialog message handlers
