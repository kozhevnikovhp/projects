// TelnetStationsForm.cpp : implementation file
//

#include "stdafx.h"
#include "TelnetStationsForm.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTelnetStationsForm

IMPLEMENT_DYNCREATE(CTelnetStationsForm, CStationsForm)

CTelnetStationsForm::CTelnetStationsForm()
	: CStationsForm()
{
	//{{AFX_DATA_INIT(CTelnetStationsForm)
	//}}AFX_DATA_INIT
}

CTelnetStationsForm::~CTelnetStationsForm()
{
}

void CTelnetStationsForm::DoDataExchange(CDataExchange* pDX)
{
	CStationsForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationsForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTelnetStationsForm, CStationsForm)
	//{{AFX_MSG_MAP(CTelnetStationsForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTelnetStationsForm diagnostics

#ifdef _DEBUG
void CTelnetStationsForm::AssertValid() const
{
	CStationsForm::AssertValid();
}

void CTelnetStationsForm::Dump(CDumpContext& dc) const
{
	CStationsForm::Dump(dc);
}
#endif //_DEBUG

int CTelnetStationsForm::GetStationNum()
{
	return m_pNsgFrad->GetTelnetStationNum();
}

//virtual 
char *CTelnetStationsForm::GetStationConfigString(int StationNo)
{
	char *pszStationConfigString;
	m_pNsgFrad->GetTelnetStationConfigString(StationNo, pszStationConfigString);
	return pszStationConfigString;
}

//virtual 
void CTelnetStationsForm::SetStationConfigString(int StationNo, char *&pszStationConfigString)
{
	m_pNsgFrad->SetTelnetStationConfigString(StationNo, pszStationConfigString);
}

//virtual 
char *CTelnetStationsForm::GetStationStatusString(int StationNo)
{
	char *pszStationConfigString;
	m_pNsgFrad->GetTelnetStationStatusString(StationNo, pszStationConfigString);
	return pszStationConfigString;
}

//virtual
void CTelnetStationsForm::WarmStart(int StationNo)
{
	m_pNsgFrad->WarmStartTelnetStation(StationNo);
}
