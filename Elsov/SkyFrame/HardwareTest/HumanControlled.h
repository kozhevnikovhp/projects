#ifndef __HUMAN_CONTROLLED_H_INCLUDED__
#define __HUMAN_CONTROLLED_H_INCLUDED__

#include "PowerMeter.h"
#include "FrequencyCounter.h"
#include "Resource.h"

class CHumanPowerMeter : public CPowerMeter
{
// Construction/destruction
public:
	CHumanPowerMeter();
	virtual ~CHumanPowerMeter();

// Common device methods
	virtual CTerminal *GetTerminal() { return NULL; }
	virtual MC_ErrorCode ReadReplyUntilPrompt() { return MC_OK; }
	virtual unsigned char *GetMCReplyData() { return NULL; }
	virtual unsigned int GetMCReplyDataLength() { return 0; }
	char *GetConnectionName() { return ""; }
	virtual BOOL IsHuman() { return TRUE; }	// human-controlled device

// Overridable methods
	virtual char *GetName() { return m_pszName; }

// Power measurement
	virtual MC_ErrorCode MeasurePower(double &Power, int Channel);

// Protected methods
protected:

// Public members
public:
	static char *m_pszName;

// Protected members
protected:
};


/////////////////////////////////////////////////////////////////////////////
// CHumanPowerMeterDialog dialog

class CHumanPowerMeterDialog : public CDialog
{
// Construction
public:
	CHumanPowerMeterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHumanPowerMeterDialog)
	enum { IDD = IDD_HUMAN_POWER_METER_DIALOG };
	double	m_Power;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CHumanPowerMeterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CHumanPowerMeterDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CHumanFrequencyCounter : public CFrequencyCounter
{
// Construction/destruction
public:
	CHumanFrequencyCounter();
	virtual ~CHumanFrequencyCounter();

// Common device methods
	virtual CTerminal *GetTerminal() { return NULL; }
	virtual MC_ErrorCode ReadReplyUntilPrompt() { return MC_OK; }
	virtual unsigned char *GetMCReplyData() { return NULL; }
	virtual unsigned int GetMCReplyDataLength() { return 0; }
	char *GetConnectionName() { return ""; }
	virtual BOOL IsHuman() { return TRUE; }	// human-controlled device

// Overridable methods
	virtual char *GetName() { return m_pszName; }

// Power measurement
	virtual MC_ErrorCode MeasureFrequency(double &Frequency, int Channel);

// Protected methods
protected:

// Public members
public:
	static char *m_pszName;

// Protected members
protected:
};


/////////////////////////////////////////////////////////////////////////////
// CHumanFrequencyCounterDialog dialog

class CHumanFrequencyCounterDialog : public CDialog
{
// Construction
public:
	CHumanFrequencyCounterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHumanFrequencyCounterDialog)
	enum { IDD = IDD_HUMAN_FREQUENCY_COUNTER_DIALOG };
	double	m_Frequency;
	//}}AFX_DATA


// Overrides
	//{{AFX_VIRTUAL(CHumanFrequencyCounterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//{{AFX_MSG(CHumanFrequencyCounterDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif //__HUMAN_CONTROLLED_H_INCLUDED__
