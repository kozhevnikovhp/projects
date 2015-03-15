// HardwareTestView.cpp : implementation of the CHardwareTestView class
//

#include "stdafx.h"
#include "Attached.h"

#include "HardwareTestDoc.h"
#include "HardwareTestView.h"
#include "HardwareTestFrame.h"

// Device interfaces
#include "Modem.h"
#include "PowerMeter.h"
#include "FrequencyCounter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestView

IMPLEMENT_DYNCREATE(CHardwareTestView, CTreeView)

BEGIN_MESSAGE_MAP(CHardwareTestView, CTreeView)
	//{{AFX_MSG_MAP(CHardwareTestView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestView construction/destruction

CHardwareTestView::CHardwareTestView()
{
	m_PrevSelection=NULL;
	m_pTreeCtrl=NULL;
	m_pCurrentlyVisibleForm=NULL;
/*	font.CreateFont ( 20, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET,
					  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					  DEFAULT_PITCH|FF_ROMAN, "System" );*/
}

CHardwareTestView::~CHardwareTestView()
{
}

void CHardwareTestView::Clear()
{
	m_PrevSelection=NULL;
	m_pTreeCtrl->DeleteAllItems();
	Refresh();
}

void CHardwareTestView::Refresh()
{
	m_pTreeCtrl->SetRedraw(TRUE);
}

BOOL CHardwareTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestView drawing

void CHardwareTestView::OnDraw(CDC* pDC)
{
	CHardwareTestDoc* pDoc = GetDocument();
}

void CHardwareTestView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

 	CHardwareTestFrame *pFrame=(CHardwareTestFrame *)GetParentFrame();
	m_pAbstractPane = (CAbstractForm *)pFrame->GetPane(0,1);
//	SetFont(&font);
	m_ImageList.Create(IDB_BITMAP_TREE,16,5,RGB(192,192,192));
	m_pTreeCtrl->SetImageList(&m_ImageList, TVSIL_NORMAL);

	long lStyleOld;
	lStyleOld = GetWindowLong(m_pTreeCtrl->m_hWnd, GWL_STYLE);
	lStyleOld |= (TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
}

