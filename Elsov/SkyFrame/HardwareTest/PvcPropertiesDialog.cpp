// PvcPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PvcPropertiesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPvcPropertiesDialog dialog


CPvcPropertiesDialog::CPvcPropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPvcPropertiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPvcPropertiesDialog)
	m_Port1 = _T("");
	m_Port2 = _T("");
	m_Channel1 = _T("");
	m_Channel2 = _T("");
	//}}AFX_DATA_INIT
}


void CPvcPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPvcPropertiesDialog)
	DDX_Text(pDX, IDC_PORT1_EDIT, m_Port1);
	DDX_Text(pDX, IDC_PORT2_EDIT, m_Port2);
	DDX_Text(pDX, IDC_CHANNEL1_EDIT, m_Channel1);
	DDX_Text(pDX, IDC_CHANNEL2_EDIT, m_Channel2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPvcPropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CPvcPropertiesDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPvcPropertiesDialog message handlers
