// HardwareTestFrame.cpp : implementation of the CHardwareTestFrame class
//

#include "stdafx.h"

#include "HardwareTestFrame.h"
#include "HardwareTestView.h"
#include "HardwareTestDoc.h"

// Forms
#include "TerminalForm.h"
#include "DeviceForm.h"
#include "ConnectionForm.h"
#include "DemodulatorCarrierForm.h"
#include "DemodulatorDataForm.h"
#include "DemodulatorLnbStatusForm.h"
#include "DemodulatorBerTestForm.h"
#include "ModulatorCarrierForm.h"
#include "ModulatorDataForm.h"
#include "ModulatorBucStatusForm.h"
#include "ModulatorBerTestForm.h"
#include "TranceiverForm.h"
#include "NsgScriptForm.h"
#include "PhysicalPortsForm.h"
#include "FrameRelayStationsForm.h"
#include "EthernetStationsForm.h"
#include "TelnetStationsForm.h"
#include "IPInterfacesForm.h"
#include "IPStaticRoutesForm.h"
#include "IPRoutingTableForm.h"
#include "PvcForm.h"
#include "UpConverterForm.h"
#include "DownConverterForm.h"
#include "SoftwareVersionForm.h"
#include "SdmsStartupConfigForm.h"
#include "EthernetPortForm.h"
#include "EthernetStatusForm.h"
#include "SerialPortForm.h"
#include "SerialStatusForm.h"
#include "SubstitutorForm.h"
#include "PowerMeterForm.h"
#include "FrequencyCounterForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestFrame

IMPLEMENT_DYNCREATE(CHardwareTestFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CHardwareTestFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CHardwareTestFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestFrame construction/destruction

CHardwareTestFrame::CHardwareTestFrame()
{
	// TODO: add member initialization code here
	
}

CHardwareTestFrame::~CHardwareTestFrame()
{
}

int CHardwareTestFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CHardwareTestFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestFrame diagnostics

#ifdef _DEBUG
void CHardwareTestFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CHardwareTestFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestFrame message handlers


BOOL CHardwareTestFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// create a splitter with 1 row, 2 columns
	if (!m_SplitterWnd.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	if (!m_SplitterWnd.CreateView(0, 0,
		RUNTIME_CLASS(CHardwareTestView), CSize(200, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - an input view in column 1
	if (!m_SplitterWnd.CreateView(0, 1,
		RUNTIME_CLASS(CAbstractForm), CSize(100, 100), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	SetActiveView((CView *)m_SplitterWnd.GetPane(0,0));
	CHardwareTestDoc *pDoc=(CHardwareTestDoc *)GetActiveDocument();
	pDoc->m_pTreeView=(CHardwareTestView *)m_SplitterWnd.GetPane(0,0);
	pDoc->m_pForms[0]=(CAbstractForm *)m_SplitterWnd.GetPane(0,1);
	
	pDoc->AddNewForm(TERMINAL_INDEX, new CTerminalForm(), &m_SplitterWnd);
	pDoc->AddNewForm(DEVICE_INDEX, new CDeviceForm(), &m_SplitterWnd);
	pDoc->AddNewForm(CONNECTION_INDEX, new CConnectionForm(), &m_SplitterWnd);
	pDoc->AddNewForm(RECEIVER_CARRIER_INDEX, new CDemodulatorCarrierForm(), &m_SplitterWnd);
	pDoc->AddNewForm(RECEIVER_DATA_INDEX, new CDemodulatorDataForm(), &m_SplitterWnd);
	pDoc->AddNewForm(RECEIVER_LNB_STATUS_INDEX, new CDemodulatorLnbStatusForm(), &m_SplitterWnd);
	pDoc->AddNewForm(RECEIVER_BERTEST_INDEX, new CDemodulatorBerTestForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TRANSMITTER_CARRIER_INDEX, new CModulatorCarrierForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TRANSMITTER_DATA_INDEX, new CModulatorDataForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TRANSMITTER_BUC_STATUS_INDEX, new CModulatorBucStatusForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TRANSMITTER_BERTEST_INDEX, new CModulatorBerTestForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TRANCEIVER_INDEX, new CTranceiverForm(), &m_SplitterWnd);
	pDoc->AddNewForm(NSG_SCRIPT_INDEX, new CNsgScriptForm(), &m_SplitterWnd);
	pDoc->AddNewForm(PHYSICAL_PORTS_INDEX, new CPhysicalPortsForm(), &m_SplitterWnd);
	pDoc->AddNewForm(FRAME_RELAY_STATIONS_INDEX, new CFrameRelayStationsForm(), &m_SplitterWnd);
	pDoc->AddNewForm(ETHERNET_STATIONS_INDEX, new CEthernetStationsForm(), &m_SplitterWnd);
	pDoc->AddNewForm(TELNET_STATIONS_INDEX, new CTelnetStationsForm(), &m_SplitterWnd);
	pDoc->AddNewForm(IP_INTERFACES_INDEX, new CIPInterfacesForm(), &m_SplitterWnd);
	pDoc->AddNewForm(IP_STATIC_ROUTES_INDEX, new CIPStaticRoutesForm(), &m_SplitterWnd);
	pDoc->AddNewForm(IP_ROUTING_TABLE_INDEX, new CIPRoutingTableForm(), &m_SplitterWnd);
	pDoc->AddNewForm(PVC_INDEX, new CPvcForm(), &m_SplitterWnd);
	pDoc->AddNewForm(UP_CONVERTER_INDEX, new CUpConverterForm(), &m_SplitterWnd);
	pDoc->AddNewForm(DOWN_CONVERTER_INDEX, new CDownConverterForm(), &m_SplitterWnd);
	pDoc->AddNewForm(SOFTWARE_VERSION_INDEX, new CSdmsSoftwareVersionForm(), &m_SplitterWnd);
	pDoc->AddNewForm(STARTUP_CONFIG_INDEX, new CSdmsStartupConfigForm(), &m_SplitterWnd);
	pDoc->AddNewForm(ETHERNET_PORT_INDEX, new CEthernetPortForm(), &m_SplitterWnd);
	pDoc->AddNewForm(ETHERNET_STATUS_INDEX, new CEthernetStatusForm(), &m_SplitterWnd);
	pDoc->AddNewForm(SERIAL_PORT_INDEX, new CSerialPortForm(), &m_SplitterWnd);
	pDoc->AddNewForm(SERIAL_STATUS_INDEX, new CSerialStatusForm(), &m_SplitterWnd);
	pDoc->AddNewForm(DEVICE_SUBSTITUTOR_INDEX, new CSubstitutorForm(), &m_SplitterWnd);
	pDoc->AddNewForm(POWER_METER_INDEX, new CPowerMeterForm(), &m_SplitterWnd);
	pDoc->AddNewForm(FREQUENCY_COUNTER_INDEX, new CFrequencyCounterForm(), &m_SplitterWnd);

	return TRUE;
}
