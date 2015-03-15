// SettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "litho.h"
#include "SettingsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog dialog


CSettingsDialog::CSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsDialog)
	m_Distance = 0.0;
	m_WaveLength = 0.0;
	m_MaskSize = 0.0;
	m_bPositiveMask = FALSE;
	m_ImageAccuracy = 0.0;
	m_ImageSize = 0.0;
	//}}AFX_DATA_INIT
}


void CSettingsDialog::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDialog)
	DDX_Text(pDX, IDC_DISTANCE_EDIT, m_Distance);
	DDX_Text(pDX, IDC_WAVELENGTH_EDIT, m_WaveLength);
	DDX_Text(pDX, IDC_MASK_SIZE_EDIT, m_MaskSize);
	DDX_Check(pDX, IDC_POSITIVE_MASK_CHECK, m_bPositiveMask);
	DDX_Text(pDX, IDC_IMAGE_ACCURACY_EDIT, m_ImageAccuracy);
	DDX_Text(pDX, IDC_IMAGE_SIZE_EDIT, m_ImageSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CSettingsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog message handlers
