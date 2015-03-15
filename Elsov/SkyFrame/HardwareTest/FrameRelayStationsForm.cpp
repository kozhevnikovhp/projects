// FrameRelayStationsForm.cpp : implementation file
//

#include "stdafx.h"
#include "FrameRelayStationsForm.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFrameRelayStationsForm

IMPLEMENT_DYNCREATE(CFrameRelayStationsForm, CStationsForm)

CFrameRelayStationsForm::CFrameRelayStationsForm()
	: CStationsForm()
{
	//{{AFX_DATA_INIT(CFrameRelayStationsForm)
	//}}AFX_DATA_INIT
}

CFrameRelayStationsForm::~CFrameRelayStationsForm()
{
}

void CFrameRelayStationsForm::DoDataExchange(CDataExchange* pDX)
{
	CStationsForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationsForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrameRelayStationsForm, CStationsForm)
	//{{AFX_MSG_MAP(CFrameRelayStationsForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameRelayStationsForm diagnostics

#ifdef _DEBUG
void CFrameRelayStationsForm::AssertValid() const
{
	CStationsForm::AssertValid();
}

void CFrameRelayStationsForm::Dump(CDumpContext& dc) const
{
	CStationsForm::Dump(dc);
}
#endif //_DEBUG

int CFrameRelayStationsForm::GetStationNum()
{
	return m_pNsgFrad->GetFrameRelayStationNum();
}

//virtual 
char *CFrameRelayStationsForm::GetStationConfigString(int StationNo)
{
	char *pszStationConfigString;
	m_pNsgFrad->GetFrameRelayStationConfigString(StationNo, pszStationConfigString);
	return pszStationConfigString;
}

//virtual 
void CFrameRelayStationsForm::SetStationConfigString(int StationNo, char *&pszStationConfigString)
{
	m_pNsgFrad->SetFrameRelayStationConfigString(StationNo, pszStationConfigString);
}

//virtual 
char *CFrameRelayStationsForm::GetStationStatusString(int StationNo)
{
	char *pszStationConfigString;
	m_pNsgFrad->GetFrameRelayStationStatusString(StationNo, pszStationConfigString);
	return pszStationConfigString;
}

