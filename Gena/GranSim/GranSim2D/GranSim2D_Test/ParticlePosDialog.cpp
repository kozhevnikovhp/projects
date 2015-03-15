// ParticlePosDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GranSim2D_Test.h"
#include "ParticlePosDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParticlePosDialog dialog


CParticlePosDialog::CParticlePosDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CParticlePosDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParticlePosDialog)
	m_X = 0.0;
	m_Y = 0.0;
	m_Fi = 0.0;
	//}}AFX_DATA_INIT
}


void CParticlePosDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParticlePosDialog)
	DDX_Text(pDX, IDC_X_EDIT, m_X);
	DDX_Text(pDX, IDC_Y_EDIT, m_Y);
	DDX_Text(pDX, IDC_FI_EDIT, m_Fi);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParticlePosDialog, CDialog)
	//{{AFX_MSG_MAP(CParticlePosDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticlePosDialog message handlers
