#if !defined(AFX_TMI_IP_STATISTICS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
#define AFX_TMI_IP_STATISTICS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiIpStatisticsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiIpStatisticsForm form view

#include "TmiStatisticsForm.h"

class CTmiIpStatisticsForm : public CTmiStatisticsForm
{
protected:
	DECLARE_DYNCREATE(CTmiIpStatisticsForm)

// Form Data
public:
	//{{AFX_DATA(CTmiIpStatisticsForm)
	enum { IDD = IDD_TMI_STATISTICS_FORM };
	//}}AFX_DATA

// Attributes
public:
	CTmiIpStatisticsForm();

// Operations
public:

	// Overrides
	//{{AFX_VIRTUAL(CTmiIpStatisticsForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual CString GetStatisticsText(BOOL bIncremental);

	// Implementation
protected:
	virtual ~CTmiIpStatisticsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//{{AFX_MSG(CTmiIpStatisticsForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMI_IP_STATISTICS_FORM_H__01EC37B8_2279_11D5_9671_008048B59EAA__INCLUDED_)
