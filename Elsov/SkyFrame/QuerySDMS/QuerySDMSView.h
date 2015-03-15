// QuerySDMSView.h
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __FORMS_H_INCLUDED__
#define __FORMS_H_INCLUDED__

#include "SNMP_stuff.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CControlForm : public CFormView
{
	DECLARE_DYNCREATE(CControlForm)
public:
	CControlForm();
	CControlForm(UINT nIDTemplate);

	//{{AFX_DATA(CControlForm)
	enum { IDD = IDD_PARAMS_FORM };
	//}}AFX_DATA

	static SetIpAddress(IPADDRESS_TYPE uIpAddress) { g_IpAddress = uIpAddress; }
	void OnIdle();
	virtual void Entering() {} 
	virtual void DoTimer() {}
	virtual void UpdateControls(LPCTSTR pszReplyTime) {}
	int GetDeltaTime() { return g_DeltaTime; }

protected:
	LOGICAL IsAdministrator();
	LOGICAL SendGetRequest(unsigned int *puiOID, int nOidLen);
	LOGICAL SendSetUINTRequest(unsigned int *puiOID, int nOidLen, unsigned int uiValue);
	LOGICAL SendSetIntRequest(unsigned int *puiOID, int nOidLen, int iValue);

// Overrides
	//{{AFX_VIRTUAL(CControlForm)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CControlForm)
	//}}AFX_MSG

protected:
	static CSnmpSocket g_Socket;
	static IPADDRESS_TYPE g_IpAddress;
	static UINT g_DeltaTime;
	static int g_ReqID;
	static CString g_ReplyTime;

	// ModIfTable
	static unsigned int g_ModIfFrequency;
	static int g_ModIfOffset;
	static double g_ModIfOutputLevel;
	static CString g_ModIfOutputEnabled;
	static CString g_ModIfModulation;
	static CString g_ModIfSpectrum;

	// ModDataTable
	static unsigned int g_ModDataBitRate;
	static CString g_ModDataCodeRate;
	static CString g_ModDataDiff;
	static CString g_ModDataScrambler;
	static CString g_ModDataClockSource;

	// ModBucTable
	static double g_ModBucVoltage;
	static double g_ModBucCurrent;
	static CString g_ModBucPower;
	static CString g_ModBuc10MHz;

	// DemodIfTable
	static unsigned int g_DemodIfFrequency;
	static unsigned int g_DemodIfSweepRange;
	static CString g_DemodIfSweepMode;
	static CString g_DemodIfModulation;
	static CString g_DemodIfSpectrum;

	// DemodDataTable
	static unsigned int g_DemodDataBitRate;
	static CString g_DemodDataCodeRate;
	static CString g_DemodDataDiff;
	static CString g_DemodDataDescrambler;
	static CString g_DemodDataClockSource;

	// DemodLnbTable
	static double g_DemodLnbVoltage;
	static double g_DemodLnbCurrent;
	static CString g_DemodLnbPower;
	static CString g_DemodLnbMHz;

	// DemodStatus
	static int g_DemodStatusOffset;
	static double g_DemodStatusLevel;
	static double g_DemodStatusEbNo;
	static int g_DemodStatusBuffer;
	static double g_DemodStatusBER;
	static double g_DemodStatusSER;
	static CString g_DemodStatusLock;

	DECLARE_MESSAGE_MAP()
};

