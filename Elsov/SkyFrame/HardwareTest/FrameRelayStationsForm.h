#if !defined(AFX_FRAME_RELAY_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_)
#define AFX_FRAME_RELAY_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// FrameRelayStationsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FrameRelayStationsForm form view

#include "StationsForm.h"

class CFrameRelayStationsForm : public CStationsForm
{
protected:
	DECLARE_DYNCREATE(CFrameRelayStationsForm)

// Form Data
public:
	//{{AFX_DATA(CFrameRelayStationsForm)
	//}}AFX_DATA

// Attributes
public:
	CFrameRelayStationsForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CFrameRelayStationsForm)
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
	virtual ~CFrameRelayStationsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Protected methods
protected:

	// Protected members
protected:

	// Generated message map functions
	//{{AFX_MSG(CFrameRelayStationsForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //AFX_FRAME_RELAY_STATIONSFORM_H__9AE7F521_3D5E_11D4_BBDF_DB1DDF3C251C__INCLUDED_

