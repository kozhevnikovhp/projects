#ifndef __POWER_METER_FORM_H_INCLUDED__
#define __POWER_METER_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Forward declarations
class CPowerMeter;

// PowerMeterForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPowerMeterForm form view

class CPowerMeterForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CPowerMeterForm)

// Form Data
public:
	//{{AFX_DATA(CPowerMeterForm)
	enum { IDD = IDD_POWER_METER_FORM };
	CString	m_HeaderString;
	CString	m_Power;
	//}}AFX_DATA

// Attributes
public:
	CPowerMeterForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CPowerMeterForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual int DeltaTimerTime() { return 5; }

	// Implementation
protected:
	virtual ~CPowerMeterForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:
	void UpdateAllControls();

	// Protected members
protected:
	CPowerMeter *m_pPowerMeter;

	//{{AFX_MSG(CPowerMeterForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __POWER_METER_FORM_H_INCLUDED__
