#if !defined(AFX_TMIBSTSTATUSFORM_H__CD5D7AC0_3051_46A8_978E_847976E86A80__INCLUDED_)
#define AFX_TMIBSTSTATUSFORM_H__CD5D7AC0_3051_46A8_978E_847976E86A80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiBstStatusForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiBstStatusForm form view

#include "AbstractForm.h"

class CTmiBstStatusForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CTmiBstStatusForm)

// Form Data
public:
	//{{AFX_DATA(CTmiBstStatusForm)
	enum { IDD = IDD_TMI_BST_STATUS_FORM };
	CStatic	m_BstTransmitterPowerCtrl;
	CStatic	m_BstReceiverPowerCtrl;
	int		m_AdcChannel;
	int		m_AdcValue;
	CString	m_AdcHexValue;
	//}}AFX_DATA

// Attributes
public:
	CTmiBstStatusForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CTmiBstStatusForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();

// Implementation
protected:
	virtual ~CTmiBstStatusForm();
	void UpdateAll();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTmiBstStatusForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMIBSTSTATUSFORM_H__CD5D7AC0_3051_46A8_978E_847976E86A80__INCLUDED_)
