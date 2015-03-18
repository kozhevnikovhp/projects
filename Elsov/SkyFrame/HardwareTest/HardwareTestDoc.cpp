// HardwareTestDoc.cpp : implementation of the CHardwareTestDoc class
//

#include "stdafx.h"

#include "HardwareTestDoc.h"
#include "HardwareTestView.h"
#include "ModemSdmsBatchTests.h"
#include "StelaMBatchTests.h"
#include "TranceiverBatchTests.h"
#include "ModulatorCarrierAutoTestSheet.h"
#include "DeviceFromName.h"
#include "ProgressDlg.h"

// Devices
#include "HumanControlled.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char *pszSaveConfigFileName = "C:\\Devices.cfg";

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestDoc

IMPLEMENT_DYNCREATE(CHardwareTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CHardwareTestDoc, CDocument)
	//{{AFX_MSG_MAP(CHardwareTestDoc)
	ON_COMMAND(ID_OPTIONS_LOGFILE, OnOptionsLogfile)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_LOGFILE, OnUpdateOptionsLogfile)
	ON_COMMAND(ID_DEVICE_CARRIERCAPTUREAUTOTEST, OnDeviceCarrierCaptureAutoTest)
	ON_COMMAND(ID_DEVICE_INTERVALTEST, OnDeviceIntervalTest)
	ON_COMMAND(ID_DEVICE_MODULATOR_CARRIER_TEST, OnDeviceModulatorCarrierTest)
	ON_COMMAND(ID_DEVICE_DELETE, OnDeviceDelete)
	ON_COMMAND(ID_DEVICE_ADD_MODEM_DATUM_PSM4900_70MHZ, OnDeviceAddModemDatumPsm4900_70MHz)
	ON_COMMAND(ID_DEVICE_ADD_MODEM_DATUM_PSM4900_140MHZ, OnDeviceAddModemDatumPsm4900140MHz)
	ON_COMMAND(ID_DEVICE_ADD_MODEM_DATUM_PSM4900_HYBRID70MHZLBAND, OnDeviceAddModemDatumPsm4900_Hybrid)
	ON_COMMAND(ID_DEVICE_ADD_ACCESSDEVICE_NSG_NPS3E, OnDeviceAddAccessDeviceNsgNps3e)
	ON_COMMAND(ID_DEVICE_ADD_ACCESSDEVICE_NSG_NX300, OnDeviceAddAccessDeviceNsgNx300)
	ON_COMMAND(ID_DEVICE_ADD_TRANCEIVER_WAVESAT_WTR70, OnDeviceAddTranceiverWaveSatWtr70)
	ON_COMMAND(ID_DEVICE_ADD_TRANCEIVER_ADVANTECH_WTY1, OnDeviceAddTranceiverAdvantechWty1)
	ON_COMMAND(ID_DEVICE_ADD_MODEM_DATUM_PSM4900_LBAND, OnDeviceAddModemDatumPsm4900_Lband)
	ON_COMMAND(ID_TESTS_MODEM_TESTS, OnTestsModemTest)
	ON_COMMAND(ID_TESTS_STELAM_TESTS, OnTestsStelaMTests)
	ON_COMMAND(ID_DEVICE_ADD_TRANCEIVER_WAVESAT_WAVESATSUBSTITUTOR, OnDeviceAddTranceiverWavesatSubstitutor)
	ON_COMMAND(ID_DEVICE_ADD_POWERMETER_AGILENT_E4418B, OnDeviceAddPowerMeterAgilentE4418B)
	ON_COMMAND(ID_TESTS_MODEMSDMSTESTS, OnTestsModemSdmsTests)
	ON_COMMAND(ID_TESTS_SDMSTESTS, OnTestsSdmsTests)
	ON_COMMAND(ID_DEVICE_ADD_UPDOWNCONVERTER_UPCONVERTERKVV140_VER2002, OnDeviceAddUpConverterKvv140Ver2002)
	ON_COMMAND(ID_DEVICE_ADD_UPDOWNCONVERTER_UPCONVERTERKVV140_VER2004, OnDeviceAddUpConverterKvv140Ver2004)
	ON_COMMAND(ID_DEVICE_ADD_UPDOWNCONVERTER_DOWNCONVERTERKVN140_VER2002, OnDeviceAddrDownConverterKvn140Ver2002)
	ON_COMMAND(ID_DEVICE_ADD_UPDOWNCONVERTER_DOWNCONVERTERKVN140_VER2004, OnDeviceAddDownConverterKvn140Ver2004)
	ON_COMMAND(ID_DEVICE_ADD_FREQUENCYCOUNTER_AGILENT_53150F, OnDeviceAddFrequencyCounterAgilent53150A)
	ON_COMMAND(ID_TESTS_TRANCEIVER_TESTS, OnTestsTranceiverTests)
	ON_COMMAND(ID_DEVICE_ADD_TRANCEIVER_VERTEX_50WATT, OnDeviceAddTranceiverVertex50watt)
	ON_COMMAND(ID_DEVICE_ADD_TRANCEIVER_VERTEX_VERTEX_SUBSTITUTOR, OnDeviceAddTranceiverVertexSubstitutor)
	ON_COMMAND(ID_DEVICE_ADD_MODEM_DATUM_PSM500_LBAND, OnDeviceAddModemDatumPSM500Lband)
	ON_COMMAND(ID_DEVICE_ADD_UPDOWNCONVERTER_DOWNCONVERTERKVN140_VER2011, OnDeviceAddDownConverterKvn140Ver2011)
	ON_COMMAND(ID_DEVICE_ADD_UPDOWNCONVERTER_UPCONVERTERKVV140_VER2011, OnDeviceAddUpConverterKvv140Ver2011)
	ON_COMMAND(ID_DEVICE_ADD_SDMS_SDMSVSAT_4900L, OnDeviceAddSdmsSdmsVsat4900l)
	ON_COMMAND(ID_DEVICE_ADD_SDMS_SDMSVSAT_M500L, OnDeviceAddSdmsSdmsVsatM500l)
	ON_COMMAND(ID_DEVICE_ADD_MODEM_DATUM_M7_LMOD_LDEM, OnDeviceAddModemDatumM7LModLDem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestDoc construction/destruction

static char *pszOptionsSection="Options";
static char *pszWriteLogEntry="WriteLogFile";
static char *pszLogFileName="LogFileName";
static char *pszLogFileDir="LogFileDir";

CHardwareTestDoc::CHardwareTestDoc()
{
	m_bAbstractFormUpdateTimerEnabled = TRUE;
	m_pTreeView=NULL;
	ZeroMemory(m_pForms, sizeof(m_pForms));
	m_bWriteLog=AfxGetApp()->GetProfileInt(pszOptionsSection, pszWriteLogEntry, 0);
	m_LogFileName=AfxGetApp()->GetProfileString(pszOptionsSection, pszLogFileName, "log.txt");
	m_LogFileDir=AfxGetApp()->GetProfileString(pszOptionsSection, pszLogFileDir, "C:\\");
	m_pLogFile=NULL;
	if (m_bWriteLog)
		OpenLogFile();
	ClearDevices();
	SetModifiedFlag();
}

CHardwareTestDoc::~CHardwareTestDoc()
{
	AfxGetApp()->WriteProfileInt(pszOptionsSection, pszWriteLogEntry, m_bWriteLog);
	AfxGetApp()->WriteProfileString(pszOptionsSection, pszLogFileName, m_LogFileName);
	AfxGetApp()->WriteProfileString(pszOptionsSection, pszLogFileDir, m_LogFileDir);
	CloseLogFile();
	FreeDevices();
}

BOOL CHardwareTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CProgressDlg dlg;
	dlg.Create();

	m_pTreeView->Clear();
	ClearDevices();

	CStdioFile file;
	if (!file.Open(pszSaveConfigFileName, CFile::modeRead))
		return TRUE;
	char pszString[256];
	CString DeviceName;
	while (file.ReadString(DeviceName))
	{
		if (dlg.CheckCancelButton())
			break;
		dlg.SetStatus(DeviceName);

		CDevice *pDevice = DeviceFromName(DeviceName);
		AddDevice(pDevice);
		file.ReadString(pszString, sizeof(pszString));
		if (!pDevice)
			continue;
		MC_ControlType ControlType = MC_UNKNOWN_TYPE;
		char pszConnectionName[128];
		unsigned int bConnected = 0;
		unsigned int nMCAddress = 0;
		unsigned int Param1 = 0;
		sscanf(pszString, "%d %s %d %d %d", &ControlType, pszConnectionName, &bConnected, &nMCAddress, &Param1);
		pDevice->SetControlType(ControlType);
		pDevice->SetConnectionName((const char *)pszConnectionName);
		pDevice->SetMCBusAddress(nMCAddress);
		CString status = "Connecting to ";
		status += DeviceName;
		status += " (";
		status += pszConnectionName;
		status += ")";
		dlg.SetStatus(status);

		if (dlg.CheckCancelButton())
			break;
		if (bConnected)
			pDevice->Connect();
		if (pDevice->IsDeviceSubstitutor())
		{
			CDeviceSubstitutor *pSubstitutor = (CDeviceSubstitutor *)pDevice;
			if (Param1)
				pSubstitutor->Start();
			else
				pSubstitutor->Stop();
		}
	}
	AddDevice(new CHumanPowerMeter());
	AddDevice(new CHumanFrequencyCounter());

	return TRUE;
}

