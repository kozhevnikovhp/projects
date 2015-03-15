#ifndef __SERIAL_STATUS_FORM_H_INCLUDED__
#define __SERIAL_STATUS_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SerialStatusForm.h : header file
//

// Forward declarations
class CSdmsAPI;

/////////////////////////////////////////////////////////////////////////////
// CSerialStatusForm form view


class CSerialStatusForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CSerialStatusForm)

// Form Data
public:
	//{{AFX_DATA(CSerialStatusForm)
	enum { IDD = IDD_SERIAL_STATUS_FORM };
	CString	m_InputPackets;
	CString	m_InputBytes;
	CString	m_InputDrops;
	CString	m_InputErrors;
	CString	m_OutputBytes;
	CString	m_OutputDrops;
	CString	m_OutputErrors;
	CString	m_OutputPackets;
	CString	m_ReceiveCRC;
	CString	m_ReceiveAbortSeq;
	CString	m_ReceiveDpll;
	CString	m_ReceiveFrameViolation;
	CString	m_ReceiveLostCarrier;
	CString	m_ReceiveNonOctet;
	CString	m_ReceiveOverrun;
	CString	m_SendUnderrun;
	CString	m_SendKeepaliveState;
	CString	m_SendLastPoll;
	CString	m_SendLostCTS;
	CString	m_SendRxSeq;
	CString	m_SendTxSeq;
	//}}AFX_DATA

// Attributes
public:
	CSerialStatusForm();

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CSerialStatusForm)
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
	virtual ~CSerialStatusForm();
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
	//{{AFX_MSG(CSerialStatusForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SERIAL_STATUS_FORM_H_INCLUDED__
