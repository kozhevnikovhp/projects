#if !defined(AFX_TELNET_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_)
#define AFX_TELNET_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TelnetStationsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TelnetStationsForm form view

#include "StationsForm.h"

class CTelnetStationsForm : public CStationsForm
{
protected:
	DECLARE_DYNCREATE(CTelnetStationsForm)

// Form Data
public:
	//{{AFX_DATA(CTelnetStationsForm)
	//}}AFX_DATA

// Attributes
public:
	CTelnetStationsForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CTelnetStationsForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual int GetStationNum();
	virtual char *GetStationConfigString(int StationNo);
	virtual void SetStationConfigString(int StationNo, char *&pszStationConfigString);
	virtual char *GetStationStatusString(int StationNo);
	virtual BOOL CanWarmStart() { return TRUE;}
	virtual void WarmStart(int StationNo);

// Implementation
protected:
	virtual ~CTelnetStationsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:

	// Protected members
protected:

	// Generated message map functions
	//{{AFX_MSG(CTelnetStationsForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif //AFX_TELNET_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_