BOOL CHardwareTestDoc::SaveModified() 
{
	CStdioFile file;
	if (file.Open(pszSaveConfigFileName, CFile::modeCreate | CFile::modeWrite))
	{
		char pszString[256];
		for (int i = 0; i<MAX_DEVICES; i++)
		{
			unsigned int Param1 = 0;
			CDevice *pDevice = m_pDevices[i];
			if (pDevice && !pDevice->IsHuman())
			{
				if (pDevice->IsDeviceSubstitutor())
				{
					CDeviceSubstitutor *pSubstitutor = (CDeviceSubstitutor *)pDevice;
					if (pSubstitutor->IsStarted())
						Param1 = 1;
				}
				sprintf(pszString, "%s\n", pDevice->GetName());
				file.Write(pszString, strlen(pszString));
				sprintf(pszString, "%d %s %d %d %d\n",
					pDevice->GetControlType(),
					pDevice->GetConnectionName(),
					pDevice->IsControllable(),
					pDevice->GetMCBusAddress(),
					Param1);
				file.Write(pszString, strlen(pszString));
			}
		}
	}
	
	return CDocument::SaveModified();
}

void CHardwareTestDoc::ClearDevices()
{
	ZeroMemory(m_pDevices, sizeof(m_pDevices));
	m_DeviceCount=0;
}

