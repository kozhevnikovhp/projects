// Bars.cpp: implementation of the CGoodDialogBar and other classes.
// 
////////////////////////////////////////////////////////////////////// 

#include "stdafx.h"
#include "Bars.h" 

#ifdef _DEBUG 
#undef THIS_FILE 
static char THIS_FILE[]=__FILE__; 
#define new DEBUG_NEW 
#endif 

////////////////////////////////////////////////////////////////////// 
// Construction/Destruction 
////////////////////////////////////////////////////////////////////// 

IMPLEMENT_DYNAMIC(CGoodDialogBar, CDialogBar) 

BEGIN_MESSAGE_MAP(CGoodDialogBar, CDialogBar) 
	//{{AFX_MSG_MAP(CGoodDialogBar) 
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, NeedToolTipsHandler)
END_MESSAGE_MAP() 

CGoodDialogBar::CGoodDialogBar() 
{ 
} 

CGoodDialogBar::~CGoodDialogBar() 
{ 
}

BOOL CGoodDialogBar::OnInitDialogBar() 
{ 
	// Support for the MFC DDX model 
	// If you do not want this do not 
	// call the base class from derived 
	// classes 
	UpdateData(FALSE); 
	return TRUE; 
} 

void CGoodDialogBar::DoDataExchange(CDataExchange* pDX) 
{ 
	//Derived Classes Overide this function 
	ASSERT(pDX); 

	CDialogBar::DoDataExchange(pDX); 

	// In derived class call the DDX_??? 
	// functions to set/retrieve values 
	// of your controls 
	// See example derived class for how 
	// to do this. 
} 

BOOL CGoodDialogBar::NeedToolTipsHandler(UINT id, NMHDR *pTTTStruct, LRESULT *pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	UINT nID =pTTTStruct->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		nID=::GetDlgCtrlID((HWND)nID);
		pTTT->hinst = AfxGetResourceHandle();
		pTTT->lpszText = GetTooltipText(nID);
		return TRUE;
	}
	return FALSE;
}

BOOL CGoodDialogBar::OnInitDialog() 
{
	OnInitDialogBar();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
