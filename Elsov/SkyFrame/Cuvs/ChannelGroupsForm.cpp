// ChannelGroupsForm.cpp : implementation file
//

#include "stdafx.h"
#include "ChannelGroupsForm.h"
#include "CuvsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChannelGroupsForm

IMPLEMENT_DYNCREATE(CChannelGroupsForm, CAbstractForm)

CChannelGroupsForm::CChannelGroupsForm()
	: CAbstractForm(CChannelGroupsForm::IDD)
{
	//{{AFX_DATA_INIT(CChannelGroupsForm)
	m_bAutoSN = FALSE;
	m_MaxSN = 0.0;
	m_MinSN = 0.0;
	//}}AFX_DATA_INIT
}

CChannelGroupsForm::~CChannelGroupsForm()
{
}

void CChannelGroupsForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelGroupsForm)
	DDX_Control(pDX, IDC_MIN_SN_EDIT, m_MinSNCtrl);
	DDX_Control(pDX, IDC_MAX_SN_EDIT, m_MaxSNCtrl);
	DDX_Control(pDX, IDC_AUTO_SN_CHECK, m_AutoSNCtrl);
	DDX_Control(pDX, IDC_MODULATORS_LIST, m_TransmitterList);
	DDX_Control(pDX, IDC_DEMODULATORS_LIST, m_ReceiverList);
	DDX_Check(pDX, IDC_AUTO_SN_CHECK, m_bAutoSN);
	DDX_Text(pDX, IDC_MAX_SN_EDIT, m_MaxSN);
	DDX_Text(pDX, IDC_MIN_SN_EDIT, m_MinSN);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChannelGroupsForm, CAbstractForm)
	//{{AFX_MSG_MAP(CChannelGroupsForm)
	ON_LBN_SELCHANGE(IDC_MODULATORS_LIST, OnSelChangeModulatorsList)
	ON_CLBN_CHKCHANGE(IDC_DEMODULATORS_LIST, OnCheckChangeDemodulatorsList)
	ON_LBN_SELCHANGE(IDC_DEMODULATORS_LIST, OnSelChangeDemodulatorsList)
	ON_BN_CLICKED(IDC_APPLY_SN_BUTTON, OnApplySnButton)
	ON_BN_CLICKED(IDC_AUTO_SN_CHECK, OnAutoSnCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannelGroupsForm diagnostics

#ifdef _DEBUG
void CChannelGroupsForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CChannelGroupsForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CChannelGroupsForm::SetupControls()
{
	int index;
	m_TransmitterList.ResetContent();
	m_ReceiverList.ResetContent();

	long DeviceID;
	long DeviceType;
	CString StationName, DeviceName;
	m_pDoc->QueryStationDevicesAndChildDevices(m_pAttached->GetStationID());
	while (m_pDoc->GetNextDeviceAndChildDevice(DeviceID, DeviceName, DeviceType, StationName))
	{
		if (DeviceType == DeviceType_ModemDatumSDMS || DeviceType == DeviceType_ModemRadyne)
		{ // modems only
			CString str = StationName + " : ";
			str += DeviceName;
			index = m_TransmitterList.AddString(str);
			m_TransmitterList.SetItemData(index, DeviceID);
		}
	}
	m_TransmitterList.SetCurSel(0);
	OnSelChangeModulatorsList();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChannelGroupsForm message handlers

void CChannelGroupsForm::OnSelChangeModulatorsList() 
{
	int index = m_TransmitterList.GetCurSel();
	if (index == LB_ERR)
		return;

	m_ReceiverList.ResetContent();
	long TransmitterID = m_TransmitterList.GetItemData(index);
	long TransmitterID_DB;
	BOOL bChannelExists;

	long ReceiverID;
	long ReceiverType;
	CString StationName, DeviceName;
	m_pDoc->QueryStationDevicesAndChildDevices(m_pAttached->GetStationID());
	while (m_pDoc->GetNextDeviceAndChildDevice(ReceiverID, DeviceName, ReceiverType, StationName))
	{
		// find channel with ReceiverID
		if (bChannelExists = m_pDoc->QueryChannelByReceiverID(TransmitterID_DB, ReceiverID))
		{ // such channel exists
			if (TransmitterID != TransmitterID_DB)
				continue;
		}
		if (ReceiverType == DeviceType_ModemDatumSDMS || ReceiverType == DeviceType_ModemRadyne)
		{ // modems only
			CString str = StationName + " : ";
			str += DeviceName;
			index = m_ReceiverList.AddString(str);
			m_ReceiverList.SetItemData(index, ReceiverID);
			if (bChannelExists)
				m_ReceiverList.SetCheck(index, 1);
		}
	}
	m_ReceiverList.SetCurSel(0);
	OnSelChangeDemodulatorsList();
}

void CChannelGroupsForm::OnCheckChangeDemodulatorsList() 
{
	int ReceiverIndex = m_ReceiverList.GetCurSel();
	if (ReceiverIndex == LB_ERR)
		return;
	long ReceiverID = m_ReceiverList.GetItemData(ReceiverIndex);

	int TransmitterIndex = m_TransmitterList.GetCurSel();
	if (TransmitterIndex == LB_ERR)
		return;
	long TransmitterID = m_TransmitterList.GetItemData(TransmitterIndex);

	if (m_ReceiverList.GetCheck(ReceiverIndex))
	{ // add new record
		m_pDoc->AddNewChannel(TransmitterID, ReceiverID);
	}
	else
	{ // delete existing record
		m_pDoc->DeleteChannel(TransmitterID, ReceiverID);
	}
	OnSelChangeDemodulatorsList();
}

void CChannelGroupsForm::OnSelChangeDemodulatorsList() 
{
	int ReceiverIndex = m_ReceiverList.GetCurSel();
	if (ReceiverIndex == LB_ERR)
		return;
	long ReceiverID = m_ReceiverList.GetItemData(ReceiverIndex);
	
	// find channel with ReceiverID
	BOOL bChannelExists =  m_pDoc->QueryChannelByReceiverID(ReceiverID, m_bAutoSN, m_MinSN, m_MaxSN);
	int nCmdShow = bChannelExists ? SW_SHOW : SW_HIDE;
	m_MinSNCtrl.ShowWindow(nCmdShow);
	m_MaxSNCtrl.ShowWindow(nCmdShow);
	m_AutoSNCtrl.ShowWindow(nCmdShow);
	GetDescendantWindow(IDC_DOTS_STATIC)->ShowWindow(nCmdShow);
	GetDescendantWindow(IDC_DECIBELS_STATIC)->ShowWindow(nCmdShow);
	GetDescendantWindow(IDC_APPLY_SN_BUTTON)->ShowWindow(nCmdShow);
	GetDescendantWindow(IDC_CHANNEL_QUALITY_STATIC)->ShowWindow(nCmdShow);
	UpdateData(FALSE);
	OnAutoSnCheck();
}

void CChannelGroupsForm::OnApplySnButton() 
{
	int ReceiverIndex = m_ReceiverList.GetCurSel();
	if (ReceiverIndex == LB_ERR)
		return;
	long ReceiverID = m_ReceiverList.GetItemData(ReceiverIndex);
	
	UpdateData();
	m_pDoc->EditChannelByReceiverID(ReceiverID, m_bAutoSN, m_MinSN, m_MaxSN);
}

void CChannelGroupsForm::OnAutoSnCheck() 
{
	UpdateData();
	m_MinSNCtrl.EnableWindow(m_bAutoSN);
	m_MaxSNCtrl.EnableWindow(m_bAutoSN);
	GetDescendantWindow(IDC_DOTS_STATIC)->EnableWindow(m_bAutoSN);
	GetDescendantWindow(IDC_DECIBELS_STATIC)->EnableWindow(m_bAutoSN);
}
