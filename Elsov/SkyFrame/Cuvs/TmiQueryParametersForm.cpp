// CTmiQueryParametersForm.cpp : implementation file
//

#include "stdafx.h"
#include "TmiQueryParametersForm.h"
#include "CuvsDoc.h"
#include "Station.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTmiAttachedBlock

//static
//CRITICAL_SECTION CTmiAttachedBlock::m_Critical;
//static
//BOOL CTmiAttachedBlock::m_bCriticalSectionInitialized = FALSE;

CTmiAttachedBlock::CTmiAttachedBlock(int ViewIndex, CAbstractForm *pForm)
	: CAttachedBlock(ViewIndex, pForm)
{
	m_TmiParamSet.Open();
	m_TmiToBeAskedSet.m_pDatabase = m_TmiParamSet.m_pDatabase;
	m_TmiToBeAskedSet.Open();
	Run(this);
}

//virtual
CTmiAttachedBlock::~CTmiAttachedBlock()
{
	m_TmiParamSet.Close();
	m_TmiToBeAskedSet.Close();
}

//virtual
BOOL CTmiAttachedBlock::Run(void *a_pArgList)
{
	return RunOwnThread(ThreadProcedure, a_pArgList);
}

//static
unsigned ( __stdcall CTmiAttachedBlock::ThreadProcedure) (void *a_pParams)
{
	CTmiAttachedBlock *pThis = (CTmiAttachedBlock *)a_pParams;
	TRACE("CTmiAttachedBlock::ThreadProcedure started\n");
	srand(GetTickCount());

	while (pThis->IsAlive())
	{
		if (!pThis->NeedContinueThread())
			break; // terminate thread
		pThis->OneCycle();
		int wait = rand() % 10000;
		Sleep(10000 + wait);
	}

	SetEvent(pThis->m_hContinueThreadEvent);
	TRACE("CTmiObject::ThreadProcedure ended\n");
	_endthreadex(0);
	return 0;
}

void CTmiAttachedBlock::OneCycle()
{
	long TmiType, IpAddress, Period, DeviceID;
	CTime LastTime, CurrentTime = CTime::GetCurrentTime();
	QueryTmiToBeAsked(m_pStation->GetStationID());
	while (GetNextTmiParamToBeAsked(DeviceID, TmiType, IpAddress, LastTime, Period))
	{
		int days = Period/24/60;
		Period = Period - days*24*60;
		int hours = Period/60;
		Period = Period - hours *60;
		int mins = Period;
		CTimeSpan DeltaTimeMax = CTimeSpan(days, hours, mins, 0);
		if ((CurrentTime - LastTime) > DeltaTimeMax)
		{ // query
			switch (TmiType)
			{
			case 1: //outdoor equipment
				break;
			case 2://demodulator status
				GetDemodulatorStatus();
				break;
			case 3: //modulator status
				break;
			case 4: //IP - statistics
				break;
			case 5: //ICMP - statistics
				break;
			case 6: //TCP - statistics
				break;
			case 7: //UDP - statistics
				break;
			case 8: //Ethernet
				break;
			case 9: //SerialInterface
				break;
			case 10: //UpTime
				break;
			}// End of switch (TmiType)
			TRACE("\tQuery: IP=%d TmiType=%d\n", IpAddress, TmiType);
			TmiReceived(DeviceID, TmiType, CurrentTime);
		}
	}
	TRACE("TMI of Station %d is required\n", m_pStation->GetStationID());
}

BOOL CTmiAttachedBlock::GetDemodulatorStatus()
{
/*	MC_ErrorCode EC = m_pStation->GetDemodulatorStatus();
	if (EC == MC_OK)
	{
		double SN, Level;
		int Offset;
		// S/N
		m_pStation->GetDemodulatorEbNo(SN);
		// Input Level
		m_pStation->GetDemodulatorInputLevel(Level);
		// Offset
		m_pStation->GetDemodulatorOffset(Offset);
		return TRUE;
	}
	else
		return FALSE;*/
	return TRUE;
}

