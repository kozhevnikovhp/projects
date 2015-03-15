// ConnectionForm.cpp : implementation file
//

#include "stdafx.h"
#include "ConnectionForm.h"
#include "Attached.h"
#include "Device.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectionForm

IMPLEMENT_DYNCREATE(CConnectionForm, CAbstractForm)

CConnectionForm::CConnectionForm()
	: CAbstractForm(CConnectionForm::IDD)
{
	//{{AFX_DATA_INIT(CConnectionForm)
	m_TcpIpAdress = _T("");
	m_BusAddress = 0;
	//}}AFX_DATA_INIT
}

CConnectionForm::~CConnectionForm()
{
}

void CConnectionForm::DoDataExchange(CDataExchange* pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectionForm)
	DDX_Control(pDX, IDC_BUS_ADDRESS_SPIN, m_BusAddressSpin);
	DDX_Control(pDX, IDC_PORT_COMBOBOX, m_PortComboBox);
	DDX_Text(pDX, IDC_TELNET_IP_ADDRESS_EDIT, m_TcpIpAdress);
	DDX_Text(pDX, IDC_BUS_ADDRESS_EDIT, m_BusAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectionForm, CAbstractForm)
	//{{AFX_MSG_MAP(CConnectionForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_BN_CLICKED(IDC_DISCONNECT_BUTTON, OnDisconnectButton)
	ON_BN_CLICKED(IDC_TELNET_RADIO, OnTelnetRadio)
	ON_BN_CLICKED(IDC_SERIAL_PORT_RADIO, OnSerialPortRadio)
	ON_BN_CLICKED(IDC_SET_BUS_ADDRESS_BUTTON, OnSetBusAddressButton)
	ON_BN_CLICKED(IDC_SCAN_BUTTON, OnScanButton)
	ON_BN_CLICKED(IDC_SNMP_RADIO, OnSnmpRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionForm diagnostics

#ifdef _DEBUG
void CConnectionForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CConnectionForm::Dump(CDumpContext& dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConnectionForm message handlers

void CConnectionForm::OnInitialUpdate()
{
	CAbstractForm::OnInitialUpdate();
	m_PortComboBox.ResetContent();
	CString str;
	for (int i=1; i <= 256; i++)
	{
		str.Format("Com%d", i);
		m_PortComboBox.SetItemData(m_PortComboBox.AddString(str), i);
	}
}

BOOL CConnectionForm::SetupControls()
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	switch (pDevice->GetControlType())
	{
	case MC_SERIAL_PORT:
	case MC_UNKNOWN_TYPE:
		CheckRadioButton(IDC_SERIAL_PORT_RADIO, IDC_GPIB_RADIO, IDC_SERIAL_PORT_RADIO);
		m_TcpIpAdress.Empty();
		break;
	case MC_TELNET:
		CheckRadioButton(IDC_SERIAL_PORT_RADIO, IDC_GPIB_RADIO, IDC_TELNET_RADIO);
		m_TcpIpAdress = pDevice->GetConnectionName();
		break;
	case MC_SNMP:
		CheckRadioButton(IDC_SERIAL_PORT_RADIO, IDC_GPIB_RADIO, IDC_SNMP_RADIO);
		m_TcpIpAdress = pDevice->GetConnectionName();
		break;
	case MC_GPIB:
		CheckRadioButton(IDC_SERIAL_PORT_RADIO, IDC_GPIB_RADIO, IDC_GPIB_RADIO);
		break;
	}
	EnableControls();
	m_BusAddress = pDevice->GetMCBusAddress();
	m_BusAddressSpin.SetRange32(pDevice->GetMinMCBusAddressScanning(), pDevice->GetMaxMCBusAddressScanning());
	
	if (pDevice->IsControllable())
	{
		for (int index = 0; index < m_PortComboBox.GetCount(); index++)
		{
			CString PortName;
			m_PortComboBox.GetLBText(index, PortName);
			if (!PortName.Compare(pDevice->GetConnectionName()))
				m_PortComboBox.SetCurSel(index);
		}
	}
	else
		m_PortComboBox.SetCurSel(-1);
	GetDescendantWindow(IDC_STATUS_STATIC)->SetWindowText(pDevice->GetConnectionStatus());
	UpdateData(FALSE);
	return TRUE;
}

void CConnectionForm::OnApplyButton() 
{
	OnDisconnectButton();
	CWaitCursor cursor;
	CDevice *pDevice=m_pAttached->m_pDevice;
	UpdateData();
	
	int ID = GetCheckedRadioButton(IDC_SERIAL_PORT_RADIO, IDC_GPIB_RADIO);
	switch (ID)
	{
	case IDC_SERIAL_PORT_RADIO:
		{
			pDevice->SetControlType(MC_SERIAL_PORT);
			pDevice->SetMCBusAddress(m_BusAddress);
			int index = m_PortComboBox.GetCurSel();
			if (index == CB_ERR)
				MessageBox("Specify correct port, please!", "Incorrect parameter", MB_ICONEXCLAMATION);
			else
			{
				CString ConnectionName;
				m_PortComboBox.GetLBText(index, ConnectionName);
			//	if (ConnectionName.Compare(pDevice->GetConnectionName()) || !pDevice->IsControllable())
					pDevice->Connect(LPCTSTR(ConnectionName));
			}
			break;
		}
	case IDC_TELNET_RADIO:
		{
			pDevice->SetControlType(MC_TELNET);
			pDevice->SetMCBusAddress(m_BusAddress);
			unsigned char b1=0, b2=0, b3=0, b4=0;
			sscanf(LPCTSTR(m_TcpIpAdress), "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
			m_TcpIpAdress.Format("%d.%d.%d.%d", b1, b2, b3, b4);
			pDevice->Connect(m_TcpIpAdress);
			UpdateData(FALSE);
			break;
		}
	case IDC_SNMP_RADIO:
		{
			pDevice->SetControlType(MC_SNMP);
			pDevice->SetMCBusAddress(m_BusAddress);
			unsigned char b1=0, b2=0, b3=0, b4=0;
			sscanf(LPCTSTR(m_TcpIpAdress), "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
			m_TcpIpAdress.Format("%d.%d.%d.%d", b1, b2, b3, b4);
			pDevice->Connect(m_TcpIpAdress);
			UpdateData(FALSE);
			break;
		}
	case IDC_GPIB_RADIO:
		MessageBox("GPIB interface is not implemented yet", "Sorry", MB_OK);
		break;
	}

	GetDescendantWindow(IDC_STATUS_STATIC)->SetWindowText(pDevice->GetConnectionStatus());
}

void CConnectionForm::OnDisconnectButton() 
{
	CWaitCursor cursor;
	CDevice *pDevice=m_pAttached->m_pDevice;
	pDevice->Disconnect();

	GetDescendantWindow(IDC_STATUS_STATIC)->SetWindowText(pDevice->GetConnectionStatus());
}

void CConnectionForm::OnTelnetRadio() 
{
	EnableControls();	
}

void CConnectionForm::OnSerialPortRadio() 
{
	EnableControls();	
}

void CConnectionForm::OnSnmpRadio() 
{
	EnableControls();	
}

void CConnectionForm::EnableControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	int ID = GetCheckedRadioButton(IDC_SERIAL_PORT_RADIO, IDC_GPIB_RADIO);
	BOOL bComPort = pDevice->CanBeControlledByComPort() && ID == IDC_SERIAL_PORT_RADIO;
	BOOL bTelnet = pDevice->CanBeControlledByTcpIp() && ID == IDC_TELNET_RADIO;
	BOOL bSnmp = pDevice->CanBeControlledBySNMP() && ID == IDC_SNMP_RADIO;
	BOOL bTcpIp = bTelnet | bSnmp;
	m_PortComboBox.EnableWindow(bComPort);
	GetDescendantWindow(IDC_TELNET_IP_ADDRESS_EDIT)->EnableWindow(bTcpIp);
	GetDescendantWindow(IDC_SERIAL_PORT_RADIO)->EnableWindow(pDevice->CanBeControlledByComPort());
	GetDescendantWindow(IDC_TELNET_RADIO)->EnableWindow(pDevice->CanBeControlledByTcpIp());
	GetDescendantWindow(IDC_SNMP_RADIO)->EnableWindow(pDevice->CanBeControlledBySNMP());
	GetDescendantWindow(IDC_GPIB_RADIO)->EnableWindow(pDevice->CanBeControlledByGPIB());
	m_BusAddressSpin.EnableWindow(pDevice->HasMCBusAddress());
	GetDescendantWindow(IDC_BUS_ADDRESS_EDIT)->EnableWindow(pDevice->HasMCBusAddress());
	GetDescendantWindow(IDC_BUS_ADDRESS_SPIN)->EnableWindow(pDevice->HasMCBusAddress());
	GetDescendantWindow(IDC_SET_BUS_ADDRESS_BUTTON)->EnableWindow(pDevice->HasMCBusAddress());
	GetDescendantWindow(IDC_SCAN_BUTTON)->EnableWindow(pDevice->HasMCBusAddress());
}

void CConnectionForm::OnSetBusAddressButton() 
{
	CDevice *pDevice=m_pAttached->m_pDevice;
	UpdateData();
	
	pDevice->SetControlType(MC_SERIAL_PORT);
	pDevice->SetMCBusAddress(m_BusAddress);
}

void CConnectionForm::OnScanButton() 
{
	CScanMCBusAddressDialog dlg;
	CDevice *pDevice=m_pAttached->m_pDevice;
	dlg.SetRange(pDevice->GetMinMCBusAddressScanning(), pDevice->GetMaxMCBusAddressScanning());
	if (dlg.DoModal() != IDOK)
		return;

	CString str;
	CProgressDlg ProgressDlg;
	ProgressDlg.Create();
	ProgressDlg.SetRange(dlg.m_StartScanning, dlg.m_StopScanning);
	for (UINT nAddress = dlg.m_StartScanning; nAddress <= dlg.m_StopScanning; nAddress++)
	{
		str.Format("Scanned address %d", nAddress);
		ProgressDlg.SetStatus(str);
		if (ProgressDlg.CheckCancelButton())
			break;
		if (pDevice->CheckMCBusAddress(nAddress))
		{
			str.Format("Bus address %d looks like valid.\nContinue scanning?", nAddress);
			if (MessageBox(str, "Information", MB_YESNO) != IDYES)
				break;
		}
		ProgressDlg.StepIt();
	}
	m_BusAddress = 	pDevice->GetMCBusAddress();
	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CScanMCBusAddressDialog dialog


CScanMCBusAddressDialog::CScanMCBusAddressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScanMCBusAddressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScanMCBusAddressDialog)
	m_StartScanning = 0;
	m_StopScanning = 0;
	//}}AFX_DATA_INIT
}


void CScanMCBusAddressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanMCBusAddressDialog)
	DDX_Control(pDX, IDC_STOP_SCANNING_SPIN, m_StopScanningSpin);
	DDX_Control(pDX, IDC_START_SCANNING_SPIN, m_StartScanningSpin);
	DDX_Text(pDX, IDC_START_SCANNING_EDIT, m_StartScanning);
	DDX_Text(pDX, IDC_STOP_SCANNING_EDIT, m_StopScanning);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanMCBusAddressDialog, CDialog)
	//{{AFX_MSG_MAP(CScanMCBusAddressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CScanMCBusAddressDialog::SetRange(UINT Start, UINT Stop)
{
	m_StartScanning = Start;
	m_StopScanning = Stop;
}

/////////////////////////////////////////////////////////////////////////////
// CScanMCBusAddressDialog message handlers

BOOL CScanMCBusAddressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_StartScanningSpin.SetRange32(m_StartScanning, m_StopScanning);
	m_StopScanningSpin.SetRange32(m_StartScanning, m_StopScanning);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

