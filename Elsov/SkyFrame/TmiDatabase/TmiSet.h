#if !defined(AFX_TMISET_H__32A88307_C793_4F72_9F14_5733C759FA95__INCLUDED_)
#define AFX_TMISET_H__32A88307_C793_4F72_9F14_5733C759FA95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TmiSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTmiSet recordset

class CTmiSet : public CRecordset
{
public:
	CTmiSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CTmiSet)

// Field/Param Data
	//{{AFX_FIELD(CTmiSet, CRecordset)
	CTime	m_Time;
	long	m_StationID;
	BOOL	m_Azs;
	double	m_SN;
	double	m_Level;
	long	m_Offset;
	double	m_T;
	double	m_Power;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmiSet)
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

#endif // !defined(AFX_TMISET_H__32A88307_C793_4F72_9F14_5733C759FA95__INCLUDED_)