BOOL CTmiAttachedBlock::QueryTmiToBeAsked(long StationID)
{
	TRY
	{
		m_TmiToBeAskedSet.m_strFilter = "StationDevices.DeviceID = TmiQueryParameters.DeviceID AND StationID = ? AND Enabled = TRUE";
		m_TmiToBeAskedSet.m_Param1 = StationID;
		m_TmiToBeAskedSet.m_strSort = "IP";
		m_TmiToBeAskedSet.Requery();
		if (m_TmiToBeAskedSet.IsEOF())
			return TRUE;
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CTmiAttachedBlock::GetNextTmiParamToBeAsked(long &DeviceID, long &TmiType, long &IpAddress, CTime &LastTmiTime, long &Period)
{
	TRY
		if (m_TmiToBeAskedSet.IsEOF())
		{
			return FALSE;
		}
		DeviceID = m_TmiToBeAskedSet.m_DeviceID;
		TmiType = m_TmiToBeAskedSet.m_TmiType;
		IpAddress = m_TmiToBeAskedSet.m_IP;
		LastTmiTime = m_TmiToBeAskedSet.m_LastTmi;
		Period = m_TmiToBeAskedSet.m_Period;
		m_TmiToBeAskedSet.MoveNext();
		return TRUE;
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}

BOOL CTmiAttachedBlock::TmiReceived(long DeviceID, long TmiType, CTime CurrentTime)
{
	TRY
	{
		m_TmiParamSet.m_strFilter = "DeviceID = ? AND TmiType = ?";
		m_TmiParamSet.m_Param1 = DeviceID;
		m_TmiParamSet.m_Param2 = TmiType;
		m_TmiParamSet.Requery();
		if (m_TmiParamSet.IsEOF())
			return FALSE;
		m_TmiParamSet.Edit();
		m_TmiParamSet.m_LastTmi = CurrentTime;
		m_TmiParamSet.Update();
		return TRUE;
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при чтении/записи ГБД", MB_ICONEXCLAMATION);
		return FALSE;
	END_CATCH
}


/////////////////////////////////////////////////////////////////////////////
// CTmiQueryParametersForm

IMPLEMENT_DYNCREATE(CTmiQueryParametersForm, CAbstractForm)

CTmiQueryParametersForm::CTmiQueryParametersForm()
	: CAbstractForm(CTmiQueryParametersForm::IDD)
{
	//{{AFX_DATA_INIT(CTmiQueryParametersForm)
	m_Period = 0;
	//}}AFX_DATA_INIT
}

CTmiQueryParametersForm::~CTmiQueryParametersForm()
{
}

void CTmiQueryParametersForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTmiQueryParametersForm)
	DDX_Control(pDX, IDC_PERIOD_SPIN, m_PeriodSpin);
	DDX_Control(pDX, IDC_PERIOD_EDIT, m_PeriodEdit);
	DDX_Control(pDX, IDC_TYPES_LIST, m_TmiTypesListBox);
	DDX_Control(pDX, IDC_DEVICES_TAB, m_DevicesTabCtrl);
	DDX_Text(pDX, IDC_PERIOD_EDIT, m_Period);
	DDV_MinMaxLong(pDX, m_Period, 1, 1440);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTmiQueryParametersForm, CAbstractForm)
	//{{AFX_MSG_MAP(CTmiQueryParametersForm)
	ON_NOTIFY(TCN_SELCHANGE, IDC_DEVICES_TAB, OnSelChangeDevicesTab)
	ON_CLBN_CHKCHANGE(IDC_TYPES_LIST, OnCheckChangeTypesList)
	ON_LBN_SELCHANGE(IDC_TYPES_LIST, OnSelChangeTypesList)
	ON_BN_CLICKED(IDC_APPLY_BUTTON1, OnApplyButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmiQueryParametersForm diagnostics

#ifdef _DEBUG
void CTmiQueryParametersForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CTmiQueryParametersForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CTmiQueryParametersForm::SetupControls()
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
	}
	m_PeriodSpin.SetRange(1, 1440);
	m_DevicesTabCtrl.SetCurSel(0);
	OnSelChangeDevicesTab(NULL, NULL);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTmiQueryParametersForm message handlers


void CTmiQueryParametersForm::OnSelChangeDevicesTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_DeviceID = 0;
	long TmiType = 0;
	CString Description;
	int index = m_DevicesTabCtrl.GetCurSel();
	if (index != -1)
	{
		TC_ITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = TCIF_PARAM;
		m_DevicesTabCtrl.GetItem(index, &item);
		m_DeviceID = item.lParam;
		BOOL bEnabled;
		long Period;
		m_TmiTypesListBox.ResetContent();
		m_pDoc->QueryTmiAvailableParams(m_DeviceID);
		while (m_pDoc->GetNextTmiAvailableParam(Description, TmiType))
		{
			int index = m_TmiTypesListBox.AddString(Description);
			m_TmiTypesListBox.SetItemData(index, TmiType);
			m_pDoc->QueryTmiParam(m_DeviceID, TmiType, bEnabled, Period);
			m_TmiTypesListBox.SetCheck(index, bEnabled);
		}
	}
	m_TmiTypesListBox.SetCurSel(0);
	OnSelChangeTypesList();

	if (pResult)
		*pResult = 0;
}

void CTmiQueryParametersForm::OnSelChangeTypesList() 
{
	int index = m_TmiTypesListBox.GetCurSel();
	if (index == LB_ERR)
		return;

	long TmiType = m_TmiTypesListBox.GetItemData(index);
	BOOL bTmiEnabled, bCtrlsEnabled = FALSE;
	m_Period = 1;
	
	// find parameter by DeviceID and TmiType
	bCtrlsEnabled =  m_pDoc->QueryTmiParam(m_DeviceID, TmiType, bTmiEnabled, m_Period);
	bCtrlsEnabled &= bTmiEnabled;

	m_PeriodSpin.EnableWindow(bCtrlsEnabled);
	m_PeriodEdit.EnableWindow(bCtrlsEnabled);
	GetDescendantWindow(IDC_PERIOD_STATIC)->EnableWindow(bCtrlsEnabled);
	GetDescendantWindow(IDC_MINUTES_STATIC)->EnableWindow(bCtrlsEnabled);
	GetDescendantWindow(IDC_APPLY_BUTTON1)->EnableWindow(bCtrlsEnabled);
	UpdateData(FALSE);
}

void CTmiQueryParametersForm::OnCheckChangeTypesList() 
{
	OnApplyButton();
	OnSelChangeTypesList();
}

void CTmiQueryParametersForm::OnApplyButton() 
{
	int index = m_TmiTypesListBox.GetCurSel();
	if (index == LB_ERR)
		return;
	long TmiType = m_TmiTypesListBox.GetItemData(index);
	
	UpdateData();
	BOOL bTmiParameterEnabled = m_TmiTypesListBox.GetCheck(index);
	m_pDoc->EditTmiParam(m_DeviceID, TmiType, bTmiParameterEnabled, m_Period);
}
