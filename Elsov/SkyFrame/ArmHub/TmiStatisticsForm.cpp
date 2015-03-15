// TmiStatisticsForm.cpp : implementation file
//

#include "stdafx.h"
#include "armhub.h"
#include "TmiStatisticsForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Station.h"

/////////////////////////////////////////////////////////////////////////////
// CTmiStatisticsForm

IMPLEMENT_DYNCREATE(CTmiStatisticsForm, CAbstractForm)

CTmiStatisticsForm::CTmiStatisticsForm()
	: CAbstractForm(CTmiStatisticsForm::IDD)
{
	//{{AFX_DATA_INIT(CTmiStatisticsForm)
	m_Statistics = _T("");
	//}}AFX_DATA_INIT
}

CTmiStatisticsForm::CTmiStatisticsForm(UINT nIDTemplate)
	: CAbstractForm(nIDTemplate)
{
}


CTmiStatisticsForm::~CTmiStatisticsForm()
{
}

void CTmiStatisticsForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiStatisticsForm)
	DDX_Control(pDX, IDC_TMI_STATISTICS_EDIT, m_StatisticsEditCtrl);
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Text(pDX, IDC_TMI_STATISTICS_EDIT, m_Statistics);
	//}}AFX_DATA_MAP
}

//virtual
BOOL CTmiStatisticsForm::SetupControls()
{
	UpdateStatisticsText();
	return TRUE;
}

//virtual
BOOL CTmiStatisticsForm::DoTimer()
{
	if (m_pStation->IsTmiChangedSince(m_LastTimerTime))
		UpdateStatisticsText();
	return TRUE;
}

void CTmiStatisticsForm::UpdateUntabbedStatisticsText()
{
	BOOL bIncremental = (GetCheckedRadioButton(IDC_CUMULATIVE_RADIO, IDC_INCREMENTAL_RADIO) == IDC_INCREMENTAL_RADIO);
	m_Statistics = GetStatisticsText(bIncremental);
	UpdateData(FALSE);
}

void CTmiStatisticsForm::UpdateTabbedStatisticsText()
{
	BOOL bIncremental = (GetCheckedRadioButton(IDC_CUMULATIVE_RADIO, IDC_INCREMENTAL_RADIO) == IDC_INCREMENTAL_RADIO);
	int nTab = m_Tab.GetCurSel();
	if (nTab < 0 )
		nTab = 0;
	m_Statistics = GetStatisticsText(nTab, bIncremental);
	UpdateData(FALSE);
}


BEGIN_MESSAGE_MAP(CTmiStatisticsForm, CAbstractForm)
	//{{AFX_MSG_MAP(CTmiStatisticsForm)
	ON_BN_CLICKED(IDC_CUMULATIVE_RADIO, OnCumulativeRadio)
	ON_BN_CLICKED(IDC_INCREMENTAL_RADIO, OnIncrementalRadio)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_BN_CLICKED(IDC_FONT_BUTTON, OnFontButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiStatisticsForm diagnostics

#ifdef _DEBUG
void CTmiStatisticsForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CTmiStatisticsForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTmiStatisticsForm message handlers

void CTmiStatisticsForm::OnInitialUpdate() 
{
	CAbstractForm::OnInitialUpdate();
	CheckRadioButton(IDC_CUMULATIVE_RADIO, IDC_INCREMENTAL_RADIO, IDC_INCREMENTAL_RADIO);
	if (IsTabVisible())
		m_Tab.ShowWindow(SW_SHOW);
	else
		m_Tab.ShowWindow(SW_HIDE);
/*	m_StatisticsFont.CreateFont(14, 0, 0, 0, FW_DONTCARE,
		FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "Courier New");
	m_StatisticsEditCtrl.SetFont(&m_StatisticsFont);*/
}

void CTmiStatisticsForm::OnCumulativeRadio() 
{
	UpdateStatisticsText();
}

void CTmiStatisticsForm::UpdateStatisticsText()
{
	if (IsTabVisible())
		UpdateTabbedStatisticsText();
	else
		UpdateUntabbedStatisticsText();
}

void CTmiStatisticsForm::OnIncrementalRadio() 
{
	UpdateStatisticsText();
}

void CTmiStatisticsForm::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateTabbedStatisticsText();
	*pResult = 0;
}

void CTmiStatisticsForm::OnFontButton() 
{
	CFont *pFont = m_StatisticsEditCtrl.GetFont();
	LOGFONT LogFont;
	pFont->GetLogFont(&LogFont);
	CFontDialog dlg(&LogFont);
	if (dlg.DoModal() != IDOK)
		return;

	m_StatisticsFont.Detach();
	m_StatisticsFont.CreateFontIndirect(&LogFont);
	m_StatisticsEditCtrl.SetFont(&m_StatisticsFont);
}
