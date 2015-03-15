#if !defined(AFX_CHANNELSRECORDSET_H__151EC333_6AFE_485B_BBE8_3E7A78220776__INCLUDED_)
#define AFX_CHANNELSRECORDSET_H__151EC333_6AFE_485B_BBE8_3E7A78220776__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChannelsRecordset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChannelsRecordset recordset

class CChannelsRecordset : public CRecordset
{
public:
	CChannelsRecordset(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CChannelsRecordset)

// Field/Param Data
	//{{AFX_FIELD(CChannelsRecordset, CRecordset)
	long	m_N_CHANNEL;
	long	m_TRANSMITTER;
	long	m_RECEIVER;
	BOOL	m_bAutoSN;
	double	m_MaxSN;
	double	m_MinSN;
	//}}AFX_FIELD
	long m_Param1;
	long m_Param2;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChannelsRecordset)
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

#endif // !defined(AFX_CHANNELSRECORDSET_H__151EC333_6AFE_485B_BBE8_3E7A78220776__INCLUDED_)
