#ifndef __ETHERNET_STATUS_FORM_H_INCLUDED__
#define __ETHERNET_STATUS_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// EthernetStatusForm.h : header file
//

// Forward declarations
class CSdmsAPI;

/////////////////////////////////////////////////////////////////////////////
// CEthernetStatusForm form view


class CEthernetStatusForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CEthernetStatusForm)

// Form Data
public:
	//{{AFX_DATA(CEthernetStatusForm)
	enum { IDD = IDD_ETHERNET_STATUS_FORM };
	CString	m_InputBytes;
	CString	m_InputDrops;
	CString	m_InputErrors;
	CString	m_InputPackets;
	CString	m_OutputBytes;
	CString	m_OutputDrops;
	CString	m_OutputErrors;
	CString	m_OutputPackets;
	CString	m_ReceiveBadCRC;
	CString	m_ReceiveCollision;
	CString	m_ReceiveDiscarded;
	CString	m_ReceiveGiant;
	CString	m_ReceiveNonOctet;
	CString	m_ReceiveOverrun;
	CString	m_ReceiveShort;
	CString	m_SendDeffered;
	CString	m_SendHeartBeat;
	CString	m_SendLateCollisions;
	CString	m_SendLostCarrier;
	CString	m_SendRetransmitLimits;
	CString	m_SendRetries;
	CString	m_SendUnderrun;
	//}}AFX_DATA

// Attributes
public:
	CEthernetStatusForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CEthernetStatusForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();
	virtual BOOL DoTimer();
	virtual int DeltaTimerTime() { return 5; }

	// Implementation
protected:
	virtual ~CEthernetStatusForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	// Protected methods
protected:
	void UpdateStatus();

	// Protected members
protected:
	CSdmsAPI *m_pSDMS;

	// Generated message map functions
	//{{AFX_MSG(CEthernetStatusForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __ETHERNET_STATUS_FORM_H_INCLUDED__
