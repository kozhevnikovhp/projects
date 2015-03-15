#if !defined(AFX_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_)
#define AFX_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationsForm form view

class CNsgFrad;

class CStationsForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CStationsForm)

// Form Data
public:
	//{{AFX_DATA(CStationsForm)
	enum { IDD = IDD_STATIONS_FORM };
	CComboBox	m_StationNumberCombo;
	CString	m_StationConfigString;
	CString	m_StatusString;
	//}}AFX_DATA

// Attributes
public:
	CStationsForm();

// Operations
public:
	void RefreshStatus();

// Overrides
	//{{AFX_VIRTUAL(CStationsForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual int DeltaTimerTime() { return 5; }
	virtual int GetStationNum() { return 0; }
	virtual char *GetStationConfigString(int StationNo) { return ""; }
	virtual void SetStationConfigString(int StationNo, char *&pszStationConfigString) {}
	virtual char *GetStationStatusString(int StationNo) { return ""; }
	virtual BOOL CanWarmStart() { return FALSE;}
	virtual void WarmStart(int StationNo) {}

// Implementation
protected:
	virtual ~CStationsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:

	// Protected members
protected:
	CNsgFrad *m_pNsgFrad;

	// Generated message map functions
	//{{AFX_MSG(CStationsForm)
	afx_msg void OnSelChangeStationNumberCombo();
	afx_msg void OnSetStationConfigButton();
	afx_msg void OnWarmButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_)
