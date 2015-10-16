#if !defined(AFX_MODEMCOMBOBOX_H__57B3B49A_BD99_4785_BE8F_985E64B8C0D2__INCLUDED_)
#define AFX_MODEMCOMBOBOX_H__57B3B49A_BD99_4785_BE8F_985E64B8C0D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModemComboBox.h : header file
//

#include "HardwareComboBox.h"

class CModem;

/////////////////////////////////////////////////////////////////////////////
// cModulationTypeComboBox window

class cModulationTypeCombo : public HardwareComboBox
{
// Construction
public:
	cModulationTypeCombo();

// Attributes
public:

// Operations
public:
	void initR(CModem *pModem, int DeviceNumber);
	void initT(CModem *pModem, int DeviceNumber);

// Overrides
	//{{AFX_VIRTUAL(cModulationTypeCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~cModulationTypeCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(cModulationTypeCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// cFecModeCombo window

class cFecModeCombo : public HardwareComboBox
{
// Construction
public:
	cFecModeCombo();

// Attributes
public:

// Operations
public:
	void initR(CModem *pModem, int DeviceNumber);
	void initT(CModem *pModem, int DeviceNumber);

// Overrides
	//{{AFX_VIRTUAL(cFecModeCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~cFecModeCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(cFecModeCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// cFecOptionCombo window

class cFecOptionCombo : public HardwareComboBox
{
// Construction
public:
	cFecOptionCombo();

// Attributes
public:

// Operations
public:
	void initR(CModem *pModem, int DeviceNumber);
	void initT(CModem *pModem, int DeviceNumber);

// Overrides
	//{{AFX_VIRTUAL(cFecOptionCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~cFecOptionCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(cFecOptionCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// cFecOptionCombo window

class cFecCodeRateCombo : public HardwareComboBox
{
// Construction
public:
	cFecCodeRateCombo();

// Attributes
public:

// Operations
public:
	void initR(CModem *pModem, int DeviceNumber);
	void initT(CModem *pModem, int DeviceNumber);

// Overrides
	//{{AFX_VIRTUAL(cFecCodeRateCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~cFecCodeRateCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(cFecCodeRateCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// cDifferentialModeCombo window

class cDifferentialModeCombo : public HardwareComboBox
{
// Construction
public:
	cDifferentialModeCombo();

// Attributes
public:

// Operations
public:
	void initR(CModem *pModem, int DeviceNumber);
	void initT(CModem *pModem, int DeviceNumber);

// Overrides
	//{{AFX_VIRTUAL(cDifferentialModeCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~cDifferentialModeCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(cDifferentialModeCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// cModulationTypeCombox window

class cClockSourceComboBox : public HardwareComboBox
{
// Construction
public:
	cClockSourceComboBox();

// Attributes
public:

// Operations
public:
	void initR(CModem *pModem, int DeviceNumber);
	void initT(CModem *pModem, int DeviceNumber);

// Overrides
	//{{AFX_VIRTUAL(cClockSourceComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~cClockSourceComboBox();

protected:
	//{{AFX_MSG(cClockSourceComboBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// cPowerSupplyCombo window

class cPowerSupplyCombo : public HardwareComboBox
{
// Construction
public:
	cPowerSupplyCombo();

// Attributes
public:

// Operations
public:
	void initR(CModem *pModem, int DeviceNumber);
	void initT(CModem *pModem, int DeviceNumber);

// Overrides
	//{{AFX_VIRTUAL(cPowerSupplyCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~cPowerSupplyCombo();

protected:
	//{{AFX_MSG(cPowerSupplyCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// cScramblerCombo window

class cScramblerCombo : public HardwareComboBox
{
// Construction
public:
	cScramblerCombo();

// Attributes
public:

// Operations
public:
	void initR(CModem *pModem, int DeviceNumber);
	void initT(CModem *pModem, int DeviceNumber);

// Overrides
	//{{AFX_VIRTUAL(cScramblerCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~cScramblerCombo();

protected:
	//{{AFX_MSG(cPowerSupplyCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// c10MHzCombo window

class c10MHzCombo : public HardwareComboBox
{
// Construction
public:
	c10MHzCombo();

// Attributes
public:

// Operations
public:
	void initR(CModem *pModem, int DeviceNumber);
	void initT(CModem *pModem, int DeviceNumber);

// Overrides
	//{{AFX_VIRTUAL(c10MHzCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~c10MHzCombo();

protected:
	//{{AFX_MSG(c10MHzCombo)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODEMCOMBOBOX_H__57B3B49A_BD99_4785_BE8F_985E64B8C0D2__INCLUDED_)
