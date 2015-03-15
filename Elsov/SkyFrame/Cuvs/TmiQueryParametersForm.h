// CTmiQueryParametersForm.h : header file
//

#ifndef __TMI_PARAMETERS_FORM_H_INCLUDED__
#define __TMI_PARAMETERS_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ThreadObject.h"
#include "GetTmiParamSet.h"
#include "TmiToBeAskedSet.h"


/////////////////////////////////////////////////////////////////////////////
// CTmiAttachedBlock

class CTmiAttachedBlock : public CAttachedBlock, public CThreadObject
{
// Construction/destruction
public:
	CTmiAttachedBlock(int ViewIndex, CAbstractForm *pForm);
	virtual ~CTmiAttachedBlock();

// Public overridables
public:
	virtual BOOL Run(void *a_pArgList);

// Protected methods
protected:
	static unsigned (__stdcall ThreadProcedure(void *a_pParams));
	void OneCycle();
	BOOL QueryTmiToBeAsked(long StationID);
	BOOL GetNextTmiParamToBeAsked(long &DeviceID, long &TmiType, long &IpAddress, CTime &LastTmiTime, long &Period);
	BOOL TmiReceived(long DeviceID, long TmiType, CTime CurrentTime);
	BOOL GetDemodulatorStatus();

// Protected members
protected:
	CGetTmiParamSet m_TmiParamSet;
	CTmiToBeAskedSet m_TmiToBeAskedSet;
};


/////////////////////////////////////////////////////////////////////////////
// CTmiQueryParametersForm form view

class CTmiQueryParametersForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CTmiQueryParametersForm)

// Form Data
public:
	//{{AFX_DATA(CTmiQueryParametersForm)
	enum { IDD = IDD_TMI_PARAMETERS_FORM };
	CSpinButtonCtrl	m_PeriodSpin;
	CEdit	m_PeriodEdit;
	CCheckListBox	m_TmiTypesListBox;
	CTabCtrl	m_DevicesTabCtrl;
	long	m_Period;
	//}}AFX_DATA

// Attributes
public:
	CTmiQueryParametersForm();

	// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CTmiQueryParametersForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CTmiQueryParametersForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

// Protected members
protected:
	long m_DeviceID;

	//{{AFX_MSG(CTmiQueryParametersForm)
	afx_msg void OnSelChangeDevicesTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeTypesList();
	afx_msg void OnCheckChangeTypesList();
	afx_msg void OnApplyButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __TMI_PARAMETERS_FORM_H_INCLUDED__
