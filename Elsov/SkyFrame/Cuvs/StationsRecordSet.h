#if !defined(AFX_STATIONSRECORDSET_H__4E19AB7C_FCFA_4503_AE66_5B98FD831C6D__INCLUDED_)
#define AFX_STATIONSRECORDSET_H__4E19AB7C_FCFA_4503_AE66_5B98FD831C6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationsRecordSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationsRecordSet recordset

class CStationsRecordSet : public CRecordset
{
public:
	CStationsRecordSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CStationsRecordSet)

// Field/Param Data
	//{{AFX_FIELD(CStationsRecordSet, CRecordset)
	long	m_StationID;
	long	m_ParentStationID;
	CString	m_Name;
	//}}AFX_FIELD
	long m_Param1;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationsRecordSet)
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

#endif // !defined(AFX_STATIONSRECORDSET_H__4E19AB7C_FCFA_4503_AE66_5B98FD831C6D__INCLUDED_)
