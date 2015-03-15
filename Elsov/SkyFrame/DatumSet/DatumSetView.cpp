// DatumSetView.cpp : implementation of the CDatumSetView class
//

#include "stdafx.h"
#include "DatumSet.h"

#include "DatumSetDoc.h"
#include "DatumSetView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char szPortName[32];
char szPortConfigureString[32];
const char *pszIniFile = "DatumSet.ini";
const char *pszSection = "Port";
const char *pszPortName = "Name";
const char *pszPortConfig = "Configuration";
const char *pszRemoteAddress = "RemoteAddress";
const char *pszUnknown = "<unknown>";

////////////////////////////////////////////////////////////////////////////
// CDatumSetView

IMPLEMENT_DYNCREATE(CDatumSetView, CFormView)

BEGIN_MESSAGE_MAP(CDatumSetView, CFormView)
	//{{AFX_MSG_MAP(CDatumSetView)
	ON_BN_CLICKED(IDC_CLOSE_BUTTON, OnCloseButton)
	ON_BN_CLICKED(IDC_OPEN_BUTTON, OnOpenButton)
	ON_BN_CLICKED(IDC_CONFIGURE_BUTTON, OnConfigureButton)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_GET_INFO_BUTTON, OnGetInfoButton)
	ON_BN_CLICKED(IDC_SET_MODEL_NAME_BUTTON, OnSetModelNameButton)
	ON_BN_CLICKED(IDC_SET_UNIT_ID_BUTTON, OnSetUnitIdButton)
	ON_BN_CLICKED(IDC_SET_SERIAL_NO_BUTTON, OnSetSerialNoButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatumSetView construction/destruction

CDatumSetView::CDatumSetView()
	: CFormView(CDatumSetView::IDD)
{
	//{{AFX_DATA_INIT(CDatumSetView)
	m_ModeString = _T("");
	m_PortStatus = _T("");
	m_RemoteAddress = 0;
	m_UnitID = _T("");
	m_ModelName = _T("");
	m_SerialNo = 0;
	//}}AFX_DATA_INIT
	m_UnitID = pszUnknown;
	m_ModelName = pszUnknown;
}

CDatumSetView::~CDatumSetView()
{
}

void CDatumSetView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatumSetView)
	DDX_Control(pDX, IDC_REMOTE_SPIN, m_RemoteAddressSpin);
	DDX_Control(pDX, IDC_REMOTE_ADDRESS_EDIT, m_RemoteAddressEdit);
	DDX_Control(pDX, IDC_PORT_COMBO, m_PortCombo);
	DDX_Text(pDX, IDC_MODE_EDIT, m_ModeString);
	DDX_Text(pDX, IDC_STATUS_EDIT, m_PortStatus);
	DDX_Text(pDX, IDC_REMOTE_ADDRESS_EDIT, m_RemoteAddress);
	DDX_Text(pDX, IDC_UNIT_ID_EDIT, m_UnitID);
	DDX_Text(pDX, IDC_MODEL_NAME_EDIT, m_ModelName);
	DDX_Text(pDX, IDC_SERIAL_NO_EDIT, m_SerialNo);
	//}}AFX_DATA_MAP
}

BOOL CDatumSetView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CDatumSetView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	GetPrivateProfileString(pszSection, pszPortName, "Com1", szPortName, sizeof(szPortName)/sizeof(szPortName[0]), pszIniFile);
	GetPrivateProfileString(pszSection, pszPortConfig, "9600,n,8,1", szPortConfigureString, sizeof(szPortConfigureString)/sizeof(szPortConfigureString[0]), pszIniFile);
	m_RemoteAddress = (BYTE)GetPrivateProfileInt(pszSection, pszRemoteAddress, 1, pszIniFile);
	m_RemoteAddressSpin.SetBuddy(&m_RemoteAddressEdit);
	m_RemoteAddressSpin.SetRange(1, 254);
	m_RemoteAddressSpin.SetPos(m_RemoteAddress);

	m_PortCombo.ResetContent();
	CString str;
	for (int i=1; i <= 256; i++)
	{
		str.Format("Com%d", i);
		int index = m_PortCombo.AddString(str);
		m_PortCombo.SetItemData(index, i);
		if (!str.CompareNoCase(szPortName))
			m_PortCombo.SetCurSel(index);
	}

	m_ModeString = szPortConfigureString;

	UpdateData(FALSE);

	OnOpenButton();
}

