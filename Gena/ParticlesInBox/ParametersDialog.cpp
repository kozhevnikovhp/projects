// ParametersDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ParticlesInBox.h"
#include "ParametersDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParametersDialog dialog


CParametersDialog::CParametersDialog(CWnd *pParent /*=NULL*/)
	: CDialog(CParametersDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParametersDialog)
	m_nParticles = 0;
	m_G = 0.0;
	m_LossRatio = 0.0;
	m_RParticle = 0.0;
	m_Xleft = 0.0;
	m_Xright = 0.0;
	m_Ybottom = 0.0;
	m_Ytop = 0.0;
	m_WallXPosition = 0.0;
	m_HolePosition = 0.0;
	m_HoleSize = 0.0;
	m_Vinit = 0.0;
	m_BounceBottom = 0.0;
	m_BounceTop = 0.0;
	m_BounceSide = 0.0;
	m_FileName = _T("");
	m_WallThickness = 0.0;
	//}}AFX_DATA_INIT
	m_bFileNameSpecified = FALSE;
}


void CParametersDialog::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParametersDialog)
	DDX_Text(pDX, IDC_N_PARTICLES_EDIT, m_nParticles);
	DDV_MinMaxInt(pDX, m_nParticles, 1, 65536);
	DDX_Text(pDX, IDC_G_EDIT, m_G);
	DDV_MinMaxDouble(pDX, m_G, 0., 1000.);
	DDX_Text(pDX, IDC_LOSS_EDIT, m_LossRatio);
	DDX_Text(pDX, IDC_R_PARTICLE_EDIT, m_RParticle);
	DDV_MinMaxDouble(pDX, m_RParticle, 0., 1.);
	DDX_Text(pDX, IDC_X_LEFT_EDIT, m_Xleft);
	DDX_Text(pDX, IDC_X_RIGHT_EDIT, m_Xright);
	DDX_Text(pDX, IDC_Y_BOTTOM_EDIT, m_Ybottom);
	DDX_Text(pDX, IDC_Y_TOP_EDIT, m_Ytop);
	DDX_Text(pDX, IDC_WALL_X_POSITION_EDIT, m_WallXPosition);
	DDX_Text(pDX, IDC_HOLE_POSITION_EDIT, m_HolePosition);
	DDX_Text(pDX, IDC_HOLE_SIZE_EDIT, m_HoleSize);
	DDX_Text(pDX, IDC_V_INIT_EDIT, m_Vinit);
	DDX_Text(pDX, IDC_BOUNCE_BOTTOM_EDIT, m_BounceBottom);
	DDX_Text(pDX, IDC_BOUNCE_TOP_EDIT, m_BounceTop);
	DDX_Text(pDX, IDC_BOUNCE_SIDE_EDIT, m_BounceSide);
	DDX_Text(pDX, IDC_FILE_NAME_STATIC, m_FileName);
	DDX_Text(pDX, IDC_WALL_THICKNESS_EDIT, m_WallThickness);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParametersDialog, CDialog)
	//{{AFX_MSG_MAP(CParametersDialog)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	ON_BN_CLICKED(ID_ABOUT_BUTTON, OnAboutButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParametersDialog message handlers

void CParametersDialog::OnBrowseButton() 
{
	UpdateData(TRUE);
	CFileDialog fd(FALSE, _T("txt"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Text files (*txt)|*txt|All files (*.*)|*.*||"));
	fd.m_ofn.lpstrTitle = "Please specify path and file name to write results to";
	if (fd.DoModal() == IDCANCEL)
		return;
	m_bFileNameSpecified = TRUE;
	m_FileName = fd.GetPathName();
	UpdateData(FALSE);
}

void CParametersDialog::OnOK() 
{
	if (!m_bFileNameSpecified)
	{
		MessageBox("Please specify file name for results writting.", "Error", MB_ICONEXCLAMATION);
		return;
	}
	CDialog::OnOK();
}

void CParametersDialog::OnAboutButton() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}
