#if !defined(AFX_TMI_UDP_STATISTICS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
#define AFX_TMI_UDP_STATISTICS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TmiUdpStatisticsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiUdpStatisticsForm form view

#include "TmiStatisticsForm.h"

class CTmiUdpStatisticsForm : public CTmiStatisticsForm
{
protected:
	DECLARE_DYNCREATE(CTmiUdpStatisticsForm)

// Form Data
public:
	//{{AFX_DATA(CTmiUdpStatisticsForm)
	enum { IDD = IDD_TMI_STATISTICS_FORM };
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	CTmiUdpStatisticsForm();

	// Overrides
	//{{AFX_VIRTUAL(CTmiUdpStatisticsForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual CString GetStatisticsText(BOOL bIncremental);

	// Implementation
protected:
	virtual ~CTmiUdpStatisticsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CTmiUdpStatisticsForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMI_UDP_STATISTICS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
