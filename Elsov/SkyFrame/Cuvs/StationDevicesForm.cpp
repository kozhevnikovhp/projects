// StationDevicesForm.cpp : implementation file
//

#include "stdafx.h"
#include "StationDevicesForm.h"
#include "CuvsDoc.h"
#include "Station.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationDevicesForm

IMPLEMENT_DYNCREATE(CStationDevicesForm, CAbstractForm)

CStationDevicesForm::CStationDevicesForm()
	: CAbstractForm(CStationDevicesForm::IDD)
{
	//{{AFX_DATA_INIT(CStationDevicesForm)
	m_Name = _T("");
	//}}AFX_DATA_INIT
}

CStationDevicesForm::~CStationDevicesForm()
{
}

void CStationDevicesForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationDevicesForm)
	DDX_Control(pDX, IDC_TYPE_COMBO, m_DeviceTypeCombo);
	DDX_Control(pDX, IDC_DEVICE_NAME_EDIT, m_NameEditCtrl);
	DDX_Control(pDX, IDC_DEVICE_IPADDRESS, m_IpAddressCtrl);
	DDX_Control(pDX, IDC_DEVICES_TAB, m_DevicesTabCtrl);
	DDX_Text(pDX, IDC_DEVICE_NAME_EDIT, m_Name);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStationDevicesForm, CAbstractForm)
	//{{AFX_MSG_MAP(CStationDevicesForm)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_NOTIFY(TCN_SELCHANGE, IDC_DEVICES_TAB, OnSelChangeDevicesTab)
	ON_BN_CLICKED(IDC_APPLY_BUTTON1, OnApplyButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationDevicesForm diagnostics

#ifdef _DEBUG
void CStationDevicesForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CStationDevicesForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CStationDevicesForm::SetupControls()
{
	CString DeviceName;
	long DeviceID;
	TC_ITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = TCIF_TEXT | TCIF_PARAM;

	m_DevicesTabCtrl.DeleteAllItems();
	m_pDoc->QueryStationDevices(m_pAttached->GetStationID());
	while (m_pDoc->GetNextStationDevice(DeviceID, DeviceName))
	{
		item.lParam = DeviceID;
		item.pszText = (char *)LPCTSTR(DeviceName);
		m_DevicesTabCtrl.InsertItem(m_DevicesTabCtrl.GetItemCount(), &item);
		++item.lParam;
	}
	m_DevicesTabCtrl.SetCurSel(0);
	OnSelChangeDevicesTab(NULL, NULL);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CStationDevicesForm message handlers

void CStationDevicesForm::OnInitialUpdate() 
{
	CAbstractForm::OnInitialUpdate();
	long DeviceType;
	CString DeviceTypeDescription;
	m_pDoc->QueryDeviceTypes();
	while (m_pDoc->GetNextDeviceType(DeviceType, DeviceTypeDescription))
	{
		int index = m_DeviceTypeCombo.AddString(DeviceTypeDescription);
		m_DeviceTypeCombo.SetItemData(index, DeviceType);
	}
}

void CStationDevicesForm::OnAddButton() 
{
	m_pDoc->CreateStationDevice(m_pAttached->GetStationID(), 1);
	SetupControls();
	m_pAttached->m_pStation->RebuildDevices();
}

void CStationDevicesForm::OnSelChangeDevicesTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	long IPAddress;
	long Type;
	BOOL bEnable = TRUE;
	int index = m_DevicesTabCtrl.GetCurSel();
	if (index != -1)
	{
		TC_ITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = TCIF_PARAM;
		m_DevicesTabCtrl.GetItem(index, &item);
		m_pDoc->GetStationDevice(item.lParam, m_Name, Type, IPAddress);
		for (int i = 0; i<m_DeviceTypeCombo.GetCount(); i++)
			if (m_DeviceTypeCombo.GetItemData(i) == (DWORD)Type)
				m_DeviceTypeCombo.SetCurSel(i);
	}
	else
	{
		m_Name = afxChNil;
		IPAddress = 0;
		bEnable = FALSE;
	}
	m_IpAddressCtrl.SetAddress(IPAddress);
	UpdateData(FALSE);
	m_NameEditCtrl.EnableWindow(bEnable);
	m_IpAddressCtrl.EnableWindow(bEnable);

	if (pResult)
		*pResult = 0;
}

void CStationDevicesForm::OnApplyButton() 
{
	int index = m_DevicesTabCtrl.GetCurSel();
	if (index != -1)
	{
		UpdateData();
		TC_ITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = TCIF_PARAM;
		m_DevicesTabCtrl.GetItem(index, &item);
		DWORD IPAddress;
		m_IpAddressCtrl.GetAddress(IPAddress);
		m_pDoc->SetStationDevice(item.lParam, m_Name, m_DeviceTypeCombo.GetCurSel()+1, (long)IPAddress);
		item.mask = TCIF_PARAM | TCIF_TEXT;
		item.pszText = (char *)LPCTSTR(m_Name);
		m_DevicesTabCtrl.SetItem(index, &item);
		m_pAttached->m_pStation->RebuildDevices();
	}
}

void CStationDevicesForm::OnDeleteButton() 
{
	int index = m_DevicesTabCtrl.GetCurSel();
	if (index != -1)
	{
		TC_ITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = TCIF_PARAM;
		m_DevicesTabCtrl.GetItem(index, &item);
		m_pDoc->DeleteStationDevice(item.lParam);
		SetupControls();
		m_pAttached->m_pStation->RebuildDevices();
	}
}