class CParamsForm : public CControlForm
{
public:
	CParamsForm();
	DECLARE_DYNCREATE(CParamsForm)

public:
	//{{AFX_DATA(CParamsForm)
	enum { IDD = IDD_PARAMS_FORM };
	UINT	m_DeltaTime;
	//}}AFX_DATA

// Operations
public:
	virtual void Entering(); 
	virtual void DoTimer();

// Overrides
	//{{AFX_VIRTUAL(CParamsForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CParamsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CParamsForm)
	afx_msg void OnApplyButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CModIfTableForm : public CControlForm
{
public:
	CModIfTableForm();
	DECLARE_DYNCREATE(CModIfTableForm)

public:
	//{{AFX_DATA(CModIfTableForm)
	enum { IDD = IDD_MOD_IF_TABLE_FORM };
	UINT	m_Frequency;
	int		m_Offset;
	double	m_Level;
	CString	m_Modulation;
	CString	m_OnOff;
	CString	m_Spectrum;
	CString	m_ReplyTime;
	//}}AFX_DATA

// Operations
public:
	virtual void Entering();
	virtual void DoTimer();
	virtual void UpdateControls(LPCTSTR pszReplyTime);

// Overrides
	//{{AFX_VIRTUAL(CModIfTableForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModIfTableForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CModIfTableForm)
	afx_msg void OnModFrequencyButton();
	afx_msg void OnModOffsetButton();
	afx_msg void OnModModulationButton();
	afx_msg void OnModLevelButton();
	afx_msg void OnModOnoffButton();
	afx_msg void OnModSpectrumButton();
	afx_msg void OnQueryNowButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CModDataTableForm : public CControlForm
{
public:
	CModDataTableForm();
	DECLARE_DYNCREATE(CModDataTableForm)

public:
	//{{AFX_DATA(CModDataTableForm)
	enum { IDD = IDD_MOD_DATA_TABLE_FORM };
	UINT	m_BitRate;
	CString	m_ClockSrc;
	CString	m_CodeRate;
	CString	m_Diff;
	CString	m_Scrambler;
	CString	m_ReplyTime;
	//}}AFX_DATA

// Operations
public:
	virtual void Entering();
	virtual void DoTimer();
	virtual void UpdateControls(LPCTSTR pszReplyTime);

// Overrides
	//{{AFX_VIRTUAL(CModDataTableForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModDataTableForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CModDataTableForm)
	afx_msg void OnModDataBitrateButton();
	afx_msg void OnModDataDiffButton();
	afx_msg void OnModDataCoderateButton();
	afx_msg void OnModDataScramblerButton();
	afx_msg void OnQueryNowButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CModBucTableForm : public CControlForm
{
public:
	CModBucTableForm();
	DECLARE_DYNCREATE(CModBucTableForm)

public:
	//{{AFX_DATA(CModBucTableForm)
	enum { IDD = IDD_MOD_BUC_TABLE_FORM };
	CString	m_ReplyTime;
	double	m_Voltage;
	double	m_Current;
	CString	m_10MHz;
	CString	m_Power;
	//}}AFX_DATA

// Operations
public:
	virtual void Entering();
	virtual void DoTimer();
	virtual void UpdateControls(LPCTSTR pszReplyTime);

// Overrides
	//{{AFX_VIRTUAL(CModBucTableForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModBucTableForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CModBucTableForm)
	afx_msg void OnQueryNowButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CDemodIfTableForm : public CControlForm
{
public:
	CDemodIfTableForm();
	DECLARE_DYNCREATE(CDemodIfTableForm)

public:
	//{{AFX_DATA(CDemodIfTableForm)
	enum { IDD = IDD_DEMOD_IF_TABLE_FORM };
	UINT	m_Frequency;
	CString	m_Modulation;
	CString	m_Spectrum;
	UINT	m_SweepRange;
	CString	m_SweepMode;
	CString	m_ReplyTime;
	//}}AFX_DATA

// Operations
public:
	virtual void Entering();
	virtual void DoTimer();
	virtual void UpdateControls(LPCTSTR pszReplyTime);

// Overrides
	//{{AFX_VIRTUAL(CDemodIfTableForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDemodIfTableForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CDemodIfTableForm)
	afx_msg void OnDemodFrequencyButton();
	afx_msg void OnDemodSweepRangeButton();
	afx_msg void OnDemodModulationButton();
	afx_msg void OnDemodSpectrumButton();
	afx_msg void OnQueryNowButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CDemodDataTableForm : public CControlForm
{
public:
	CDemodDataTableForm();
	DECLARE_DYNCREATE(CDemodDataTableForm)

public:
	//{{AFX_DATA(CDemodDataTableForm)
	enum { IDD = IDD_DEMOD_DATA_TABLE_FORM };
	UINT	m_BitRate;
	CString	m_ClockSrc;
	CString	m_CodeRate;
	CString	m_Descrambler;
	CString	m_Diff;
	CString	m_ReplyTime;
	//}}AFX_DATA

// Operations
public:
	virtual void Entering();
	virtual void DoTimer();
	virtual void UpdateControls(LPCTSTR pszReplyTime);

// Overrides
	//{{AFX_VIRTUAL(CDemodDataTableForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDemodDataTableForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CDemodDataTableForm)
	afx_msg void OnDemodDataBitrateButton();
	afx_msg void OnDemodDataDiffButton();
	afx_msg void OnDemodDataCoderateButton();
	afx_msg void OnDemodDataDescramblerButton();
	afx_msg void OnQueryNowButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CDemodLnbTableForm : public CControlForm
{
public:
	CDemodLnbTableForm();
	DECLARE_DYNCREATE(CDemodLnbTableForm)

public:
	//{{AFX_DATA(CDemodLnbTableForm)
	enum { IDD = IDD_DEMOD_LNB_TABLE_FORM };
	CString	m_ReplyTime;
	//}}AFX_DATA

// Operations
public:
	virtual void Entering();
	virtual void DoTimer();
	virtual void UpdateControls(LPCTSTR pszReplyTime);

// Overrides
	//{{AFX_VIRTUAL(CDemodLnbTableForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDemodLnbTableForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CDemodLnbTableForm)
	afx_msg void OnQueryNowButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



class CDemodStatusForm : public CControlForm
{
public:
	CDemodStatusForm();
	DECLARE_DYNCREATE(CDemodStatusForm)

public:
	//{{AFX_DATA(CDemodStatusForm)
	enum { IDD = IDD_DEMOD_STATUS_FORM };
	double	m_BER;
	double	m_EbNo;
	double	m_Level;
	CString	m_Lock;
	int		m_Offset;
	double	m_SER;
	int		m_Buffer;
	CString	m_ReplyTime;
	//}}AFX_DATA

// Operations
public:
	virtual void Entering();
	virtual void DoTimer();
	virtual void UpdateControls(LPCTSTR pszReplyTime);

// Overrides
	//{{AFX_VIRTUAL(CDemodStatusForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDemodStatusForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CDemodStatusForm)
	afx_msg void OnQueryNowButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__FORMS_H_INCLUDED__
