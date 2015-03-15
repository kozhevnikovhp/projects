/* ControlForm.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	13 Nov 2007 - initial creation, host names control form
	15 Nov 2007 - subnet names control form
	16 Nov 2007 = control form initialization
	19 Nov 2007 - host/subnet names storing in DB
	28 Nov 2007 - host/subnet addresses control form
	13 Mar 2008 - user name; HostChanged event processing, updating read-only fields separately
	20 Mar 2008 - ping group changing
	21 Mar 2008 - default groups are dynamically allocated as ordinary ones
	08 Jun 2008 - host appearance control form
	22 Jul 2008 - subnet appearance control form
	26 Aug 2008 - host SNMP control form
*/

#ifndef __CONTROL_FORM_H_INCLUDED__
#define __CONTROL_FORM_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CControlForm form view

#include "NetworkDocAPI.h"
#include "ColorButton.h"

// Forward declarations
class CGroup;

class CGroupComboBox : public CComboBox
{
//Constuctors/destructors
public:

// Public methods
public:
	void Init(CGroup *pDefaultGroup);
	void SelectGroup(CGroup *pGroup);
	int AddGroup(CGroup *pGroup);
	CGroup *GetSelectedGroup();
	void UpdateGroupName(CGroup *pGroup);
// Public overridables
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
};

class CControlForm : public CNetworkForm
{
protected:
	CControlForm();           // protected constructor used by dynamic creation
	CControlForm(UINT nIDTemplate);           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CControlForm)

// Form Data
public:
	//{{AFX_DATA(CControlForm)
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:

	//{{AFX_VIRTUAL(CControlForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CControlForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:

// Protected members
protected:

	//{{AFX_MSG(CControlForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CHostControlForm

class CHostControlForm : public CControlForm
{
public:
	CHostControlForm();
	CHostControlForm(UINT nIDTemplate);

protected:
	DECLARE_DYNCREATE(CHostControlForm)

// Form Data
public:
	//{{AFX_DATA(CHostControlForm)
	//}}AFX_DATA

// Public methods
public:
	void Init(CHost *pHost) { m_pHost = pHost; }

// Public overridables
public:
	virtual void NewGroupCreated(CGroup *pGroup) {}
	virtual void GroupChanged(CGroup *pGroup) {}
	virtual CGroup *GetSelectedGroup() { return NULL; }

	//{{AFX_VIRTUAL(CHostControlForm)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHostControlForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected methods
protected:
	void SetGroup();

// Protected members
protected:
	CHost *m_pHost;

	//{{AFX_MSG(CHostControlForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSubnetControlForm

class CSubnetControlForm : public CControlForm
{
public:
	CSubnetControlForm();
	CSubnetControlForm(UINT nIDTemplate);

protected:
	DECLARE_DYNCREATE(CSubnetControlForm)

// Form Data
public:
	//{{AFX_DATA(CSubnetControlForm)
	//}}AFX_DATA

// Public methods
public:
	void Init(CSubnet *pSubnet) { m_pSubnet = pSubnet; }

// Public overridables
public:

	//{{AFX_VIRTUAL(CSubnetControlForm)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSubnetControlForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

// Protected members
protected:
	CSubnet *m_pSubnet;

	//{{AFX_MSG(CSubnetControlForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CHostNamesControlForm form view

class CHostNamesControlForm : public CHostControlForm
{
public:
	CHostNamesControlForm();

protected:
	DECLARE_DYNCREATE(CHostNamesControlForm)

// Form Data
public:
	//{{AFX_DATA(CHostNamesControlForm)
	CString	m_GivenName;
	CString	m_NetbiosName;
	CString	m_NetBiosDomainName;
	CString	m_UserName;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL UpdateReadOnlyFields();
	virtual LOGICAL Leaving();

