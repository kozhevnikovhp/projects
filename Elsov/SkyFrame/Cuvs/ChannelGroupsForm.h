// ChannelGroupsForm.h : header file
//

#ifndef __CHANNEL_GROUPS_FORM_H_INCLUDED__
#define __CHANNEL_GROUPS_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CChannelGroupsForm form view

class CChannelGroupsForm : public CAbstractForm
{
protected:
	DECLARE_DYNCREATE(CChannelGroupsForm)

// Form Data
public:
	//{{AFX_DATA(CChannelGroupsForm)
	enum { IDD = IDD_CHANNEL_GROUPS_FORM };
	CEdit	m_MinSNCtrl;
	CEdit	m_MaxSNCtrl;
	CButton	m_AutoSNCtrl;
	CListBox	m_TransmitterList;
	CCheckListBox	m_ReceiverList;
	BOOL	m_bAutoSN;
	double	m_MaxSN;
	double	m_MinSN;
	//}}AFX_DATA

// Attributes
public:
	CChannelGroupsForm();

	// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CChannelGroupsForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Overridables
public:
	virtual BOOL SetupControls();

	// Implementation
protected:
	virtual ~CChannelGroupsForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

// Protected members
protected:

	//{{AFX_MSG(CChannelGroupsForm)
	afx_msg void OnSelChangeModulatorsList();
	afx_msg void OnCheckChangeDemodulatorsList();
	afx_msg void OnSelChangeDemodulatorsList();
	afx_msg void OnApplySnButton();
	afx_msg void OnAutoSnCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CHANNEL_GROUPS_FORM_H_INCLUDED__
