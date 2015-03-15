// CarrierCaptureAutoTestSheet.h : header file
//
// This class defines custom modal property sheet 
// CCarrierCaptureAutoTestSheet.
 
#ifndef __CARRIERCAPTUREAUTOTESTSHEET_H__
#define __CARRIERCAPTUREAUTOTESTSHEET_H__

#include "AutoTestSheet.h"
class CProgressDlg;

/////////////////////////////////////////////////////////////////////////////
// CCarrierCaptureAutoTestSheet

class CCarrierCaptureAutoTestSheet : public CAutoTestSheet
{
	DECLARE_DYNAMIC(CCarrierCaptureAutoTestSheet)

// Construction
public:
	CCarrierCaptureAutoTestSheet(CDevice **pDevices, int nDevices, CWnd *pWndParent = NULL);

// Attributes
public:
	CDevicesPage m_DevicesPage;
	CCarrierPage m_CarrierPage;
	CDataPage m_DataPage;
	CLevelsPage m_LevelPage;
	COtherPage m_OtherPage;

// Operations
public:
	void RunAutoTest();
	void WriteHeader(CStdioFile *pFile);
	void Measure(unsigned int DataRate, double Level, CStdioFile *pFile, CProgressDlg *pDlg);

// Overrides
	//{{AFX_VIRTUAL(CCarrierCaptureAutoTestSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCarrierCaptureAutoTestSheet();

protected:
	//{{AFX_MSG(CCarrierCaptureAutoTestSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __CARRIERCAPTUREAUTOTESTSHEET_H__
