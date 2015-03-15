#if !defined(AFX_ETHERNET_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_)
#define AFX_ETHERNET_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// EthernetStationsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EthernetStationsForm form view

#include "StationsForm.h"

class CEthernetStationsForm : public CStationsForm
{
protected:
	DECLARE_DYNCREATE(CEthernetStationsForm)

// Form Data
public:
	//{{AFX_DATA(CEthernetStationsForm)
	//}}AFX_DATA

// Attributes
public:
	CEthernetStationsForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CEthernetStationsForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual int GetStationNum();
	virtual char *GetStationConfigString(int StationNo);
	virtual void SetStationConfigString(int StationNo, char *&pszStationConfigString);
	virtual char *GetStationStatusString(int StationNo);

// Implementation
protected:
	virtual ~CEthernetStationsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:

	// Protected members
protected:

	// Generated message map functions
	//{{AFX_MSG(CEthernetStationsForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif //AFX_ETHERNET_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_
