#ifndef __FREQUENCY_COUNTER_FORM_H_INCLUDED__
#define __FREQUENCY_COUNTER_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Forward declarations
class CFrequencyCounter;

// FrequencyCounterForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrequencyCounterForm form view

class CFrequencyCounterForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CFrequencyCounterForm)

// Form Data
public:
	//{{AFX_DATA(CFrequencyCounterForm)
	enum { IDD = IDD_FREQUENCY_COUNTER_FORM };
	CString	m_HeaderString;
	CString	m_Frequency;
	//}}AFX_DATA

// Attributes
public:
	CFrequencyCounterForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CFrequencyCounterForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual int DeltaTimerTime() { return 1; }

	// Implementation
protected:
	virtual ~CFrequencyCounterForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:
	void UpdateAllControls();

	// Protected members
protected:
	CFrequencyCounter *m_pFrequencyCounter;

	//{{AFX_MSG(CFrequencyCounterForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __FREQUENCY_COUNTER_FORM_H_INCLUDED__
