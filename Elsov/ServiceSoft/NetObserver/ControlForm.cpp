/* ControlForm.cpp

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

#include "stdafx.h"
#include "netobserver.h"
#include "ControlForm.h"
#include "Network.h"
#include "Groups.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupComboBox

void CGroupComboBox::Init(CGroup *pDefaultGroup)
{
	ResetContent();
	for (CListOfGroups::iterator p = g_AllGroups.begin(); p != g_AllGroups.end(); p++)
	{
		CGroup *pGroup = *p;
		if (pDefaultGroup->IsEqualType(pGroup))
			AddGroup(pGroup);
	}
}

int CGroupComboBox::AddGroup(CGroup *pGroup)
{
	int index = AddString(pGroup->GetName());
	SetItemDataPtr(index, pGroup);
	return index;
}

CGroup *CGroupComboBox::GetSelectedGroup()
{
	int index = GetCurSel();
	if (index == CB_ERR)
		return NULL;
	return (CGroup *)GetItemDataPtr(index);
}

void CGroupComboBox::UpdateGroupName(CGroup *pGroup)
{
	int iSelected = GetCurSel();
	int iItemToUpdate = CB_ERR;
	for (int i = 0; i < GetCount(); i++)
	{
		if (GetItemDataPtr(i) == pGroup)
		{
			iItemToUpdate = i;
			break;
		}
	}
	if (iItemToUpdate != CB_ERR)
	{
		DeleteString(iItemToUpdate);
		int iNewString = AddGroup(pGroup);
		if (iSelected == iItemToUpdate)
			SetCurSel(iNewString);
	}
}

void CGroupComboBox::SelectGroup(CGroup *pGroup)
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (GetItemDataPtr(i) == pGroup)
		{
			SetCurSel(i);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CControlForm

IMPLEMENT_DYNCREATE(CControlForm, CNetworkForm)

CControlForm::CControlForm() : CNetworkForm(IDD_CONTROL_FORM)
{
	//{{AFX_DATA_INIT(CControlForm)
	//}}AFX_DATA_INIT
}

CControlForm::CControlForm(UINT nIDTemplate)  : CNetworkForm(nIDTemplate)
{
}

CControlForm::~CControlForm()
{
}

void CControlForm::DoDataExchange(CDataExchange *pDX)
{
	CNetworkForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControlForm, CNetworkForm)
	//{{AFX_MSG_MAP(CControlForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlForm diagnostics

#ifdef _DEBUG
void CControlForm::AssertValid() const
{
	CNetworkForm::AssertValid();
}

void CControlForm::Dump(CDumpContext &dc) const
{
	CNetworkForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CControlForm message handlers

/////////////////////////////////////////////////////////////////////////////
// CHostControlForm

IMPLEMENT_DYNCREATE(CHostControlForm, CControlForm)

CHostControlForm::CHostControlForm() : CControlForm()
{
	//{{AFX_DATA_INIT(CHostControlForm)
	//}}AFX_DATA_INIT
	m_pHost = NULL;
}

CHostControlForm::CHostControlForm(UINT nIDTemplate) : CControlForm(nIDTemplate)
{
	//{{AFX_DATA_INIT(CHostControlForm)
	//}}AFX_DATA_INIT
	m_pHost = NULL;
}

CHostControlForm::~CHostControlForm()
{
}


BEGIN_MESSAGE_MAP(CHostControlForm, CControlForm)
	//{{AFX_MSG_MAP(CHostControlForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostControlForm diagnostics

#ifdef _DEBUG
void CHostControlForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CHostControlForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}
#endif //_DEBUG

void CHostControlForm::SetGroup()
{
	CGroup *pGroup = GetSelectedGroup();
	ASSERT(pGroup);
	m_pHost->SetGroup(pGroup);
}

/////////////////////////////////////////////////////////////////////////////
// CHostControlForm message handlers


/////////////////////////////////////////////////////////////////////////////
// CSubnetControlForm

IMPLEMENT_DYNCREATE(CSubnetControlForm, CControlForm)

CSubnetControlForm::CSubnetControlForm() : CControlForm()
{
	//{{AFX_DATA_INIT(CSubnetControlForm)
	//}}AFX_DATA_INIT
	m_pSubnet = NULL;
}

CSubnetControlForm::CSubnetControlForm(UINT nIDTemplate) : CControlForm(nIDTemplate)
{
	//{{AFX_DATA_INIT(CSubnetControlForm)
	//}}AFX_DATA_INIT
	m_pSubnet = NULL;
}

CSubnetControlForm::~CSubnetControlForm()
{
}


BEGIN_MESSAGE_MAP(CSubnetControlForm, CControlForm)
	//{{AFX_MSG_MAP(CSubnetControlForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubnetControlForm diagnostics

#ifdef _DEBUG
void CSubnetControlForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CSubnetControlForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CSubnetControlForm message handlers



/////////////////////////////////////////////////////////////////////////////
// CHostNamesControlForm

IMPLEMENT_DYNCREATE(CHostNamesControlForm, CHostControlForm)

CHostNamesControlForm::CHostNamesControlForm()
	: CHostControlForm(IDD_HOST_NAMES_CONTROL_FORM)
{
	//{{AFX_DATA_INIT(CHostNamesControlForm)
	m_GivenName = _T("");
	m_NetbiosName = _T("");
	m_NetBiosDomainName = _T("");
	m_UserName = _T("");
	//}}AFX_DATA_INIT
}

CHostNamesControlForm::~CHostNamesControlForm()
{
}

void CHostNamesControlForm::DoDataExchange(CDataExchange *pDX)
{
	CHostControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostNamesControlForm)
	DDX_Text(pDX, IDC_GIVEN_NAME_EDIT, m_GivenName);
	DDV_MaxChars(pDX, m_GivenName, 64);
	DDX_Text(pDX, IDC_NETBIOS_NAME_EDIT, m_NetbiosName);
	DDX_Text(pDX, IDC_NETBIOS_DOMAIN_NAME_EDIT, m_NetBiosDomainName);
	DDX_Text(pDX, IDC_NETBIOS_USER_NAME_EDIT, m_UserName);
	//}}AFX_DATA_MAP
}

//virtual
LOGICAL CHostNamesControlForm::Entering()
{
	m_GivenName = m_pHost->GetGivenName();
	UpdateData(FALSE);
	return UpdateReadOnlyFields();
}

//virtual
LOGICAL CHostNamesControlForm::UpdateReadOnlyFields()
{
	// Store  RW fields
	UpdateData(TRUE);

	m_NetbiosName = m_pHost->GetOwnName();
	m_NetBiosDomainName = m_pHost->GetNetBiosDomainName();
	m_UserName = m_pHost->GetUserName();

	UpdateData(FALSE);
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CHostNamesControlForm::Leaving()
{
	OnApplyButton();
	return LOGICAL_TRUE;
}

BEGIN_MESSAGE_MAP(CHostNamesControlForm, CHostControlForm)
	//{{AFX_MSG_MAP(CHostNamesControlForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	ON_EN_CHANGE(IDC_GIVEN_NAME_EDIT, OnChangeGivenNameEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostNamesControlForm diagnostics

#ifdef _DEBUG
void CHostNamesControlForm::AssertValid() const
{
	CHostControlForm::AssertValid();
}

void CHostNamesControlForm::Dump(CDumpContext &dc) const
{
	CHostControlForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHostNamesControlForm message handlers

void CHostNamesControlForm::OnApplyButton() 
{
	UpdateData();
	if (m_pHost->SetGivenName(m_GivenName))
	{
		g_pWholeNetwork->HostChanged(m_pHost);
	}
	Entering();
	ClearModifiedFlag();
}

void CHostNamesControlForm::OnCancelButton() 
{
	Entering();
	ClearModifiedFlag();
}

void CHostNamesControlForm::OnChangeGivenNameEdit() 
{
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
// CSubnetNamesControlForm

IMPLEMENT_DYNCREATE(CSubnetNamesControlForm, CSubnetControlForm)

CSubnetNamesControlForm::CSubnetNamesControlForm()
	: CSubnetControlForm(IDD_SUBNET_NAMES_CONTROL_FORM)
{
	//{{AFX_DATA_INIT(CSubnetNamesControlForm)
	m_GivenName = _T("");
	//}}AFX_DATA_INIT
}

CSubnetNamesControlForm::~CSubnetNamesControlForm()
{
}

void CSubnetNamesControlForm::DoDataExchange(CDataExchange *pDX)
{
	CSubnetControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubnetNamesControlForm)
	DDX_Text(pDX, IDC_GIVEN_NAME_EDIT, m_GivenName);
	DDV_MaxChars(pDX, m_GivenName, 64);
	//}}AFX_DATA_MAP
}

//virtual
LOGICAL CSubnetNamesControlForm::Entering()
{
	m_GivenName = m_pSubnet->GetGivenName();
	UpdateData(FALSE);
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CSubnetNamesControlForm::Leaving()
{
	OnApplyButton();
	return LOGICAL_TRUE;
}


BEGIN_MESSAGE_MAP(CSubnetNamesControlForm, CSubnetControlForm)
	//{{AFX_MSG_MAP(CSubnetNamesControlForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	ON_EN_CHANGE(IDC_GIVEN_NAME_EDIT, OnChangeGivenNameEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubnetNamesControlForm diagnostics

#ifdef _DEBUG
void CSubnetNamesControlForm::AssertValid() const
{
	CSubnetControlForm::AssertValid();
}

void CSubnetNamesControlForm::Dump(CDumpContext &dc) const
{
	CSubnetControlForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSubnetNamesControlForm message handlers


void CSubnetNamesControlForm::OnApplyButton() 
{
	UpdateData();
	if (m_pSubnet->SetGivenName(m_GivenName))
	{
		g_pGui->SubnetChanged(m_pSubnet);
		g_pDB->SubnetChanged(m_pSubnet);
	}
	Entering();
}

void CSubnetNamesControlForm::OnCancelButton() 
{
	Entering();
}

void CSubnetNamesControlForm::OnChangeGivenNameEdit() 
{
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
// CHostAddressesControlForm

IMPLEMENT_DYNCREATE(CHostAddressesControlForm, CSubnetControlForm)

CHostAddressesControlForm::CHostAddressesControlForm()
	: CHostControlForm(IDD_HOST_ADDRESSES_CONTROL_FORM)
{
	//{{AFX_DATA_INIT(CHostAddressesControlForm)
	m_IpAddress = _T("");
	m_MacAddress = _T("");
	m_SubnetMask = _T("");
	//}}AFX_DATA_INIT
}

CHostAddressesControlForm::~CHostAddressesControlForm()
{
}

//virtual
LOGICAL CHostAddressesControlForm::UpdateReadOnlyFields()
{
	m_IpAddress = m_pHost->GetIpAddressString();
	m_SubnetMask = m_pHost->GetSubnetMaskString();
	m_MacAddress = m_pHost->GetMacAddress();
	UpdateData(FALSE);
	return LOGICAL_TRUE;
}

void CHostAddressesControlForm::DoDataExchange(CDataExchange *pDX)
{
	CHostControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostAddressesControlForm)
	DDX_Text(pDX, IDC_IPADDRESS_EDIT, m_IpAddress);
	DDX_Text(pDX, IDC_MACADDRESS_EDIT, m_MacAddress);
	DDX_Text(pDX, IDC_SUBNET_MASK_EDIT, m_SubnetMask);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHostAddressesControlForm, CHostControlForm)
	//{{AFX_MSG_MAP(CHostAddressesControlForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostAddressesControlForm diagnostics

#ifdef _DEBUG
void CHostAddressesControlForm::AssertValid() const
{
	CHostControlForm::AssertValid();
}

void CHostAddressesControlForm::Dump(CDumpContext &dc) const
{
	CHostControlForm::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CSubnetAddressControlForm

IMPLEMENT_DYNCREATE(CSubnetAddressControlForm, CSubnetControlForm)

CSubnetAddressControlForm::CSubnetAddressControlForm()
	: CSubnetControlForm(IDD_SUBNET_ADDRESS_CONTROL_FORM)
{
	//{{AFX_DATA_INIT(CSubnetAddressControlForm)
	m_IpAddress = _T("");
	m_SubnetMask = _T("");
	//}}AFX_DATA_INIT
}

CSubnetAddressControlForm::~CSubnetAddressControlForm()
{
}

//virtual
LOGICAL CSubnetAddressControlForm::Entering()
{
	m_IpAddress = m_pSubnet->GetIpAddressString();
	m_SubnetMask = m_pSubnet->GetSubnetMaskString();
	UpdateData(FALSE);
	return LOGICAL_TRUE;
}

void CSubnetAddressControlForm::DoDataExchange(CDataExchange *pDX)
{
	CSubnetControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubnetAddressControlForm)
	DDX_Text(pDX, IDC_IPADDRESS_EDIT, m_IpAddress);
	DDX_Text(pDX, IDC_SUBNET_MASK_EDIT, m_SubnetMask);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSubnetAddressControlForm, CSubnetControlForm)
	//{{AFX_MSG_MAP(CSubnetAddressControlForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubnetAddressControlForm diagnostics

#ifdef _DEBUG
void CSubnetAddressControlForm::AssertValid() const
{
	CSubnetControlForm::AssertValid();
}

void CSubnetAddressControlForm::Dump(CDumpContext &dc) const
{
	CSubnetControlForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSubnetAddressControlForm message handlers


/////////////////////////////////////////////////////////////////////////////
// CSubnetAppearanceControlForm

IMPLEMENT_DYNCREATE(CSubnetAppearanceControlForm, CSubnetControlForm)

CSubnetAppearanceControlForm::CSubnetAppearanceControlForm()
	: CSubnetControlForm(IDD_SUBNET_APPEARANCE_CONTROL_FORM)
{
	//{{AFX_DATA_INIT(CSubnetAppearanceControlForm)
	//}}AFX_DATA_INIT
}

CSubnetAppearanceControlForm::~CSubnetAppearanceControlForm()
{
}

//virtual
LOGICAL CSubnetAppearanceControlForm::Entering()
{
	m_ColorButton.SetColor(RGBAtoCOLORREF(m_pSubnet->GetColor()));
	m_Color.Init(m_pSubnet->GetColor());
	m_TextColorButton.SetColor(RGBAtoCOLORREF(m_pSubnet->GetTextColor()));
	m_TextColor.Init(m_pSubnet->GetTextColor());

	return LOGICAL_TRUE;
}

void CSubnetAppearanceControlForm::DoDataExchange(CDataExchange *pDX)
{
	CSubnetControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubnetAppearanceControlForm)
	DDX_Control(pDX, IDC_TEXT_COLOR_BUTTON, m_TextColorButton);
	DDX_Control(pDX, IDC_ORDINARY_COLOR_BUTTON, m_ColorButton);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSubnetAppearanceControlForm, CSubnetControlForm)
	//{{AFX_MSG_MAP(CSubnetAppearanceControlForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	ON_BN_CLICKED(IDC_ORDINARY_COLOR_BUTTON, OnOrdinaryColorButton)
	ON_BN_CLICKED(IDC_TEXT_COLOR_BUTTON, OnTextColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubnetAppearanceControlForm diagnostics

#ifdef _DEBUG
void CSubnetAppearanceControlForm::AssertValid() const
{
	CSubnetControlForm::AssertValid();
}

void CSubnetAppearanceControlForm::Dump(CDumpContext &dc) const
{
	CSubnetControlForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSubnetAppearanceControlForm message handlers

void CSubnetAppearanceControlForm::OnOrdinaryColorButton() 
{
	m_ColorButton.ChangeColor(&m_Color);
}

void CSubnetAppearanceControlForm::OnTextColorButton() 
{
	m_TextColorButton.ChangeColor(&m_TextColor);
}

void CSubnetAppearanceControlForm::OnApplyButton() 
{
	m_pSubnet->SetColors(m_Color, m_TextColor);

	g_pWholeNetwork->SubnetChanged(m_pSubnet);	
	ClearModifiedFlag();
}

void CSubnetAppearanceControlForm::OnCancelButton() 
{
	Entering();
}

/////////////////////////////////////////////////////////////////////////////
// CHostPingControlForm

IMPLEMENT_DYNCREATE(CHostPingControlForm, CHostControlForm)

CHostPingControlForm::CHostPingControlForm()
	: CHostControlForm(IDD_HOST_PING_CONTROL_FORM)
{
	//{{AFX_DATA_INIT(CHostPingControlForm)
	m_GroupParamsString = _T("");
	m_KeepAliveStatusStr = _T("");
	//}}AFX_DATA_INIT
}

CHostPingControlForm::~CHostPingControlForm()
{
}

void CHostPingControlForm::DoDataExchange(CDataExchange *pDX)
{
	CHostControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostPingControlForm)
	DDX_Control(pDX, IDC_GROUP_COMBO, m_GroupCombo);
	DDX_Text(pDX, IDC_GROUP_PARAMS_EDIT, m_GroupParamsString);
	DDX_Text(pDX, IDC_KEEPALIVE_STATUS_EDIT, m_KeepAliveStatusStr);
	//}}AFX_DATA_MAP
}

//virtual
LOGICAL CHostPingControlForm::Entering()
{
	m_GroupCombo.Init(g_pPingDefaultGroup);
	m_GroupCombo.SelectGroup(m_pHost->GetPingGroup());
	UpdateGroupParamsString();
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CHostPingControlForm::Leaving()
{
	return LOGICAL_TRUE;
}

//virtual
void CHostPingControlForm::NewGroupCreated(CGroup *pGroup)
{
	if (g_pPingDefaultGroup->IsEqualType(pGroup))
		m_GroupCombo.AddGroup(pGroup);
}

//virtual
void CHostPingControlForm::GroupChanged(CGroup *pGroup)
{
	CGroup *pSelectedGroup = m_GroupCombo.GetSelectedGroup();
	if (pSelectedGroup == pGroup)
		UpdateGroupParamsString();
	m_GroupCombo.UpdateGroupName(pGroup);
}

//virtual
void CHostPingControlForm::UpdateGroupParamsString()
{
	CGroup *pSelectedGroup = m_GroupCombo.GetSelectedGroup();
	if (!pSelectedGroup)
		return;
	m_GroupParamsString = pSelectedGroup->GetUserParamValuesStringGUI();
	UpdateData(FALSE);
}

//virtual
void CHostPingControlForm::OnUpdateStatus()
{
	char szTime[256];
	m_pHost->GetKeepAliveStatusStr(szTime, sizeof(szTime));
	m_KeepAliveStatusStr = szTime;
	UpdateData(FALSE);
}


BEGIN_MESSAGE_MAP(CHostPingControlForm, CHostControlForm)
	//{{AFX_MSG_MAP(CHostPingControlForm)
	ON_CBN_SELCHANGE(IDC_GROUP_COMBO, OnSelChangeGroupCombo)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostPingControlForm diagnostics

#ifdef _DEBUG
void CHostPingControlForm::AssertValid() const
{
	CHostControlForm::AssertValid();
}

void CHostPingControlForm::Dump(CDumpContext& dc) const
{
	CHostControlForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHostPingControlForm message handlers

void CHostPingControlForm::OnSelChangeGroupCombo() 
{
	UpdateGroupParamsString();	
}

void CHostPingControlForm::OnApplyButton() 
{
	SetGroup();
}

void CHostPingControlForm::OnCancelButton() 
{
	Entering();
}


/////////////////////////////////////////////////////////////////////////////
// CHostAppearanceControlForm

IMPLEMENT_DYNCREATE(CHostAppearanceControlForm, CHostControlForm)

CHostAppearanceControlForm::CHostAppearanceControlForm()
	: CHostControlForm(IDD_HOST_APPEARANCE_CONTROL_FORM)
{
	//{{AFX_DATA_INIT(CHostAppearanceControlForm)
	m_GroupParamsString = _T("");
	//}}AFX_DATA_INIT
}

CHostAppearanceControlForm::~CHostAppearanceControlForm()
{
}

void CHostAppearanceControlForm::DoDataExchange(CDataExchange *pDX)
{
	CHostControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostAppearanceControlForm)
	DDX_Control(pDX, IDC_GROUP_COMBO, m_GroupCombo);
	DDX_Text(pDX, IDC_GROUP_PARAMS_EDIT, m_GroupParamsString);
	//}}AFX_DATA_MAP
}

//virtual
LOGICAL CHostAppearanceControlForm::Entering()
{
	m_GroupCombo.Init(g_pAppearanceDefaultGroup);
	m_GroupCombo.SelectGroup(m_pHost->GetAppearanceGroup());
	UpdateGroupParamsString();
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CHostAppearanceControlForm::Leaving()
{
	return LOGICAL_TRUE;
}

//virtual
void CHostAppearanceControlForm::NewGroupCreated(CGroup *pGroup)
{
	if (g_pAppearanceDefaultGroup->IsEqualType(pGroup))
		m_GroupCombo.AddGroup(pGroup);
}

//virtual
void CHostAppearanceControlForm::GroupChanged(CGroup *pGroup)
{
	CGroup *pSelectedGroup = m_GroupCombo.GetSelectedGroup();
	if (pSelectedGroup == pGroup)
		UpdateGroupParamsString();
	m_GroupCombo.UpdateGroupName(pGroup);
}

//virtual
void CHostAppearanceControlForm::UpdateGroupParamsString()
{
	CGroup *pSelectedGroup = m_GroupCombo.GetSelectedGroup();
	if (!pSelectedGroup)
		return;
	m_GroupParamsString = pSelectedGroup->GetUserParamValuesStringGUI();
	UpdateData(FALSE);
}

//virtual
void CHostAppearanceControlForm::OnUpdateStatus()
{
	UpdateData(FALSE);
}


BEGIN_MESSAGE_MAP(CHostAppearanceControlForm, CHostControlForm)
	//{{AFX_MSG_MAP(CHostAppearanceControlForm)
	ON_CBN_SELCHANGE(IDC_GROUP_COMBO, OnSelChangeGroupCombo)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostAppearanceControlForm diagnostics

#ifdef _DEBUG
void CHostAppearanceControlForm::AssertValid() const
{
	CHostControlForm::AssertValid();
}

void CHostAppearanceControlForm::Dump(CDumpContext& dc) const
{
	CHostControlForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHostAppearanceControlForm message handlers

void CHostAppearanceControlForm::OnSelChangeGroupCombo() 
{
	UpdateGroupParamsString();	
}

void CHostAppearanceControlForm::OnApplyButton() 
{
	SetGroup();
}

void CHostAppearanceControlForm::OnCancelButton() 
{
	Entering();
}


/////////////////////////////////////////////////////////////////////////////
// CHostSnmpControlForm

IMPLEMENT_DYNCREATE(CHostSnmpControlForm, CHostControlForm)

CHostSnmpControlForm::CHostSnmpControlForm()
	: CHostControlForm(IDD_HOST_SNMP_CONTROL_FORM)
{
	//{{AFX_DATA_INIT(CHostSnmpControlForm)
	m_GroupParamsString = _T("");
	//}}AFX_DATA_INIT
}

CHostSnmpControlForm::~CHostSnmpControlForm()
{
}

void CHostSnmpControlForm::DoDataExchange(CDataExchange *pDX)
{
	CHostControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostSnmpControlForm)
	DDX_Control(pDX, IDC_GROUP_COMBO, m_GroupCombo);
	DDX_Text(pDX, IDC_GROUP_PARAMS_EDIT, m_GroupParamsString);
	//}}AFX_DATA_MAP
}

//virtual
LOGICAL CHostSnmpControlForm::Entering()
{
	m_GroupCombo.Init(g_pSnmpDefaultGroup);
	m_GroupCombo.SelectGroup(m_pHost->GetSnmpGroup());
	UpdateGroupParamsString();
	return LOGICAL_TRUE;
}

//virtual
LOGICAL CHostSnmpControlForm::Leaving()
{
	return LOGICAL_TRUE;
}

//virtual
void CHostSnmpControlForm::NewGroupCreated(CGroup *pGroup)
{
	if (g_pSnmpDefaultGroup->IsEqualType(pGroup))
		m_GroupCombo.AddGroup(pGroup);
}

//virtual
void CHostSnmpControlForm::GroupChanged(CGroup *pGroup)
{
	CGroup *pSelectedGroup = m_GroupCombo.GetSelectedGroup();
	if (pSelectedGroup == pGroup)
		UpdateGroupParamsString();
	m_GroupCombo.UpdateGroupName(pGroup);
}

//virtual
void CHostSnmpControlForm::UpdateGroupParamsString()
{
	CGroup *pSelectedGroup = m_GroupCombo.GetSelectedGroup();
	if (!pSelectedGroup)
		return;
	m_GroupParamsString = pSelectedGroup->GetUserParamValuesStringGUI();
	UpdateData(FALSE);
}

//virtual
void CHostSnmpControlForm::OnUpdateStatus()
{
	UpdateData(FALSE);
}


BEGIN_MESSAGE_MAP(CHostSnmpControlForm, CHostControlForm)
	//{{AFX_MSG_MAP(CHostSnmpControlForm)
	ON_CBN_SELCHANGE(IDC_GROUP_COMBO, OnSelChangeGroupCombo)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostSnmpControlForm diagnostics

#ifdef _DEBUG
void CHostSnmpControlForm::AssertValid() const
{
	CHostControlForm::AssertValid();
}

void CHostSnmpControlForm::Dump(CDumpContext& dc) const
{
	CHostControlForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHostSnmpControlForm message handlers

void CHostSnmpControlForm::OnSelChangeGroupCombo() 
{
	UpdateGroupParamsString();	
}

void CHostSnmpControlForm::OnApplyButton() 
{
	SetGroup();
}

void CHostSnmpControlForm::OnCancelButton() 
{
	Entering();
}

