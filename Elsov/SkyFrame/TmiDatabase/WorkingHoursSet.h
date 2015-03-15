#if !defined(AFX_WORKINGHOURSSET_H__CCC011BD_9165_4BF2_8A64_0BD891C6063B__INCLUDED_)
#define AFX_WORKINGHOURSSET_H__CCC011BD_9165_4BF2_8A64_0BD891C6063B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkingHoursSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkingHoursSet recordset

class CWorkingHoursSet : public CRecordset
{
public:
	CWorkingHoursSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CWorkingHoursSet)

// Field/Param Data
	//{{AFX_FIELD(CWorkingHoursSet, CRecordset)
	CTime	m_Time;
	BOOL	m_Azs;
	double	m_SN;
	long	m_Offset;
	//}}AFX_FIELD
	// Params
	CTime	m_StartTime;
	CTime	m_EndTime;
	long	m_StationParam;


// Overrides
	//{{AFX_VIRTUAL(CWorkingHoursSet)
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

#endif // !defined(AFX_WORKINGHOURSSET_H__CCC011BD_9165_4BF2_8A64_0BD891C6063B__INCLUDED_)