/////////////////////////////////////////////////////////////////////////////
// CDatumSetView diagnostics

#ifdef _DEBUG
void CDatumSetView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDatumSetView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDatumSetDoc* CDatumSetView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDatumSetDoc)));
	return (CDatumSetDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDatumSetView message handlers

void CDatumSetView::OnCloseButton() 
{
	m_Modem.GetSerialPort()->Close();
	m_PortStatus = m_Modem.GetSerialPort()->GetStatusString();
	UpdateData(FALSE);
}

void CDatumSetView::OnOpenButton() 
{
	int index = m_PortCombo.GetCurSel();
	if (index == CB_ERR)
	{	
		MessageBox("Specify correct port, please!", "Incorrect parameter", MB_ICONEXCLAMATION);
		return;
	}
	else
	{
		m_PortCombo.GetLBText(index, szPortName);
		m_Modem.GetSerialPort()->Open(szPortName);
	}
	m_PortStatus = m_Modem.GetSerialPort()->GetStatusString();
	UpdateData(FALSE);

	m_Modem.SetControlType(MC_SERIAL_PORT);
	m_Modem.Connect(szPortName);

	WritePrivateProfileString(pszSection, pszPortName, szPortName, pszIniFile);
}

void CDatumSetView::OnConfigureButton() 
{
	int index = m_PortCombo.GetCurSel();
	if (index == CB_ERR)
		return;

	m_PortCombo.GetLBText(index, szPortName);

	if (!Comm_GuiUpdateConfigureString(szPortName, szPortConfigureString))
		return;  // canceled => do nothing

	
	m_ModeString = szPortConfigureString;
	UpdateData(FALSE);

	WritePrivateProfileString(pszSection, pszPortConfig, szPortConfigureString, pszIniFile);
	OnOpenButton();
}

void CDatumSetView::OnDestroy() 
{
	m_Modem.GetSerialPort()->Close();
	CFormView::OnDestroy();
}

void CDatumSetView::OnGetInfoButton() 
{
	CWaitCursor cursor;
	UpdateData();
	m_Modem.SetMCBusAddress(m_RemoteAddress);

	m_UnitID = pszUnknown;
	char *pszUnitName;
	if (m_Modem.GetUnitName(pszUnitName) == MC_OK)
		m_UnitID = pszUnitName;

	m_SerialNo = 0;
	m_Modem.GetSerialNo(m_SerialNo);
	
	char szStr[128];
	m_ModelName = pszUnknown;
	if (m_Modem.GetModelName(szStr) == MC_OK)
		m_ModelName = szStr;
	
	UpdateData(FALSE);
}

void CDatumSetView::OnSetModelNameButton() 
{
	UpdateData();
	m_Modem.SetMCBusAddress(m_RemoteAddress);
	char *pszModelName = (char *)LPCTSTR(m_ModelName);
	m_Modem.SetModelName(pszModelName);
	//OnGetInfoButton();
}

void CDatumSetView::OnSetUnitIdButton() 
{
	UpdateData();
	char *pszUnitName = (char *)LPCTSTR(m_UnitID);
	m_Modem.SetUnitName(pszUnitName);
	OnGetInfoButton();
}

void CDatumSetView::OnSetSerialNoButton() 
{
	UpdateData();
	m_Modem.SetMCBusAddress(m_RemoteAddress);
	m_Modem.SetSerialNo(m_SerialNo);
}
