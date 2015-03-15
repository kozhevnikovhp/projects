// ArmHubDoc.h : interface of the CArmHubDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARMHUBDOC_H__850AA255_2201_11D5_9670_008048B59EAA__INCLUDED_)
#define AFX_ARMHUBDOC_H__850AA255_2201_11D5_9670_008048B59EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAbstractForm;
class CStationsView;
class CTmiSocket;
class CPingSocket;

enum FI_FormIndex
{
	ABSTRACT_INDEX,
	STATION_INDEX,
	IP_ADDRESS_INDEX,
	QUERY_PARAMETERS_INDEX,
	TMI_IP_STATISTICS_INDEX,
	TMI_UDP_STATISTICS_INDEX,
	TMI_NET_DEVICE_STATISTICS_INDEX,
	TMI_MODEM_STATUS_INDEX,
	TMI_BST_STATUS_INDEX,
	TELNET_INDEX,
	STOP_FORM_INDEX
};

#include "Station.h"
#include "Scheduler.h"

class CArmHubDoc : public CDocument
{
protected: // create from serialization only
	CArmHubDoc();
	DECLARE_DYNCREATE(CArmHubDoc)

// Attributes
public:
	CAbstractForm *m_pForms[STOP_FORM_INDEX+1];
	CStationsView *m_pTreeView;
	CPingSocket *m_pPingSocket;
	CListOfObjects StationList;
	CScheduler m_Scheduler;

// Operations
public:
	void LockStations() { EnterCritical(); }
	void UnlockStations() { LeaveCritical(); }
	void AddStation(CStation *pStationToAdd);
	void DeleteStation(CStation *pStationToDelete);
	CStation *GetFirstStation();
	CStation *GetNextStation();
	void AddNewForm(FI_FormIndex IndexInViewArray, CView *pViewToAdd, CWnd *pParent);
	void Save();
	void DispatchPingEchoReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned long SourceIP);
	void DispatchPingDestUnreachable(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader);

// Overrides
	//{{AFX_VIRTUAL(CArmHubDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArmHubDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void EnterCritical();
	void LeaveCritical();

	// Generated message map functions
protected:
	//{{AFX_MSG(CArmHubDoc)
	afx_msg void OnStationsAdd();
	afx_msg void OnStationsDelete();
	afx_msg void OnStationsImport();
	afx_msg void OnStationsExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	POSITION m_StationIteratorPos;
	CRITICAL_SECTION m_Critical;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARMHUBDOC_H__850AA255_2201_11D5_9670_008048B59EAA__INCLUDED_)
