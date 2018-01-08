// Bars.h: interface for the CGoodDialogBar and other classes.
// 
////////////////////////////////////////////////////////////////////// 

#if !defined(AFX_GOODDIALOGBAR_H__46B4D2B3_C982_11D1_8902_0060979C2EFD__INCLUDED_) 
#define AFX_GOODDIALOGBAR_H__46B4D2B3_C982_11D1_8902_0060979C2EFD__INCLUDED_ 

#if _MSC_VER >= 1000 
#pragma once 
#endif // _MSC_VER >= 1000 

///////////////////////////////////////////////////////////////////////////// 
// CGoodDialogBar window 

class CGoodDialogBar : public CDialogBar 
{ 
	DECLARE_DYNAMIC(CGoodDialogBar) 

// Construction / Destruction 
public:
	CGoodDialogBar();
	virtual ~CGoodDialogBar();

// Operations 
public: 

// Overrides 
	//{{AFX_VIRTUAL(CGoodDialogBar) 
	protected: 
	virtual void DoDataExchange(CDataExchange* pDX); 
	//}}AFX_VIRTUAL 
	virtual LPSTR GetTooltipText(UINT nID) { return MAKEINTRESOURCE(nID); }

// Implementation 
public: 

protected: 
	virtual BOOL OnInitDialogBar(); 

protected: 
	//{{AFX_MSG(CGoodDialogBar) 
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	BOOL NeedToolTipsHandler(UINT id, NMHDR *pTTTStruct, LRESULT *pResult);
	DECLARE_MESSAGE_MAP() 
}; 

///////////////////////////////////////////////////////////////////////////// 

#endif // !defined(AFX_GOODDIALOGBAR_H__46B4D2B3_C982_11D1_8902_0060979C2EFD__INCLUDED_) 
/////////////////////////////////////////////////////////////////////////////