void CHardwareTestDoc::FreeDevice(int index)
{
	ASSERT(index<MAX_DEVICES);
	ASSERT(index>=0);
	CDevice *&pDevice=m_pDevices[index];
	if (m_pDevices[index])
	{
		delete pDevice;
		pDevice=NULL;
		m_DeviceCount--;
	}
}

void CHardwareTestDoc::FreeDevices()
{
	for (int index=0; index<MAX_DEVICES; index++)
		FreeDevice(index);
}

int CHardwareTestDoc::FindPlaceForDevice()
{
	for (int index=0; index<MAX_DEVICES; index++)
		if (!m_pDevices[index]) return index;
	return -1; // could not find empty place for new device
}

int CHardwareTestDoc::AddDevice(CDevice *pDevice)
{
	if (!pDevice)
		return -1;
	int index=FindPlaceForDevice();
	if (index==-1)
		return -1;  // could not find empty place for new device
	m_pDevices[index]=pDevice;
	m_DeviceCount++;

	if (!strcmp(pDevice->GetName(), CDatumPsm4900_70MHz::m_pszName) ||
		!strcmp(pDevice->GetName(), CDatumPsm4900_140MHz::m_pszName) ||
		!strcmp(pDevice->GetName(), CDatumPsm4900_Hybrid::m_pszName) ||
		!strcmp(pDevice->GetName(), CDatumPsm4900_LBand::m_pszName) ||
		!strcmp(pDevice->GetName(), CDatumPsm500_LBand::m_pszName) ||
		!strcmp(pDevice->GetName(), CDatum_M7_LMod_LDem::m_pszName))
		m_pTreeView->AddModem(pDevice);

	if (!strcmp(pDevice->GetName(), CRadyneDMD2401::m_pszName))
		m_pTreeView->AddModem(pDevice);

	if (!strcmp(pDevice->GetName(), CSdmsVsat4900L::m_pszName) ||
		!strcmp(pDevice->GetName(), CSdmsVsatM500L::m_pszName))
		m_pTreeView->AddStation(pDevice);

	if (!strcmp(pDevice->GetName(), CNps3e::m_pszName) ||
		!strcmp(pDevice->GetName(), CNx300::m_pszName))
		m_pTreeView->AddNsgFrad(pDevice);

	if (!strcmp(pDevice->GetName(), CWaveSatWtr70::m_pszName) ||
		!strcmp(pDevice->GetName(), CAdvantechWty1::m_pszName) ||
		!strcmp(pDevice->GetName(), CVertex50W::m_pszName))
		m_pTreeView->AddTranceiver(pDevice);

	if (!strcmp(pDevice->GetName(), CPozharskyDownConverterKvn140Ver2002::m_pszName) ||
		!strcmp(pDevice->GetName(), CPozharskyDownConverterKvn140Ver2004::m_pszName) ||
		!strcmp(pDevice->GetName(), CPozharskyDownConverterKvn140Ver2011::m_pszName) ||
		!strcmp(pDevice->GetName(), CPozharskyUpConverterKvv140Ver2002::m_pszName) ||
		!strcmp(pDevice->GetName(), CPozharskyUpConverterKvv140Ver2004::m_pszName) ||
		!strcmp(pDevice->GetName(), CPozharskyUpConverterKvv140Ver2011::m_pszName))
		m_pTreeView->AddUpDownConverter(pDevice);

	if (!strcmp(pDevice->GetName(), CWaveSatSubstitutor::m_pszName) ||
		!strcmp(pDevice->GetName(), CVertexSubstitutor::m_pszName))
		m_pTreeView->AddDeviceSubstitutor(pDevice);

	if (!strcmp(pDevice->GetName(), CAgilentE4418B::m_pszName))
		m_pTreeView->AddPowerMeter(pDevice);

	if (!strcmp(pDevice->GetName(), CAgilent53150A::m_pszName))
		m_pTreeView->AddFrequencyCounter(pDevice);

	m_pTreeView->Refresh();
	return index;
}

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestDoc serialization

void CHardwareTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestDoc diagnostics

#ifdef _DEBUG
void CHardwareTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHardwareTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CHardwareTestDoc::AddNewForm(FI_FormIndex IndexInViewArray, CView *pViewToAdd, CWnd *pParent)
{
	m_pForms[IndexInViewArray]=(CAbstractForm *)pViewToAdd;
	m_pForms[IndexInViewArray]->SetDocument(this);
	pViewToAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CFrameWnd::rectDefault, pParent,
		AFX_IDW_PANE_FIRST+IndexInViewArray+1, NULL);
	AddView(pViewToAdd);
}

/////////////////////////////////////////////////////////////////////////////
// CHardwareTestDoc commands

void CHardwareTestDoc::OnOptionsLogfile() 
{
	m_bWriteLog=1-m_bWriteLog;
	if (m_bWriteLog)
	{
		CFileDialog fd(FALSE, _T("txt"), m_LogFileName, OFN_HIDEREADONLY,
		_T("Текстовые файлы (*txt)|*txt|Все файлы (*.*)|*.*||"));
		fd.m_ofn.lpstrInitialDir=m_LogFileDir;
		if (fd.DoModal()==IDCANCEL)
		{
			m_bWriteLog=1-m_bWriteLog;
			return;
		}
		CString LogFilePath=fd.GetPathName();
		m_LogFileName=fd.GetFileName();
		m_LogFileDir=LogFilePath.Left(LogFilePath.GetLength()-m_LogFileName.GetLength());
		OpenLogFile();
	}
	else
		CloseLogFile();
}

