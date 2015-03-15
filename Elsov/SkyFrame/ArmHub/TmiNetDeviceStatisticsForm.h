#if !defined(AFX_TMI_NET_DEVICE_STATISTICS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
#define AFX_TMI_NET_DEVICE_STATISTICS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiNetDeviceStatisticsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiNetDeviceStatisticsForm form view

#include "TmiStatisticsForm.h"

class CTmiNetDeviceStatisticsForm : public CTmiStatisticsForm
{
protected:
	DECLARE_DYNCREATE(CTmiNetDeviceStatisticsForm)

// Form Data
public:
	//{{AFX_DATA(CTmiNetDeviceStatisticsForm)
	enum { IDD = IDD_TMI_STATISTICS_FORM };
	//}}AFX_DATA

// Attributes
public:
	CTmiNetDeviceStatisticsForm();

// Operations
public:

	// Overrides
	//{{AFX_VIRTUAL(CTmiNetDeviceStatisticsForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual CString GetStatisticsText(unsigned nTab, BOOL bIncremental);
	virtual BOOL IsTabVisible() { return TRUE; }

	// Implementation
protected:
	virtual ~CTmiNetDeviceStatisticsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CTmiNetDeviceStatisticsForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMI_NET_DEVICE_STATISTICS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
