#if !defined(AFX_TMIQUERYSET_H__6E9CBCBC_63ED_40DA_8277_62A4D49884AA__INCLUDED_)
#define AFX_TMIQUERYSET_H__6E9CBCBC_63ED_40DA_8277_62A4D49884AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiQuerySet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiFromTimeToTimeQuerySet recordset

class CTmiFromTimeToTimeQuerySet : public CRecordset
{
public:
	CTmiFromTimeToTimeQuerySet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CTmiFromTimeToTimeQuerySet)

// Field/Param Data
	//{{AFX_FIELD(CTmiFromTimeToTimeQuerySet, CRecordset)
	CTime	m_Time;
	long	m_StationID;
	BOOL	m_Azs;
	double	m_SN;
	double	m_T;
	double	m_Power;
	long	m_Offset;
	double	m_Level;
	//}}AFX_FIELD
	// Params
	CTime	m_StartTime;
	CTime	m_EndTime;
	long	m_StationParam;
	BOOL	m_bAzsParam;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiFromTimeToTimeQuerySet)
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

#endif // !defined(AFX_TMIQUERYSET_H__6E9CBCBC_63ED_40DA_8277_62A4D49884AA__INCLUDED_)
