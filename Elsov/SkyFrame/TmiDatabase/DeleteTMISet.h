#if !defined(AFX_DELETETMISET_H__90B393C8_A661_421F_A2C5_32ADC71DE0AA__INCLUDED_)
#define AFX_DELETETMISET_H__90B393C8_A661_421F_A2C5_32ADC71DE0AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeleteTMISet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeleteTMISet recordset

class CDeleteTMISet : public CRecordset
{
public:
	CDeleteTMISet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CDeleteTMISet)

// Field/Param Data
	//{{AFX_FIELD(CDeleteTMISet, CRecordset)
	CTime	m_Time;
	long	m_StationID;
	double	m_SN;
	BOOL	m_bAzs;
	//}}AFX_FIELD
	// Params
	CTime	m_StartTime;
	CTime	m_EndTime;
	long	m_StationParam;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteTMISet)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETETMISET_H__90B393C8_A661_421F_A2C5_32ADC71DE0AA__INCLUDED_)
