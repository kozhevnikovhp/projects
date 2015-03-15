// PointToPointView.h : interface of the CPointToPointView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTTOPOINTVIEW_H__E1365D82_CFA3_4C26_B26F_2B40FF4A1B88__INCLUDED_)
#define AFX_POINTTOPOINTVIEW_H__E1365D82_CFA3_4C26_B26F_2B40FF4A1B88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../HardwareTest/StelaM.h"

class CCity
{
public:
	CCity() {}
	~CCity() {}
	BOOL ParseString(const char *pszString);

	char m_szName[256];
	CModIfParams m_ModIfParams;
	CModemDataParams m_ModDataParams;
	CDemIfParams m_DemIfParams;
	CModemDataParams m_DemDataParams;
	IPADDRESS_TYPE m_IpAddress;
};

class CPointToPointView : public CFormView
{
protected: // create from serialization only
	CPointToPointView();
	DECLARE_DYNCREATE(CPointToPointView)

public:
	//{{AFX_DATA(CPointToPointView)
	enum { IDD = IDD_POINTTOPOINT_FORM };
	CEdit	m_SkyFrameEdit;
	CEdit	m_PointToPointEdit;
	CButton	m_SkyFrameButton;
	CButton	m_PointToPointButton;
	CTabCtrl	m_CityTabCtrl;
	CString	m_SkyFrameConfig;
	CString	m_PointToPointConfig;
	CString	m_CurrentConfig;
	//}}AFX_DATA

// Attributes
public:
	CPointToPointDoc* GetDocument();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CPointToPointView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPointToPointView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CCity *m_pThisCity;

protected:
	CCity *GetCity(int index);
	void InitCurrentTab();
	
	//{{AFX_MSG(CPointToPointView)
	afx_msg void OnSelChangeCityTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangingCityTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnPointToPointButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PointToPointView.cpp
inline CPointToPointDoc* CPointToPointView::GetDocument()
   { return (CPointToPointDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTTOPOINTVIEW_H__E1365D82_CFA3_4C26_B26F_2B40FF4A1B88__INCLUDED_)
