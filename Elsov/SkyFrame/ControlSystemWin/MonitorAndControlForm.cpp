// CMonitorAndControlForm.cpp : implementation file
//

#include "stdafx.h"
#include "ControlSystemWin.h"
#include "ControlSystemView.h"
#include "ControlSystemDoc.h"
#include "TargetInstance.h"
#include "MonitorAndControlForm.h"
#include "Attached.h"
#include "ControlTargetAPI.h"
#include "ControlObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const char *pszTelnetOptionsStoringFormat = "%s : %d";

/////////////////////////////////////////////////////////////////////////////
// CMonitorAndControlForm

IMPLEMENT_DYNCREATE(CMonitorAndControlForm, CAbstractForm)

CMonitorAndControlForm::CMonitorAndControlForm()
	: CAbstractForm(CMonitorAndControlForm::IDD)
{
	//{{AFX_DATA_INIT(CMonitorAndControlForm)
	m_bUseParentConnection = FALSE;
	m_strTelnetIpAddress = _T("");
	m_TelnetPortNo = 0;
	m_SerialPortBusAddress = 0;
	//}}AFX_DATA_INIT
	m_pObject = NULL;
//	m_pTarget = NULL;
//	m_pTargetInstance = NULL;
}

CMonitorAndControlForm::~CMonitorAndControlForm()
{
}

void CMonitorAndControlForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorAndControlForm)
	DDX_Control(pDX, IDC_BUS_ADDRESS_SPIN, m_SerialPortBusAddressSpin);
	DDX_Control(pDX, IDC_TELNET_PORT_SPIN, m_TelnetPortSpin);
	DDX_Control(pDX, IDC_USE_PARENT_CONNECTION_CHECK, m_UseParentConnectionCheck);
	DDX_Control(pDX, IDC_PORT_COMBOBOX, m_PortComboBox);
	DDX_Check(pDX, IDC_USE_PARENT_CONNECTION_CHECK, m_bUseParentConnection);
	DDX_Text(pDX, IDC_TELNET_IP_ADDRESS_EDIT, m_strTelnetIpAddress);
	DDX_Text(pDX, IDC_TELNET_PORT_EDIT, m_TelnetPortNo);
	DDV_MinMaxInt(pDX, m_TelnetPortNo, 1, 1023);
	DDX_Text(pDX, IDC_BUS_ADDRESS_EDIT, m_SerialPortBusAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonitorAndControlForm, CAbstractForm)
	//{{AFX_MSG_MAP(CMonitorAndControlForm)
	ON_BN_CLICKED(IDC_USE_PARENT_CONNECTION_CHECK, OnUseParentConnectionCheck)
	ON_BN_CLICKED(IDC_SERIAL_PORT_RADIO, OnSerialPortRadio)
	ON_BN_CLICKED(IDC_GPIB_RADIO, OnGpibRadio)
	ON_BN_CLICKED(IDC_SNMP_RADIO, OnSnmpRadio)
	ON_BN_CLICKED(IDC_TELNET_RADIO, OnTelnetRadio)
	ON_BN_CLICKED(IDC_UNKNOWN_RADIO, OnUnknownRadio)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_SERIAL_PORT_CONFIGURE_BUTTON, OnSerialPortConfigureButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonitorAndControlForm diagnostics

#ifdef _DEBUG
void CMonitorAndControlForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CMonitorAndControlForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMonitorAndControlForm message handlers

void CMonitorAndControlForm::OnInitialUpdate() 
{
	CAbstractForm::OnInitialUpdate();
	m_PortComboBox.ResetContent();
	CString str;
	for (int i = 1; i <= 256; i++)
	{
		str.Format("Com%d", i);
		m_PortComboBox.SetItemData(m_PortComboBox.AddString(str), i);
	}

	// Apply button
	str.LoadString(IDS_APPLY_BUTTON);
	GetDescendantWindow(IDC_APPLY_BUTTON)->SetWindowText(str);
	// Configure button
	str.LoadString(IDS_CONFIGURE_BUTTON);
	GetDescendantWindow(IDC_SERIAL_PORT_CONFIGURE_BUTTON)->SetWindowText(str);
	
	m_TelnetPortSpin.SetRange(1, 1023);
}


