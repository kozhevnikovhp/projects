// StationPropertiesForm.h : header file
//

#ifndef __STATION_PROPERTIES_FORM_H_INCLUDED__
#define __STATION_PROPERTIES_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CStationPropertiesForm form view

class CStationPropertiesForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CStationPropertiesForm)

// Form Data
public:
	//{{AFX_DATA(CStationPropertiesForm)
	enum { IDD = IDD_STATION_PROPERTIES_FORM };
	CString	m_StationName;
	//}}AFX_DATA

// Attributes
public:
	CStationPropertiesForm();

	// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CStationPropertiesForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL CanDragItem(CAttachedBlock *pAttached) { return TRUE; }
	virtual BOOL CanDropItem(CAttachedBlock *pAttached) { return TRUE; }

	// Implementation
protected:
	virtual ~CStationPropertiesForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Protected methods
protected:

// Protected members
protected:

	//{{AFX_MSG(CStationPropertiesForm)
	afx_msg void OnApplyButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __STATION_PROPERTIES_FORM_H_INCLUDED__