void CHardwareTestDoc::OnUpdateOptionsLogfile(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bWriteLog);	
}

void CHardwareTestDoc::OpenLogFile()
{
	m_pLogFile=fopen(m_LogFileDir+m_LogFileName, "a+");
	CDevice::SetLogFile(m_pLogFile);
	char *pszHeader="******************** New session ******************\n\n";
	fwrite(pszHeader, strlen(pszHeader), 1, m_pLogFile);
}

void CHardwareTestDoc::CloseLogFile()
{
	if (!m_pLogFile)
		return;
	char *pszHeader="\n****************** End of session ******************\n\n";
	fwrite(pszHeader, strlen(pszHeader), 1, m_pLogFile);
	fclose(m_pLogFile);
	m_pLogFile=NULL;
	CDevice::SetLogFile(m_pLogFile);
}


void CHardwareTestDoc::OnDeviceCarrierCaptureAutoTest() 
{
}


void CHardwareTestDoc::OnDeviceIntervalTest() 
{
}

void CHardwareTestDoc::OnDeviceModulatorCarrierTest() 
{
}


void CHardwareTestDoc::OnDeviceDelete() 
{
	CDevice *pDevice = m_pTreeView->DeleteSelectedDevice();
	if (!pDevice)
		return;
	
	for (int index =0 ; index < MAX_DEVICES; index++)
		if (m_pDevices[index] == pDevice)
			m_pDevices[index] = NULL;
	delete pDevice;
}

void CHardwareTestDoc::OnDeviceAddModemDatumPsm4900_70MHz() 
{
	AddDevice(new CDatumPsm4900_70MHz());
}

void CHardwareTestDoc::OnDeviceAddModemDatumPsm4900140MHz() 
{
	AddDevice(new CDatumPsm4900_140MHz());	
}

void CHardwareTestDoc::OnDeviceAddModemDatumPsm4900_Hybrid() 
{
	AddDevice(new CDatumPsm4900_Hybrid());	
}

void CHardwareTestDoc::OnDeviceAddModemDatumPsm4900_Lband() 
{
	AddDevice(new CDatumPsm4900_LBand());	
}

void CHardwareTestDoc::OnDeviceAddModemDatumPSM500Lband() 
{
	AddDevice(new CDatumPsm500_LBand());
}

void CHardwareTestDoc::OnDeviceAddModemDatumM7LModLDem() 
{
	AddDevice(new CDatum_M7_LMod_LDem());	
}

void CHardwareTestDoc::OnDeviceAddAccessDeviceNsgNps3e() 
{
	AddDevice(new CNps3e());	
}

void CHardwareTestDoc::OnDeviceAddAccessDeviceNsgNx300() 
{
	AddDevice(new CNx300());
}

void CHardwareTestDoc::OnDeviceAddTranceiverWaveSatWtr70() 
{
	AddDevice(new CWaveSatWtr70());	
}

void CHardwareTestDoc::OnDeviceAddTranceiverWavesatSubstitutor() 
{
	AddDevice(new CWaveSatSubstitutor());	
}

void CHardwareTestDoc::OnDeviceAddTranceiverAdvantechWty1() 
{
	AddDevice(new CAdvantechWty1());	
}

void CHardwareTestDoc::OnDeviceAddSdmsSdmsVsat4900l() 
{
	AddDevice(new CSdmsVsat4900L());
}

void CHardwareTestDoc::OnDeviceAddSdmsSdmsVsatM500l() 
{
	AddDevice(new CSdmsVsatM500L());
}

void CHardwareTestDoc::OnTestsModemTest() 
{
	m_bAbstractFormUpdateTimerEnabled = FALSE;
	CModemBatchTestsPropertySheet propSheet(&m_pDevices[0], m_DeviceCount);
	propSheet.m_AutoTestConfig.m_nNumberOfTransmitters=1;
	propSheet.m_AutoTestConfig.m_nNumberOfReceivers=1;

	propSheet.DoModal();
	
	m_pTreeView->RefreshCurrentForm();
	m_bAbstractFormUpdateTimerEnabled = TRUE;
}