	//{{AFX_VIRTUAL(CHostNamesControlForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHostNamesControlForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CHostNamesControlForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	afx_msg void OnChangeGivenNameEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CSubnetNamesControlForm : public CSubnetControlForm
{
public:
	CSubnetNamesControlForm();

protected:
	DECLARE_DYNCREATE(CSubnetNamesControlForm)

// Form Data
public:
	//{{AFX_DATA(CSubnetNamesControlForm)
	CString	m_GivenName;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();

	//{{AFX_VIRTUAL(CSubnetNamesControlForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSubnetNamesControlForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CSubnetNamesControlForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	afx_msg void OnChangeGivenNameEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CHostAddressesControlForm

class CHostAddressesControlForm : public CHostControlForm
{
protected:
	DECLARE_DYNCREATE(CHostAddressesControlForm)

public:
	CHostAddressesControlForm();

// Form Data
public:
	//{{AFX_DATA(CHostAddressesControlForm)
	CString	m_IpAddress;
	CString	m_MacAddress;
	CString	m_SubnetMask;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering() { return UpdateReadOnlyFields(); } // RO fields only
	virtual LOGICAL UpdateReadOnlyFields();

// Overrides
	//{{AFX_VIRTUAL(CHostAddressesControlForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHostAddressesControlForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CHostAddressesControlForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSubnetAddressControlForm

class CSubnetAddressControlForm : public CSubnetControlForm
{
protected:
	DECLARE_DYNCREATE(CSubnetAddressControlForm)

public:
	CSubnetAddressControlForm();

// Form Data
public:
	//{{AFX_DATA(CSubnetAddressControlForm)
	CString	m_IpAddress;
	CString	m_SubnetMask;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();

// Overrides
	//{{AFX_VIRTUAL(CSubnetAddressControlForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSubnetAddressControlForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CSubnetAddressControlForm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSubnetAppearanceControlForm

class CSubnetAppearanceControlForm : public CSubnetControlForm
{
protected:
	DECLARE_DYNCREATE(CSubnetAppearanceControlForm)

public:
	CSubnetAppearanceControlForm();

// Form Data
public:
	//{{AFX_DATA(CSubnetAppearanceControlForm)
	CColorButton	m_TextColorButton;
	CColorButton	m_ColorButton;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();

// Overrides
	//{{AFX_VIRTUAL(CSubnetAppearanceControlForm)
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Protected methods
protected:

// Protected members
protected:
	SRGBA m_Color, m_TextColor;

// Implementation
protected:
	virtual ~CSubnetAppearanceControlForm();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CSubnetAppearanceControlForm)
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	afx_msg void OnOrdinaryColorButton();
	afx_msg void OnTextColorButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CHostPingControlForm form view

class CHostPingControlForm : public CHostControlForm
{
protected:
	DECLARE_DYNCREATE(CHostPingControlForm)

public:
	CHostPingControlForm();

// Form Data
public:
	//{{AFX_DATA(CHostPingControlForm)
	CGroupComboBox	m_GroupCombo;
	CString	m_GroupParamsString;
	CString	m_KeepAliveStatusStr;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();
	virtual void NewGroupCreated(CGroup *pGroup);
	virtual void GroupChanged(CGroup *pGroup);
	virtual CGroup *GetSelectedGroup() { return m_GroupCombo.GetSelectedGroup(); }
	virtual void OnUpdateStatus();

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Protected methods
protected:
	void UpdateGroupParamsString();

// Protected overridables
protected:
	virtual ~CHostPingControlForm();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CHostPingControlForm)
	afx_msg void OnSelChangeGroupCombo();
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CHostApperanceControlForm form view

class CHostAppearanceControlForm : public CHostControlForm
{
protected:
	DECLARE_DYNCREATE(CHostAppearanceControlForm)

public:
	CHostAppearanceControlForm();

// Form Data
public:
	//{{AFX_DATA(CHostAppearanceControlForm)
	CGroupComboBox	m_GroupCombo;
	CString	m_GroupParamsString;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();
	virtual void NewGroupCreated(CGroup *pGroup);
	virtual void GroupChanged(CGroup *pGroup);
	virtual CGroup *GetSelectedGroup() { return m_GroupCombo.GetSelectedGroup(); }
	virtual void OnUpdateStatus();

	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Protected methods
protected:
	void UpdateGroupParamsString();

// Protected overridables
protected:
	virtual ~CHostAppearanceControlForm();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CHostAppearanceControlForm)
	afx_msg void OnSelChangeGroupCombo();
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CHostSnmpControlForm form view

class CHostSnmpControlForm : public CHostControlForm
{
protected:
	DECLARE_DYNCREATE(CHostSnmpControlForm)

public:
	CHostSnmpControlForm();

// Form Data
public:
	//{{AFX_DATA(CHostSnmpControlForm)
	CGroupComboBox	m_GroupCombo;
	CString	m_GroupParamsString;
	//}}AFX_DATA

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Entering();
	virtual LOGICAL Leaving();
	virtual void NewGroupCreated(CGroup *pGroup);
	virtual void GroupChanged(CGroup *pGroup);
	virtual CGroup *GetSelectedGroup() { return m_GroupCombo.GetSelectedGroup(); }
	virtual void OnUpdateStatus();

	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Protected methods
protected:
	void UpdateGroupParamsString();

// Protected overridables
protected:
	virtual ~CHostSnmpControlForm();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext &dc) const;
#endif

	//{{AFX_MSG(CHostSnmpControlForm)
	afx_msg void OnSelChangeGroupCombo();
	afx_msg void OnApplyButton();
	afx_msg void OnCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CONTROL_FORM_H_INCLUDED__