//virtual
BOOL CMonitorAndControlForm::SetupControls()
{
	CAttachedBlockForObject *pAttachedForObject = (CAttachedBlockForObject *)m_pAttached;
	m_pObject = pAttachedForObject->GetObject();
	ASSERT(m_pObject);
//	m_pTarget = m_pAttached->GetTarget();
//	ASSERT(m_pTarget);
//	m_pTargetInstance = m_pAttached->GetTargetInstance();
//	ASSERT(m_pTargetInstance);

	m_pObject->MC_LoadMonitorAndControlParams();

	// Using of parent
	m_bUseParentConnection = m_pObject->GetMCParams()->m_bUSE_PARENT;
	
	// SerialPort
	for (int index = 0; index < m_PortComboBox.GetCount(); index++)
	{
		int PortNumber = m_PortComboBox.GetItemData(index);
		if (PortNumber == m_pObject->GetMCParams()->m_SerialPortNo)
			m_PortComboBox.SetCurSel(index);
	}
	m_SerialPortBusAddress = m_pObject->GetMCParams()->m_SerialPortBusAddress;

	// Telnet
	int TelnetPortNo = m_pObject->MC_GetDefaultTelnetPortNo();
	char szTelnetIpAddress[32]; szTelnetIpAddress[0] = 0;
	sscanf(m_pObject->GetMCParams()->m_TelnetOptions, pszTelnetOptionsStoringFormat, szTelnetIpAddress, &TelnetPortNo);
	m_strTelnetIpAddress = szTelnetIpAddress;
	m_TelnetPortSpin.SetPos(TelnetPortNo);
	m_TelnetPortNo = TelnetPortNo;
	
	// Connection type radio-buttons
	int RadioButtonID = IDC_UNKNOWN_RADIO;
	switch (m_pObject->GetMCParams()->m_TYPE)
	{
	case MCT_SerialPort:
		RadioButtonID = IDC_SERIAL_PORT_RADIO;
		break;
	case MCT_Telnet:
		RadioButtonID = IDC_TELNET_RADIO;
		break;
	case MCT_SNMP:
		RadioButtonID = IDC_SNMP_RADIO;
		break;
	case MCT_GPIB:
		RadioButtonID = IDC_GPIB_RADIO;
		break;

	default:
		RadioButtonID = IDC_UNKNOWN_RADIO;
	}
	CheckRadioButton(IDC_UNKNOWN_RADIO, IDC_GPIB_RADIO, RadioButtonID);

	UpdateData(FALSE);
	EnableControls();

	return TRUE;
}

void CMonitorAndControlForm::EnableControls()
{
	int ID = GetCheckedRadioButton(IDC_UNKNOWN_RADIO, IDC_GPIB_RADIO);
	UpdateData(TRUE);
	m_UseParentConnectionCheck.EnableWindow(m_pObject->MC_CanUseParentConnection());
	BOOL bSerialPort = !m_bUseParentConnection && m_pObject->MC_CanBeControlledBySerialPort() && ID == IDC_SERIAL_PORT_RADIO;
	BOOL bTelnet = !m_bUseParentConnection && m_pObject->MC_CanBeControlledByTelnet() && ID == IDC_TELNET_RADIO;
	BOOL bSNMP = !m_bUseParentConnection && m_pObject->MC_CanBeControlledBySNMP() && ID == IDC_SNMP_RADIO;
	BOOL bGPIB = !m_bUseParentConnection && m_pObject->MC_CanBeControlledByGPIB() && ID == IDC_GPIB_RADIO;

	m_PortComboBox.EnableWindow(bSerialPort);
	// Radio buttons
	GetDescendantWindow(IDC_UNKNOWN_RADIO)->EnableWindow(!m_bUseParentConnection);
	GetDescendantWindow(IDC_SERIAL_PORT_RADIO)->EnableWindow(!m_bUseParentConnection && m_pObject->MC_CanBeControlledBySerialPort());
	GetDescendantWindow(IDC_TELNET_RADIO)->EnableWindow(!m_bUseParentConnection && m_pObject->MC_CanBeControlledByTelnet());
	GetDescendantWindow(IDC_SNMP_RADIO)->EnableWindow(!m_bUseParentConnection && m_pObject->MC_CanBeControlledBySNMP());
	GetDescendantWindow(IDC_GPIB_RADIO)->EnableWindow(!m_bUseParentConnection && m_pObject->MC_CanBeControlledByGPIB());
	// Telnet group
	GetDescendantWindow(IDC_TELNET_IP_ADDRESS_EDIT)->EnableWindow(bTelnet);
	BOOL bTelnetPortNo = m_pObject->MC_CanChangeTelnetPortNo();
	GetDescendantWindow(IDC_TELNET_PORT_EDIT)->EnableWindow(bTelnet && bTelnetPortNo);
	GetDescendantWindow(IDC_TELNET_PORT_SPIN)->EnableWindow(bTelnet && bTelnetPortNo);
	//Serial port group
	BOOL bSerialPortConfigure = bSerialPort && m_pObject->MC_CanConfigureSerialPort();
	GetDescendantWindow(IDC_SERIAL_PORT_CONFIGURE_BUTTON)->EnableWindow(bSerialPortConfigure);
	BOOL bScanBusAddress = bSerialPort && m_pObject->MC_HasSerialPortBusAddress();
	m_SerialPortBusAddressSpin.EnableWindow(bScanBusAddress);
	GetDescendantWindow(IDC_BUS_ADDRESS_EDIT)->EnableWindow(bScanBusAddress);
	GetDescendantWindow(IDC_BUS_ADDRESS_SPIN)->EnableWindow(bScanBusAddress);
	GetDescendantWindow(IDC_SET_BUS_ADDRESS_BUTTON)->EnableWindow(bScanBusAddress);
	GetDescendantWindow(IDC_SCAN_BUTTON)->EnableWindow(bScanBusAddress);
}

