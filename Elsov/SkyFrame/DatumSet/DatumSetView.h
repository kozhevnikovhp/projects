// DatumSetView.h : interface of the CDatumSetView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATUMSETVIEW_H__C0D7FFF1_6E35_420A_8030_C3045D989DC2__INCLUDED_)
#define AFX_DATUMSETVIEW_H__C0D7FFF1_6E35_420A_8030_C3045D989DC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../HardwareTest/Datum.h"

class CDatumSetView : public CFormView
{
protected: // create from serialization only
	CDatumSetView();
	DECLARE_DYNCREATE(CDatumSetView)

public:
	//{{AFX_DATA(CDatumSetView)
	enum { IDD = IDD_DATUMSET_FORM };
	CSpinButtonCtrl	m_RemoteAddressSpin;
	CEdit	m_RemoteAddressEdit;
	CComboBox	m_PortCombo;
	CString	m_ModeString;
	CString	m_PortStatus;
	BYTE	m_RemoteAddress;
	CString	m_UnitID;
	CString	m_ModelName;
	UINT	m_SerialNo;
	//}}AFX_DATA

// Attributes
public:
	CDatumSetDoc* GetDocument();
	CDatumPsm4900_LBand m_Modem;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatumSetView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDatumSetView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDatumSetView)
	afx_msg void OnCloseButton();
	afx_msg void OnOpenButton();
	afx_msg void OnConfigureButton();
	afx_msg void OnDestroy();
	afx_msg void OnGetInfoButton();
	afx_msg void OnSetModelNameButton();
	afx_msg void OnSetUnitIdButton();
	afx_msg void OnSetSerialNoButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DatumSetView.cpp
inline CDatumSetDoc* CDatumSetView::GetDocument()
   { return (CDatumSetDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATUMSETVIEW_H__C0D7FFF1_6E35_420A_8030_C3045D989DC2__INCLUDED_)