void CHardwareTestDoc::OnTestsSdmsTests() 
{
	m_bAbstractFormUpdateTimerEnabled = FALSE;
	CSdmsBatchTestsPropertySheet propSheet(&m_pDevices[0], m_DeviceCount);
	propSheet.m_AutoTestConfig.m_nNumberOfTransmitters=1;
	propSheet.m_AutoTestConfig.m_nNumberOfReceivers=1;

	propSheet.DoModal();
	
	m_pTreeView->RefreshCurrentForm();
	m_bAbstractFormUpdateTimerEnabled = TRUE;
}

void CHardwareTestDoc::OnTestsModemSdmsTests() 
{
	m_bAbstractFormUpdateTimerEnabled = FALSE;
	CModemSdmsBatchTestsPropertySheet propSheet(&m_pDevices[0], m_DeviceCount);
	propSheet.m_AutoTestConfig.m_nNumberOfTransmitters=1;
	propSheet.m_AutoTestConfig.m_nNumberOfReceivers=1;

	propSheet.DoModal();
	
	m_pTreeView->RefreshCurrentForm();
	m_bAbstractFormUpdateTimerEnabled = TRUE;
}

void CHardwareTestDoc::OnTestsStelaMTests() 
{
	m_bAbstractFormUpdateTimerEnabled = FALSE;
	CVppuBatchTestsPropertySheet propSheet(&m_pDevices[0], m_DeviceCount);
	propSheet.m_AutoTestConfig.m_nNumberOfTransmitters=1;
	propSheet.m_AutoTestConfig.m_nNumberOfReceivers=1;

	propSheet.DoModal();
	m_pTreeView->RefreshCurrentForm();
	m_bAbstractFormUpdateTimerEnabled = TRUE;
}

void CHardwareTestDoc::OnTestsTranceiverTests() 
{
	m_bAbstractFormUpdateTimerEnabled = FALSE;
	CTranceiverBatchTestsPropertySheet propSheet(&m_pDevices[0], m_DeviceCount);
	propSheet.m_AutoTestConfig.m_nNumberOfTransmitters=1;
	propSheet.m_AutoTestConfig.m_nNumberOfReceivers=1;

	propSheet.DoModal();
	m_pTreeView->RefreshCurrentForm();
	m_bAbstractFormUpdateTimerEnabled = TRUE;
}

void CHardwareTestDoc::OnDeviceAddPowerMeterAgilentE4418B() 
{
	AddDevice(new CAgilentE4418B);	
}

void CHardwareTestDoc::OnDeviceAddUpConverterKvv140Ver2002() 
{
	AddDevice(new CPozharskyUpConverterKvv140Ver2002);
}

void CHardwareTestDoc::OnDeviceAddUpConverterKvv140Ver2004() 
{
	AddDevice(new CPozharskyUpConverterKvv140Ver2004);
}

void CHardwareTestDoc::OnDeviceAddrDownConverterKvn140Ver2002() 
{
	AddDevice(new CPozharskyDownConverterKvn140Ver2002);
}

void CHardwareTestDoc::OnDeviceAddDownConverterKvn140Ver2004() 
{
	AddDevice(new CPozharskyDownConverterKvn140Ver2004);
}

void CHardwareTestDoc::OnDeviceAddFrequencyCounterAgilent53150A() 
{
	AddDevice(new CAgilent53150A);
}


void CHardwareTestDoc::OnDeviceAddTranceiverVertex50watt() 
{
	AddDevice(new CVertex50W);
}

void CHardwareTestDoc::OnDeviceAddTranceiverVertexSubstitutor() 
{
	AddDevice(new CVertexSubstitutor());	
}

void CHardwareTestDoc::OnDeviceAddDownConverterKvn140Ver2011() 
{
	AddDevice(new CPozharskyDownConverterKvn140Ver2011);	
}

void CHardwareTestDoc::OnDeviceAddUpConverterKvv140Ver2011() 
{
	AddDevice(new CPozharskyUpConverterKvv140Ver2011);	
}

