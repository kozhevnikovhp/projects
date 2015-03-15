#if !defined(AFX_STATIONFORM_H__01EC37B7_2279_11D5_9671_008048B59EAA__INCLUDED_)
#define AFX_STATIONFORM_H__01EC37B7_2279_11D5_9671_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationForm form view

#include "AbstractForm.h"

class CStationForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CStationForm)

// Form Data
public:
	//{{AFX_DATA(CStationForm)
	enum { IDD = IDD_STATION_FORM };
	CString	m_Name;
	//}}AFX_DATA

// Attributes
public:

	// Operations
public:
	CStationForm();

// Overrides
	//{{AFX_VIRTUAL(CStationForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL IsAlarm(CStation *pStation);

// Implementation
protected:
	virtual ~CStationForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CStationForm)
	afx_msg void OnApplyButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONFORM_H__01EC37B7_2279_11D5_9671_008048B59EAA__INCLUDED_)