//virtual
BOOL CMonitorAndControlForm::BeforeHiding()
{
	return TRUE;
}


void CMonitorAndControlForm::OnUseParentConnectionCheck() 
{
	EnableControls();
}

void CMonitorAndControlForm::OnSerialPortRadio() 
{
	EnableControls();
}

void CMonitorAndControlForm::OnGpibRadio() 
{
	EnableControls();
}

void CMonitorAndControlForm::OnSnmpRadio() 
{
	EnableControls();
}

void CMonitorAndControlForm::OnTelnetRadio() 
{
	EnableControls();
}

void CMonitorAndControlForm::OnUnknownRadio() 
{
	EnableControls();
}

void CMonitorAndControlForm::OnApplyButton() 
{
	UpdateData(TRUE);
	m_pObject->GetMCParams()->m_bUSE_PARENT = m_bUseParentConnection;

	// Serial port
	int index = m_PortComboBox.GetCurSel();
	if (index == CB_ERR)
		m_pObject->GetMCParams()->m_SerialPortNo = 0;
	else
		m_pObject->GetMCParams()->m_SerialPortNo = m_PortComboBox.GetItemData(index);
	m_pObject->GetMCParams()->m_SerialPortBusAddress = m_SerialPortBusAddress;

	// Radio buttons
	int RadioButtonID = GetCheckedRadioButton(IDC_UNKNOWN_RADIO, IDC_GPIB_RADIO);
	switch (RadioButtonID)
	{
	case IDC_SERIAL_PORT_RADIO :
		m_pObject->GetMCParams()->m_TYPE = MCT_SerialPort;
		break;
	case IDC_TELNET_RADIO :
		m_pObject->GetMCParams()->m_TYPE = MCT_Telnet;
		m_pObject->GetMCParams()->m_TelnetOptions.Format(pszTelnetOptionsStoringFormat, m_strTelnetIpAddress, m_TelnetPortNo);
		break;
	case IDC_SNMP_RADIO :
		m_pObject->GetMCParams()->m_TYPE = MCT_SNMP;
		break;
	case IDC_GPIB_RADIO :
		m_pObject->GetMCParams()->m_TYPE = MCT_GPIB;
		break;

	default:
		m_pObject->GetMCParams()->m_TYPE = MCT_Unknown;
		break;
	}
	m_pObject->MC_SaveMonitorAndControlParams();
}

void CMonitorAndControlForm::OnSerialPortConfigureButton() 
{
	int index = m_PortComboBox.GetCurSel();
	if (index == CB_ERR)
		return;

	CString strPortName;
	m_PortComboBox.GetLBText(index, strPortName);

	COMMCONFIG config;
	config.dwSize = sizeof(config);
	config.wVersion = 1;
	BOOL bSuccess = BuildCommDCB(m_pObject->GetMCParams()->m_SerialPortOptions, &config.dcb);
	if (!bSuccess)
	{
		DWORD dwErrorCode = GetLastError();
	}

	bSuccess = CommConfigDialog(strPortName, m_hWnd, &config);
	if (!bSuccess)
	{
		DWORD dwErrorCode = GetLastError();
		if (dwErrorCode == ERROR_CANCELLED)
			return; // canceled => do nothing
	}
	
	char szDummy[32];
	// Baud rate
	CString options = "baud=";
	options += itoa(config.dcb.BaudRate, szDummy, 10);
	// Parity
	options += " parity=";
	switch (config.dcb.Parity)
	{
	case 0: options += "N"; break;
	case 1: options += "O"; break;
	case 2: options += "E"; break;
	case 3: options += "M"; break;
	case 4: options += "S"; break;
	default: options += "N"; break;
	}
	// Byte size
	options += " data=";
	options += itoa(config.dcb.ByteSize, szDummy, 10);
	// Stop bits
	options += " stop=";
	switch (config.dcb.StopBits)
	{
	case 0: options += "1"; break;
	case 1: options += "1.5"; break;
	case 2: options += "2"; break;
	default: options += "1"; break;
	}

	m_pObject->GetMCParams()->m_SerialPortOptions = options;
}
