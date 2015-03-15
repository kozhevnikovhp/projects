// EthernetStationsForm.cpp : implementation file
//

#include "stdafx.h"
#include "EthernetStationsForm.h"
#include "NsgFrad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEthernetStationsForm

IMPLEMENT_DYNCREATE(CEthernetStationsForm, CStationsForm)

CEthernetStationsForm::CEthernetStationsForm()
	: CStationsForm()
{
	//{{AFX_DATA_INIT(CEthernetStationsForm)
	//}}AFX_DATA_INIT
}

CEthernetStationsForm::~CEthernetStationsForm()
{
}

void CEthernetStationsForm::DoDataExchange(CDataExchange* pDX)
{
	CStationsForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationsForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEthernetStationsForm, CStationsForm)
	//{{AFX_MSG_MAP(CEthernetStationsForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEthernetStationsForm diagnostics

#ifdef _DEBUG
void CEthernetStationsForm::AssertValid() const
{
	CStationsForm::AssertValid();
}

void CEthernetStationsForm::Dump(CDumpContext& dc) const
{
	CStationsForm::Dump(dc);
}
#endif //_DEBUG

int CEthernetStationsForm::GetStationNum()
{
	return m_pNsgFrad->GetEthernetStationNum();
}

//virtual 
char *CEthernetStationsForm::GetStationConfigString(int StationNo)
{
	char *pszStationConfigString;
	m_pNsgFrad->GetEthernetStationConfigString(StationNo, pszStationConfigString);
	return pszStationConfigString;
}

//virtual 
void CEthernetStationsForm::SetStationConfigString(int StationNo, char *&pszStationConfigString)
{
	m_pNsgFrad->SetEthernetStationConfigString(StationNo, pszStationConfigString);
}

//virtual 
char *CEthernetStationsForm::GetStationStatusString(int StationNo)
{
	char *pszStationConfigString;
	m_pNsgFrad->GetEthernetStationStatusString(StationNo, pszStationConfigString);
	return pszStationConfigString;
}

