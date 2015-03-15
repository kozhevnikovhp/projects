// CuvsDoc.h : interface of the CCuvsDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __CUVS_DOC_H_INCLUDED__
#define __CUVS_DOC_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Forward declarations
class CStationsView;
class CAbstractForm;

#include "StationsRecordSet.h"
#include "StationDevicesSet.h"
#include "ChannelsRecordset.h"
#include "AllDevicesAndChildDevicesQuerySet.h"
#include "TmiAvailableParamsSet.h"
#include "GetTmiParamSet.h"
#include "DeviceTypesSet.h"
#include "DatabaseConstants.h"

const int UNDEFINED_PARENT = 0;

enum FI_FormIndex
{
	ABSTRACT_INDEX,
	ALL_NET_INDEX,
	STATION_PROPERTIES_INDEX,
	STATION_DEVICES_INDEX,
	CHANNEL_GROUPS_INDEX,
	TMI_PARAMETERS_INDEX,
	STOP_FORM_INDEX
};

class CCuvsDoc : public CDocument
{
	friend class CStationsFrame;
protected: // create from serialization only
	CCuvsDoc();
	DECLARE_DYNCREATE(CCuvsDoc)

// Attributes
public:

// Operations
public:
	CDatabase *GetDatabase() { return m_pDatabase; }
	void AddNewForm(FI_FormIndex IndexInViewArray, CAbstractForm *pViewToAdd, CWnd *pParent);
	void RefreshStations();

	BOOL GetStationName(long StationID, CString &Name);
	BOOL SetStationName(long StationID, CString &Name);

	BOOL SetStationParentID(long StationID, long ParentStationID);

	BOOL QueryChildStations(long StationID);
	BOOL GetNextChildStation(long &StationID, CString &StationName);

	// Device types (read from database)
	BOOL QueryDeviceTypes();
	BOOL GetNextDeviceType(long &DeviceType, CString &DeviceTypeDescription);

	BOOL CreateStationDevice(long StationID, long DeviceType);
	BOOL DeleteStationDevice(long DeviceID);
	BOOL QueryStationDevices(long StationID);
	BOOL GetNextStationDevice(long &DeviceID, CString &DeviceName);

	BOOL QueryStationDevicesAndChildDevices(long StationID);
	BOOL GetNextDeviceAndChildDevice(long &DeviceID, CString &DeviceName, long &DeviceType, CString &StationName);

	BOOL GetStationDevice(long DeviceID, CString &Name, long &Type, long &IPAddress);
	BOOL SetStationDevice(long DeviceID, CString &Name, long Type, long IPAddress);
	BOOL GetStationDeviceType(long DeviceID, long &Type);

	BOOL AddNewChannel(long TransmitterID, long ReceiverID);
	BOOL DeleteChannel(long TransmitterID, long ReceiverID);
	BOOL QueryChannelByReceiverID(long &TransmitterID, long ReceiverID);
	BOOL QueryChannelByReceiverID(long ReceiverID, BOOL &bAutoSN, double &MinSN, double &MaxSN);
	BOOL EditChannelByReceiverID(long ReceiverID, BOOL bAutoSN, double MinSN, double MaxSN);

	BOOL QueryTmiAvailableParams(long DeviceID);
	BOOL GetNextTmiAvailableParam(CString &ParamDescription, long &TmiType);
	BOOL QueryTmiParam(long DeviceID, long TmiType, BOOL &bEnabled, long &Period);
	BOOL EditTmiParam(long DeviceID, long TmiType, BOOL bEnabled, long Period);


// Overrides
	//{{AFX_VIRTUAL(CCuvsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive &ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCuvsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Public members
public:
	CAbstractForm *m_pForms[STOP_FORM_INDEX];


// Protected methods
protected:
	BOOL OpenDatabase();
	BOOL CloseDatabase();
	void AddStation(CTreeCtrl *pTreeCtrl, HTREEITEM Item, CStationsRecordSet *pStationsSet);

// Protected members
protected:
	CStationsView *m_pStationsView;
	HTREEITEM m_AllNetRootItem;
	CDatabase *m_pDatabase;
	CStationsRecordSet m_StationsSet;
	CStationDevicesSet m_StationDevicesSet;
	CChannelsRecordset m_ChannelsSet;
	CAllDevicesAndChildDevicesQuerySet m_AllDevicesAndChildDevicesSet;
	CTmiAvailableParamsSet m_TmiAvailableParamsSet;
	CGetTmiParamSet m_TmiParamSet;
	CDeviceTypesSet m_DeviceTypesSet;

// Generated message map functions
protected:
	//{{AFX_MSG(CCuvsDoc)
	afx_msg void OnStationsAddNew();
	afx_msg void OnStationsDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CUVS_DOC_H_INCLUDED__