HTREEITEM CHardwareTestView::InsertTerminalItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Terminal", 2, 2, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(TERMINAL_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertConnectionItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Monitor & Control", 2, 2, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(CONNECTION_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertSoftwareVersionItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem("Software version", 2, 2, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(SOFTWARE_VERSION_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertStartupConfigItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem("Startup config", 2, 2, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(STARTUP_CONFIG_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertEthernetPortItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem("Ethernet port", 7, 7, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(ETHERNET_PORT_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertEthernetStatusItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem("Ethernet port status", 7, 7, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(ETHERNET_STATUS_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertSerialPortItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem("Serial port", 7, 7, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(SERIAL_PORT_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertSerialStatusItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem("Serial port status", 7, 7, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(SERIAL_STATUS_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertTranceiverItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Tranceiver", 5, 5, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(TRANCEIVER_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertDemodulatorCarrierItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Carrier", 3, 3, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(RECEIVER_CARRIER_INDEX, pDevice);
	pAttached->m_DeviceNumber=nDevice;
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertDemodulatorDataItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Data", 4, 4, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(RECEIVER_DATA_INDEX, pDevice);
	pAttached->m_DeviceNumber=nDevice;
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertModulatorCarrierItem(HTREEITEM ParentItem, CDevice *pDevice, int nDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Carrier", 3, 3, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(TRANSMITTER_CARRIER_INDEX, pDevice);
	pAttached->m_DeviceNumber = nDevice;
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertModulatorDataItem(HTREEITEM ParentItem, CDevice *pDevice,	int nDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Data", 4, 4, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(TRANSMITTER_DATA_INDEX, pDevice);
	pAttached->m_DeviceNumber = nDevice;
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertBucStatusItem(HTREEITEM ParentItem, CDevice *pDevice,	int nDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("BUC", 9, 9, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(TRANSMITTER_BUC_STATUS_INDEX, pDevice);
	pAttached->m_DeviceNumber = nDevice;
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertLnbStatusItem(HTREEITEM ParentItem, CDevice *pDevice,	int nDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("LNB", 9, 9, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(RECEIVER_LNB_STATUS_INDEX, pDevice);
	pAttached->m_DeviceNumber = nDevice;
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertModulatorBerTestItem(HTREEITEM ParentItem, CDevice *pDevice,	int nDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem("BER-Test", 5, 5, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(TRANSMITTER_BERTEST_INDEX, pDevice);
	pAttached->m_DeviceNumber = nDevice;
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertDemodulatorBerTestItem(HTREEITEM ParentItem, CDevice *pDevice,	int nDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem("BER-Test", 5, 5, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(RECEIVER_BERTEST_INDEX, pDevice);
	pAttached->m_DeviceNumber = nDevice;
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertNsgScriptItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Script", 4, 4, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(NSG_SCRIPT_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertPhysicalPortsItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Physical Ports", 4, 4, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(PHYSICAL_PORTS_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertFrameRelayStationsItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Frame Relay Stations", 4, 4, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(FRAME_RELAY_STATIONS_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertEthernetStationsItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Ethernet Stations", 4, 4, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(ETHERNET_STATIONS_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertTelnetStationsItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Telnet Stations", 4, 4, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(TELNET_STATIONS_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertIPInterfacesItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("IP Interfaces", 6, 6, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(IP_INTERFACES_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertIPStaticRoutesItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Static Routes", 6, 6, ParentItem);
	CAttachedBlock *pAttached=new CAttachedBlock(IP_STATIC_ROUTES_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertIPRoutingTableItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Routing Table", 8, 8, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(IP_ROUTING_TABLE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertX25RoutingItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	return NULL;
}

HTREEITEM CHardwareTestView::InsertPVCsItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("PVCs", 4, 4, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(PVC_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

HTREEITEM CHardwareTestView::InsertSubstitutorItem(HTREEITEM ParentItem, CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem("Enable/disable", 4, 4, ParentItem);
	CAttachedBlock *pAttached = new CAttachedBlock(DEVICE_SUBSTITUTOR_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	return Item;
}

void CHardwareTestView::InsertModem(HTREEITEM ParentItem, CDevice *pDevice)
{
	CModem *pModem=(CModem *)pDevice;

	CString ItemName;
	for (int i=1; i <= pModem->GetModulatorsNumber(); i++)
	{
		HTREEITEM ModulatorItem=ParentItem;
		if (pModem->GetModulatorsNumber() + pModem->GetDemodulatorsNumber()>1)
		{
			ItemName.Format("Modulator %d", i);
			ModulatorItem = m_pTreeCtrl->InsertItem(ItemName, 0, 0, ParentItem);
		}
		InsertModulatorCarrierItem(ModulatorItem, pDevice, i);
		InsertModulatorDataItem(ModulatorItem, pDevice, i);
		if (pModem->CanGetBucStatus())
			InsertBucStatusItem(ModulatorItem, pDevice, i);
		BOOL bCanBerTest = FALSE;
		for (int Power = 1; Power <= 23; Power++)
			if (pModem->CanBerTest(Power))
				bCanBerTest = TRUE;
		if (bCanBerTest)
			InsertModulatorBerTestItem(ModulatorItem, pDevice, i);
	}

	for (i=1; i <= pModem->GetDemodulatorsNumber(); i++)
	{
		HTREEITEM DemodulatorItem=ParentItem;
		if (pModem->GetModulatorsNumber()+pModem->GetDemodulatorsNumber()>1)
		{
			ItemName.Format("Demodulator %d", i);
			DemodulatorItem=m_pTreeCtrl->InsertItem(ItemName, 1, 1, ParentItem);
		}
		InsertDemodulatorCarrierItem(DemodulatorItem, pDevice, i);
		InsertDemodulatorDataItem(DemodulatorItem, pDevice, i);
		if (pModem->CanGetLnbStatus())
			InsertLnbStatusItem(DemodulatorItem, pDevice, i);
		BOOL bCanBerTest = FALSE;
		for (int Power = 1; Power <= 23; Power++)
			if (pModem->CanBerTest(Power))
				bCanBerTest = TRUE;
		if (bCanBerTest)
			InsertDemodulatorBerTestItem(DemodulatorItem, pDevice, i);
	}
}

void CHardwareTestView::AddModulator(CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem(pDevice->GetName(), 0, 0);
	CAttachedBlock *pAttached = new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	InsertConnectionItem(Item, pDevice);
	InsertModem(Item, pDevice);
	if (pDevice->CanAsciiControl())
		InsertTerminalItem(Item, pDevice);
}

void CHardwareTestView::AddModem(CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem(pDevice->GetName(), 3, 3);
	CAttachedBlock *pAttached = new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	InsertConnectionItem(Item, pDevice);
	InsertModem(Item, pDevice);
	if (pDevice->CanAsciiControl())
		InsertTerminalItem(Item, pDevice);
}

void CHardwareTestView::AddStation(CDevice *pDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem(pDevice->GetName(), 9, 9);
	CAttachedBlock *pAttached = new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	InsertConnectionItem(Item, pDevice);
	InsertModem(Item, pDevice);
	HTREEITEM EthernetItem = InsertEthernetPortItem(Item, pDevice);
	InsertEthernetStatusItem(EthernetItem, pDevice);
	HTREEITEM SerialItem = InsertSerialPortItem(Item, pDevice);
	InsertSerialStatusItem(SerialItem, pDevice);
	InsertSoftwareVersionItem(Item, pDevice);
	InsertStartupConfigItem(Item, pDevice);
	if (pDevice->CanAsciiControl())
		InsertTerminalItem(Item, pDevice);
}

void CHardwareTestView::AddDemodulator(CDevice *pDevice)
{
	HTREEITEM Item = m_pTreeCtrl->InsertItem(pDevice->GetName(), 1, 1);
	CAttachedBlock *pAttached = new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);

	InsertConnectionItem(Item, pDevice);
	InsertModem(Item, pDevice);
	if (pDevice->CanAsciiControl())
		InsertTerminalItem(Item, pDevice);
}

void CHardwareTestView::AddTranceiver(CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem(pDevice->GetName(), 5, 5);
	CAttachedBlock *pAttached = new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);

	InsertConnectionItem(Item, pDevice);
	InsertTranceiverItem(Item, pDevice);
}

void CHardwareTestView::AddUpDownConverter(CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem(pDevice->GetName(), 3, 3);
	CAttachedBlock *pAttached = new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	InsertConnectionItem(Item, pDevice);

	HTREEITEM ConverterItem;
	ConverterItem = Item;
	if (pDevice->IsUpConverter())
	{
		ConverterItem = m_pTreeCtrl->InsertItem("UpConverter", 0, 0, Item);
		pAttached = new CAttachedBlock(UP_CONVERTER_INDEX, pDevice);
		m_pTreeCtrl->SetItemData(ConverterItem, (DWORD)pAttached);
	}

	ConverterItem = Item;
	if (pDevice->IsDownConverter())
	{
		ConverterItem = m_pTreeCtrl->InsertItem("DownConverter", 1, 1, Item);
		pAttached = new CAttachedBlock(DOWN_CONVERTER_INDEX, pDevice);
		m_pTreeCtrl->SetItemData(ConverterItem, (DWORD)pAttached);
	}

	if (pDevice->CanAsciiControl())
		InsertTerminalItem(Item, pDevice);
}

void CHardwareTestView::AddDeviceSubstitutor(CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem(pDevice->GetName(), 5, 5);
	CAttachedBlock *pAttached=new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);

	InsertConnectionItem(Item, pDevice);
	InsertSubstitutorItem(Item, pDevice);
}

void CHardwareTestView::AddPowerMeter(CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem(pDevice->GetName(), 5, 5);
	CAttachedBlock *pAttached=new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	InsertConnectionItem(Item, pDevice);
	CString ItemName;

	CPowerMeter *pPowerMeter = (CPowerMeter *)pDevice;
	for (int nDevice = 1; nDevice <= pPowerMeter->GetChannelsNumber(); nDevice++)
	{
		ItemName.Format("Channel %c", 'A'+nDevice-1);
		HTREEITEM ChannelItem = m_pTreeCtrl->InsertItem(ItemName, 5, 5, Item);
		pAttached = new CAttachedBlock(POWER_METER_INDEX, pDevice);
		pAttached->m_DeviceNumber = nDevice;
		m_pTreeCtrl->SetItemData(ChannelItem, (DWORD)pAttached);
	}
	if (pDevice->CanAsciiControl())
		InsertTerminalItem(Item, pDevice);
}

void CHardwareTestView::AddFrequencyCounter(CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem(pDevice->GetName(), 5, 5);
	CAttachedBlock *pAttached = new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);
	InsertConnectionItem(Item, pDevice);
	CString ItemName;

	CFrequencyCounter *pFrequencyCounter = (CFrequencyCounter *)pDevice;
	for (int nDevice = 1; nDevice <= pFrequencyCounter->GetChannelsNumber(); nDevice++)
	{
		ItemName.Format("Channel %c", 'A'+nDevice-1);
		HTREEITEM ChannelItem = m_pTreeCtrl->InsertItem(ItemName, 5, 5, Item);
		pAttached = new CAttachedBlock(FREQUENCY_COUNTER_INDEX, pDevice);
		pAttached->m_DeviceNumber = nDevice;
		m_pTreeCtrl->SetItemData(ChannelItem, (DWORD)pAttached);
	}
	if (pDevice->CanAsciiControl())
		InsertTerminalItem(Item, pDevice);
}

void CHardwareTestView::AddNsgFrad(CDevice *pDevice)
{
	HTREEITEM Item=m_pTreeCtrl->InsertItem(pDevice->GetName(), 7, 7);
	CAttachedBlock *pAttached=new CAttachedBlock(DEVICE_INDEX, pDevice);
	m_pTreeCtrl->SetItemData(Item, (DWORD)pAttached);

	InsertConnectionItem(Item, pDevice);
	InsertPhysicalPortsItem(Item, pDevice);

	// Virtual objects (Frame Relay and Ethernet stations)
	HTREEITEM VirtualObjectsItem=m_pTreeCtrl->InsertItem("VirtualObjects", 4, 4, Item);
	InsertFrameRelayStationsItem(VirtualObjectsItem, pDevice);
	InsertEthernetStationsItem(VirtualObjectsItem, pDevice);
	InsertTelnetStationsItem(VirtualObjectsItem, pDevice);

	// Routing
	HTREEITEM RoutingItem=m_pTreeCtrl->InsertItem("Routing", 5, 5, Item);
	HTREEITEM IPRoutingItem=m_pTreeCtrl->InsertItem("IP Routing", 5, 5, RoutingItem);
	// IP
	InsertIPInterfacesItem(IPRoutingItem, pDevice);
	InsertIPStaticRoutesItem(IPRoutingItem, pDevice);
	InsertIPRoutingTableItem(IPRoutingItem, pDevice);
	// X.25
	InsertX25RoutingItem(RoutingItem, pDevice);
	// PVC
	InsertPVCsItem(RoutingItem, pDevice);
	
	InsertNsgScriptItem(Item, pDevice);
	if (pDevice->CanAsciiControl())
		InsertTerminalItem(Item, pDevice);
}

CDevice *CHardwareTestView::DeleteSelectedDevice()
{
	HTREEITEM Item = m_pTreeCtrl->GetSelectedItem();
	if (!Item)
		return NULL;

	CAttachedBlock *pAttached = (CAttachedBlock *)m_pTreeCtrl->GetItemData(Item);
	CAbstractForm *pCurrentForm = GetDocument()->m_pForms[pAttached->m_ViewIndex];
	if (!pCurrentForm->CanBeDeleted())
		return NULL;
	
	m_pTreeCtrl->SelectItem(NULL);
	m_PrevSelection = NULL;
	m_pCurrentlyVisibleForm = NULL;

	CDevice *pDevice = pAttached->m_pDevice;
	m_pTreeCtrl->DeleteItem(Item);
	return pDevice;
}

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestView printing

BOOL CHardwareTestView::OnPreparePrinting(CPrintInfo *pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CHardwareTestView::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
	// TODO: add extra initialization before printing
}

void CHardwareTestView::OnEndPrinting(CDC *pDC, CPrintInfo *pInfo)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestView diagnostics

#ifdef _DEBUG
void CHardwareTestView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CHardwareTestView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CHardwareTestDoc* CHardwareTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHardwareTestDoc)));
	return (CHardwareTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestView message handlers

void CHardwareTestView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CWaitCursor cursor;

	CHardwareTestFrame *pFrame=(CHardwareTestFrame *)GetParentFrame();
	CAbstractForm *pOld = (CAbstractForm *)pFrame->GetPane(0,1);
	m_pCurrentlyVisibleForm = NULL;
	HTREEITEM Item=m_pTreeCtrl->GetSelectedItem();
	if (!Item)
	{
		TurnToAbstractPane(pOld);
		return;
	}

	CAttachedBlock *pOldPtr=NULL, *pNewPtr=(CAttachedBlock *)m_pTreeCtrl->GetItemData(Item);
	if (m_PrevSelection)
		pOldPtr=(CAttachedBlock *)m_pTreeCtrl->GetItemData(m_PrevSelection);

	if (!pNewPtr)
	{
		TurnToAbstractPane(pOld);
		return;
	}

	CAbstractForm *pNew=GetDocument()->m_pForms[pNewPtr->m_ViewIndex];

	UINT OldIDD=pOld->GetDlgCtrlID();
	UINT NewIDD=pNew->GetDlgCtrlID();
	if (OldIDD==NewIDD && Item==m_PrevSelection) return;
	if (pOldPtr)
	{
		if (!pOld->BeforeHiding())
		{
			m_pTreeCtrl->SelectItem(m_PrevSelection);
			return;
		}
	}

	pNew->m_pAttached = pNewPtr;
	CDevice *pDevice = pNewPtr->m_pDevice;
	pNew->m_DeviceNumber = pNewPtr->m_DeviceNumber;
	if (pNew->SetupControls())
	{
		m_PrevSelection = Item;
		m_pCurrentlyVisibleForm = pNew;
		if (OldIDD != NewIDD)
		{
			pOld->ShowWindow(SW_HIDE);
			pNew->ShowWindow(SW_SHOW);
			pOld->SetDlgCtrlID(NewIDD);
			pNew->SetDlgCtrlID(OldIDD);
			pFrame->Refresh();
		}
	}
	else
	{
		MessageBeep(0);
		m_pTreeCtrl->SelectItem(m_PrevSelection);
	}
	*pResult = 0;
}

void CHardwareTestView::TurnToAbstractPane(CAbstractForm *pOld)
{
	CHardwareTestFrame *pFrame=(CHardwareTestFrame *)GetParentFrame();

	UINT OldIDD=pOld->GetDlgCtrlID();
	UINT NewIDD=m_pAbstractPane->GetDlgCtrlID();
	pOld->ShowWindow(SW_HIDE);
	m_pAbstractPane->ShowWindow(SW_SHOW);
	pOld->SetDlgCtrlID(NewIDD);
	m_pAbstractPane->SetDlgCtrlID(OldIDD);
	if (m_PrevSelection)
		pOld->BeforeHiding();
	pFrame->Refresh();
}

void CHardwareTestView::RefreshCurrentForm()
{
	CWaitCursor cursor;
	if (m_pCurrentlyVisibleForm)
		m_pCurrentlyVisibleForm->SetupControls();
}

int CHardwareTestView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_TimerID=SetTimer(1,1000, NULL);
	
	m_pTreeCtrl=&GetTreeCtrl();
	
	return 0;
}

void CHardwareTestView::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;
	CAttachedBlock *pAttached=(CAttachedBlock *)pNMTreeView->itemOld.lParam;
	delete pAttached;
	*pResult = 0;
}

void CHardwareTestView::OnDestroy() 
{
	KillTimer(m_TimerID);
	CTreeView::OnDestroy();	
}

void CHardwareTestView::OnTimer(UINT nIDEvent) 
{
	if (m_pCurrentlyVisibleForm)
		m_pCurrentlyVisibleForm->Timer();
	CTreeView::OnTimer(nIDEvent);
}
