// ModulatorCarrierAutoTestSheet.h : header file
//
// This class defines custom modal property sheet 
// CModulatorCarrierAutoTestSheet.
 
#ifndef __MODULATOR_CARRIER_AUTOTEST_SHEET_H__
#define __MODULATOR_CARRIER_AUTOTEST_SHEET_H__

#include "AutoTestSheet.h"
class CProgressDlg;

/////////////////////////////////////////////////////////////////////////////
// CModulatorCarrierAutoTestSheet

class CModulatorCarrierAutoTestSheet : public CAutoTestSheet
{
	DECLARE_DYNAMIC(CModulatorCarrierAutoTestSheet)

// Construction
public:
	CModulatorCarrierAutoTestSheet(CDevice **pDevices, int nDevices, CWnd *pWndParent = NULL);

// Attributes
public:
	CDevicesPage m_DevicesPage;
	CSecondSignalPage m_CarrierPage;
	COtherPage m_OtherPage;

// Operations
public:
	void RunAutoTest();
	void WriteHeader(CStdioFile *pFile);
	void Measure(unsigned int freq, CStdioFile *pFile, CProgressDlg *pDlg);

// Overrides
	//{{AFX_VIRTUAL(CModulatorCarrierAutoTestSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModulatorCarrierAutoTestSheet();

protected:
	//{{AFX_MSG(CModulatorCarrierAutoTestSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __MODULATOR_CARRIER_AUTOTEST_SHEET_H__
