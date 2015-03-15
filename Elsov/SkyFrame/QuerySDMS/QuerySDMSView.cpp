// QuerySDMSView.cpp
//

#include "stdafx.h"
#include "QuerySDMS.h"

#include "QuerySDMSDoc.h"
#include "QuerySDMSView.h"
#include "../Common/SDMS_OIDs.h"
#include "Dialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char *pszPublicCommunity = "public";
const char *pszPrivateCommunity = "private";
const char *pszUnknown = "Unknown";
const char *pszNormal = "Нормальный";
const char *pszInverted = "Инвертированный";
const char *pszBPSK = "BPSK";
const char *pszQPSK = "QPSK";
const char *pszOn = "Вкл.";
const char *pszOff = "Выкл.";
const char *psz12 = "1/2";
const char *psz34 = "3/4";
const char *psz78 = "7/8";
const char *pszDisable = "Выкл";
const char *pszIess308 = "IESS-308";
const char *pszIess309 = "IESS-309";
const char *pszV35 = "V.35";
const char *pszAltV35 = "Alt. V.35";
const char *pszIntelsat = "Intelsat";
const char *pszAltIntelsat = "Alt. Intelsat";
const char *pszTpcSync = "tpcSync";
const char *pszInternal = "Internal";
const char *pszExternal = "External";
const char *pszRcvClock = "RCV Clock";

/////////////////////////////////////////////////////////////////////////////
// CControlForm

UINT CControlForm::g_DeltaTime = 20;
CSnmpSocket CControlForm::g_Socket;
IPADDRESS_TYPE CControlForm::g_IpAddress = 0;
int CControlForm::g_ReqID = 0;
CString CControlForm::g_ReplyTime = pszUnknown;

// ModIfTable
unsigned int CControlForm::g_ModIfFrequency = 0;
int CControlForm::g_ModIfOffset = 0;
double CControlForm::g_ModIfOutputLevel = 0;
CString CControlForm::g_ModIfOutputEnabled = pszUnknown;
CString CControlForm::g_ModIfModulation = pszUnknown;
CString CControlForm::g_ModIfSpectrum = pszUnknown;

// ModDataTable
unsigned int CControlForm::g_ModDataBitRate = 0;
CString CControlForm::g_ModDataCodeRate = pszUnknown;
CString CControlForm::g_ModDataDiff = pszUnknown;
CString CControlForm::g_ModDataScrambler = pszUnknown;
CString CControlForm::g_ModDataClockSource = pszUnknown;

// ModBucTable
double CControlForm::g_ModBucVoltage = 0;
double CControlForm::g_ModBucCurrent = 0;
CString CControlForm::g_ModBucPower = pszUnknown;
CString CControlForm::g_ModBuc10MHz = pszUnknown;

// DemodIfTable
unsigned int CControlForm::g_DemodIfFrequency = 0;
unsigned int CControlForm::g_DemodIfSweepRange = 0;
CString CControlForm::g_DemodIfSweepMode = pszUnknown;
CString CControlForm::g_DemodIfModulation = pszUnknown;
CString CControlForm::g_DemodIfSpectrum = pszUnknown;

// DemodDataTable
unsigned int CControlForm::g_DemodDataBitRate = 0;
CString CControlForm::g_DemodDataCodeRate = pszUnknown;
CString CControlForm::g_DemodDataDiff = pszUnknown;
CString CControlForm::g_DemodDataDescrambler = pszUnknown;
CString CControlForm::g_DemodDataClockSource = pszUnknown;

// DemodLnbTable
double CControlForm::g_DemodLnbVoltage = 0;
double CControlForm::g_DemodLnbCurrent = 0;
CString CControlForm::g_DemodLnbPower = pszUnknown;
CString CControlForm::g_DemodLnbMHz = pszUnknown;

// DemodStatus
int CControlForm::g_DemodStatusOffset = 0;
double CControlForm::g_DemodStatusLevel = 0;
double CControlForm::g_DemodStatusEbNo = 0;
double CControlForm::g_DemodStatusBER = 0;
double CControlForm::g_DemodStatusSER = 0;
CString CControlForm::g_DemodStatusLock = pszUnknown;
int CControlForm::g_DemodStatusBuffer = 0;

IMPLEMENT_DYNCREATE(CControlForm, CFormView)

CControlForm::CControlForm() : CFormView(1)
{
}

CControlForm::CControlForm(UINT nIDTemplate) : CFormView(nIDTemplate)
{
}

BEGIN_MESSAGE_MAP(CControlForm, CFormView)
	//{{AFX_MSG_MAP(CControlForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LOGICAL CControlForm::IsAdministrator()
{
	CQuerySDMSApp *pApp = (CQuerySDMSApp *)AfxGetApp();
	return pApp->IsWriteEnabled();
}

LOGICAL CControlForm::SendGetRequest(unsigned int *puiOID, int nOidLen)
{
	LOGICAL b = g_Socket.SendSnmpGetRequest(g_IpAddress, pszPublicCommunity, puiOID, nOidLen, ++g_ReqID);
	Sleep(0);
	return b;
}

LOGICAL CControlForm::SendSetUINTRequest(unsigned int *puiOID, int nOidLen, unsigned int uiValue)
{
	LOGICAL b = g_Socket.SendSnmpSetUnsignedIntegerRequest(g_IpAddress, pszPrivateCommunity, puiOID, nOidLen, ++g_ReqID, uiValue);
	Sleep(0);
	return b;
}

LOGICAL CControlForm::SendSetIntRequest(unsigned int *puiOID, int nOidLen, int iValue)
{
	LOGICAL b = g_Socket.SendSnmpSetIntegerRequest(g_IpAddress, pszPrivateCommunity, puiOID, nOidLen, ++g_ReqID, iValue);
	Sleep(0);
	return b;
}

void CControlForm::OnIdle()
{
	SSnmpReply reply;
	BOOL bSomethingChanged = FALSE;
	while(g_Socket.GetSnmpReply(&reply))
	{
		if (reply.m_StatusCode != 0 || reply.m_ErrorCode != 0)
			continue;
		if (reply.IsTheSameOID(OidModulatorIfFrequency, OidModulatorIfFrequencyLen))
		{
			g_ModIfFrequency = reply.m_uiIntegerValue/1000;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorIfOffset, OidModulatorIfOffsetLen))
		{
			g_ModIfOffset = reply.m_iIntegerValue;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen))
		{
			g_ModIfOutputLevel = reply.m_iIntegerValue/10.;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_ModIfOutputEnabled = pszOff;
			else
				g_ModIfOutputEnabled = pszOn;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorIfModulation, OidModulatorIfModulationLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_ModIfModulation = pszBPSK;
			else
				g_ModIfModulation = pszQPSK;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_ModIfSpectrum = pszNormal;
			else
				g_ModIfSpectrum = pszInverted;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorIfFrequency, OidDemodulatorIfFrequencyLen))
		{
			g_DemodIfFrequency = reply.m_uiIntegerValue/1000;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorIfSweepRange, OidDemodulatorIfSweepRangeLen))
		{
			g_DemodIfSweepRange = reply.m_uiIntegerValue;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorIfSweepMode, OidDemodulatorIfSweepModeLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_DemodIfSweepMode = "Fast";
			else
				g_DemodIfSweepMode = "Search";
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_DemodIfModulation = pszBPSK;
			else
				g_DemodIfModulation = pszQPSK;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_DemodIfSpectrum = pszNormal;
			else
				g_DemodIfSpectrum = pszInverted;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorStatusOffset, OidDemodulatorStatusOffsetLen))
		{
			g_DemodStatusOffset = reply.m_iIntegerValue;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorStatusInputLevel, OidDemodulatorStatusInputLevelLen))
		{
			g_DemodStatusLevel = reply.m_iIntegerValue;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorStatusEbNo, OidDemodulatorStatusEbNoLen))
		{
			g_DemodStatusEbNo = reply.m_iIntegerValue/10.;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorStatusBER, OidDemodulatorStatusBERLen))
		{
			sscanf(reply.m_OctetStringValue, "%lf", &g_DemodStatusBER);
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorStatusSER, OidDemodulatorStatusSERLen))
		{
			sscanf(reply.m_OctetStringValue, "%lf", &g_DemodStatusSER);
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorStatusBuffer, OidDemodulatorStatusBufferLen))
		{
			g_DemodStatusBuffer = reply.m_iIntegerValue;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorStatusLock, OidDemodulatorStatusLockLen))
		{
			if (reply.m_iIntegerValue == 2)
				g_DemodStatusLock = "Захват";
			else
				g_DemodStatusLock = "Нет захвата";
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorDataBitRate, OidModulatorDataBitRateLen))
		{
			g_ModDataBitRate = reply.m_uiIntegerValue;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorDataBitRate, OidDemodulatorDataBitRateLen))
		{
			g_DemodDataBitRate = reply.m_uiIntegerValue;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorDataCodeRate, OidModulatorDataCodeRateLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_ModDataCodeRate = psz12;
			else if (reply.m_iIntegerValue == 2)
				g_ModDataCodeRate = psz34;
			else if (reply.m_iIntegerValue == 3)
				g_ModDataCodeRate = psz78;
			else
				g_ModDataCodeRate = pszUnknown;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorDataCodeRate, OidDemodulatorDataCodeRateLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_DemodDataCodeRate = psz12;
			else if (reply.m_iIntegerValue == 2)
				g_DemodDataCodeRate = psz34;
			else if (reply.m_iIntegerValue == 3)
				g_DemodDataCodeRate = psz78;
			else
				g_DemodDataCodeRate = pszUnknown;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_ModDataDiff = pszOff;
			else if (reply.m_iIntegerValue == 2)
				g_ModDataDiff = pszOn;
			else if(reply.m_iIntegerValue == 3)
				g_ModDataDiff = "Diff. coherent";
			else
				g_ModDataDiff = pszUnknown;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_DemodDataDiff = pszOff;
			else if (reply.m_iIntegerValue == 2)
				g_DemodDataDiff = pszOn;
			else
				g_DemodDataDiff = pszUnknown;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorDataScrambler, OidModulatorDataScramblerLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_ModDataScrambler = pszDisable;
			else if (reply.m_iIntegerValue == 2)
				g_ModDataScrambler = pszIess308;
			else if (reply.m_iIntegerValue == 3)
				g_ModDataScrambler = pszIess309;
			else if (reply.m_iIntegerValue == 4)
				g_ModDataScrambler = pszV35;
			else if (reply.m_iIntegerValue == 5)
				g_ModDataScrambler = pszAltV35;
			else if (reply.m_iIntegerValue == 6)
				g_ModDataScrambler = pszIntelsat;
			else if (reply.m_iIntegerValue == 7)
				g_ModDataScrambler = pszAltIntelsat;
			else if (reply.m_iIntegerValue == 8)
				g_ModDataScrambler = pszTpcSync;
			else
				g_ModDataDiff = pszUnknown;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorDataDescrambler, OidDemodulatorDataDescramblerLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_DemodDataDescrambler = pszDisable;
			else if (reply.m_iIntegerValue == 2)
				g_DemodDataDescrambler = pszIess308;
			else if (reply.m_iIntegerValue == 3)
				g_DemodDataDescrambler = pszIess309;
			else if (reply.m_iIntegerValue == 4)
				g_DemodDataDescrambler = pszV35;
			else if (reply.m_iIntegerValue == 5)
				g_DemodDataDescrambler = pszAltV35;
			else if (reply.m_iIntegerValue == 6)
				g_DemodDataDescrambler = pszIntelsat;
			else if (reply.m_iIntegerValue == 7)
				g_DemodDataDescrambler = pszAltIntelsat;
			else if (reply.m_iIntegerValue == 8)
				g_DemodDataDescrambler = pszTpcSync;
			else
				g_ModDataDiff = pszUnknown;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorDataClockSrc, OidModulatorDataClockSrcLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_ModDataClockSource = pszInternal;
			else if (reply.m_iIntegerValue == 2)
				g_ModDataClockSource = "TT Clock";
			else if(reply.m_iIntegerValue == 3)
				g_ModDataClockSource = pszExternal;
			else if(reply.m_iIntegerValue == 4)
				g_ModDataClockSource = pszRcvClock;
			else
				g_ModDataClockSource = pszUnknown;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidDemodulatorDataClockSrc, OidDemodulatorDataClockSrcLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_DemodDataClockSource = pszRcvClock;
			else if (reply.m_iIntegerValue == 2)
				g_DemodDataClockSource = pszInternal;
			else if(reply.m_iIntegerValue == 3)
				g_DemodDataClockSource = pszExternal;
			else if(reply.m_iIntegerValue == 4)
				g_DemodDataClockSource = "Mod Clock";
			else
				g_DemodDataClockSource = pszUnknown;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorBucVoltageOut, OidModulatorBucVoltageOutLen))
		{
			g_ModBucVoltage = reply.m_iIntegerValue/10;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorBucCurrentOut, OidModulatorBucCurrentOutLen))
		{
			g_ModBucCurrent = reply.m_iIntegerValue/10;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorBucPower, OidModulatorBucPowerLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_ModBucPower = pszOff;
			else if (reply.m_iIntegerValue == 2)
				g_ModBucPower = pszOn;
			else
				g_ModBucPower = pszUnknown;
			bSomethingChanged = TRUE;
		}
		else if (reply.IsTheSameOID(OidModulatorBuc10MHz, OidModulatorBuc10MHzLen))
		{
			if (reply.m_iIntegerValue == 1)
				g_ModBuc10MHz = pszOff;
			else if (reply.m_iIntegerValue == 2)
				g_ModBuc10MHz = pszOn;
			else
				g_ModBuc10MHz = pszUnknown;
			bSomethingChanged = TRUE;
		}
	}
	if (bSomethingChanged)
	{
		CTime time = CTime::GetCurrentTime();
		CString ReplyTime; ReplyTime = time.Format("%d.%m.%Y %H:%M:%S");
		UpdateControls(ReplyTime);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParamsForm

IMPLEMENT_DYNCREATE(CParamsForm, CControlForm)

BEGIN_MESSAGE_MAP(CParamsForm, CControlForm)
	//{{AFX_MSG_MAP(CParamsForm)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CParamsForm::CParamsForm() : CControlForm(IDD_PARAMS_FORM)
{
	//{{AFX_DATA_INIT(CParamsForm)
	m_DeltaTime = 0;
	//}}AFX_DATA_INIT
}

CParamsForm::~CParamsForm()
{
}

//virtual
void CParamsForm::Entering()
{
	m_DeltaTime = g_DeltaTime;
	UpdateData(FALSE);
}

//virtual
void CParamsForm::DoTimer()
{
}

void CParamsForm::DoDataExchange(CDataExchange *pDX)
{
	CControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParamsForm)
	DDX_Text(pDX, IDC_DELTA_TIME_EDIT, m_DeltaTime);
	//}}AFX_DATA_MAP
}

void CParamsForm::OnInitialUpdate()
{
	CControlForm::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	g_Socket.SetReadTimeout(10);
}

/////////////////////////////////////////////////////////////////////////////
// CParamsForm diagnostics

#ifdef _DEBUG
void CParamsForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CParamsForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CModIfTableForm

IMPLEMENT_DYNCREATE(CModIfTableForm, CControlForm)

BEGIN_MESSAGE_MAP(CModIfTableForm, CControlForm)
	//{{AFX_MSG_MAP(CModIfTableForm)
	ON_BN_CLICKED(IDC_MOD_FREQUENCY_BUTTON, OnModFrequencyButton)
	ON_BN_CLICKED(IDC_MOD_OFFSET_BUTTON, OnModOffsetButton)
	ON_BN_CLICKED(IDC_MOD_MODULATION_BUTTON, OnModModulationButton)
	ON_BN_CLICKED(IDC_MOD_LEVEL_BUTTON, OnModLevelButton)
	ON_BN_CLICKED(IDC_MOD_ONOFF_BUTTON, OnModOnoffButton)
	ON_BN_CLICKED(IDC_MOD_SPECTRUM_BUTTON, OnModSpectrumButton)
	ON_BN_CLICKED(IDC_QUERY_NOW_BUTTON, OnQueryNowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CModIfTableForm::CModIfTableForm() : CControlForm(IDD_MOD_IF_TABLE_FORM)
{
	//{{AFX_DATA_INIT(CModIfTableForm)
	m_Frequency = 0;
	m_Offset = 0;
	m_Level = 0.0;
	m_Modulation = _T("");
	m_OnOff = _T("");
	m_Spectrum = _T("");
	m_ReplyTime = _T("");
	//}}AFX_DATA_INIT
}

CModIfTableForm::~CModIfTableForm()
{
}

//virtual
void CModIfTableForm::Entering()
{
}

//virtual
void CModIfTableForm::DoTimer()
{
	SendGetRequest(OidModulatorIfFrequency, OidModulatorIfFrequencyLen);
	SendGetRequest(OidModulatorIfOffset, OidModulatorIfOffsetLen);
	SendGetRequest(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen);
	SendGetRequest(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen);
	SendGetRequest(OidModulatorIfModulation, OidModulatorIfModulationLen);
	SendGetRequest(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen);
}

//virtual
void CModIfTableForm::UpdateControls(LPCTSTR pszReplyTime)
{
	m_Frequency = g_ModIfFrequency;
	m_Offset = g_ModIfOffset;
	m_Level = g_ModIfOutputLevel;
	m_OnOff = g_ModIfOutputEnabled;
	m_Modulation = g_ModIfModulation;
	m_Spectrum = g_ModIfSpectrum;
	if (strlen(pszReplyTime))
		m_ReplyTime = pszReplyTime;
	UpdateData(FALSE);
}

void CModIfTableForm::DoDataExchange(CDataExchange *pDX)
{
	CControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModIfTableForm)
	DDX_Text(pDX, IDC_MOD_IF_FREQUENCY_EDIT, m_Frequency);
	DDX_Text(pDX, IDC_MOD_IF_OFFSET_EDIT, m_Offset);
	DDX_Text(pDX, IDC_MOD_IF_LEVEL_EDIT, m_Level);
	DDX_Text(pDX, IDC_MOD_IF_MODULATION_EDIT, m_Modulation);
	DDX_Text(pDX, IDC_MOD_IF_ON_OFF_EDIT, m_OnOff);
	DDX_Text(pDX, IDC_MOD_IF_SPECTRUM_EDIT, m_Spectrum);
	DDX_Text(pDX, IDC_SECONDS_EDIT, m_ReplyTime);
	//}}AFX_DATA_MAP
}

void CModIfTableForm::OnInitialUpdate()
{
	CControlForm::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	LOGICAL bAdm = IsAdministrator();
	GetDescendantWindow(IDC_MOD_FREQUENCY_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_MOD_OFFSET_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_MOD_LEVEL_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_MOD_ONOFF_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_MOD_MODULATION_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_MOD_SPECTRUM_BUTTON)->EnableWindow(bAdm);
}

void CModIfTableForm::OnModFrequencyButton() 
{
	unsigned int freq = m_Frequency;
	BOOL bChanged = ChangeUnsignedIntValue(freq, 950000, 1350000);
	if (bChanged)
	{
		SendSetUINTRequest(OidModulatorIfFrequency, OidModulatorIfFrequencyLen, freq*1000);
		SendGetRequest(OidModulatorIfFrequency, OidModulatorIfFrequencyLen);
	}
}

void CModIfTableForm::OnModOffsetButton() 
{
	int offset = m_Offset;
	BOOL bChanged = ChangeIntValue(offset, -100000, 100000);
	if (bChanged)
	{
		SendSetIntRequest(OidModulatorIfOffset, OidModulatorIfOffsetLen, offset);
		SendGetRequest(OidModulatorIfOffset, OidModulatorIfOffsetLen);
	}
}

void CModIfTableForm::OnModModulationButton() 
{
	BOOL bBPSK = !m_Modulation.Compare(pszBPSK);
	BOOL bChanged = ChangeModulation(bBPSK);
	if (bChanged)
	{
		if (bBPSK)
			SendSetIntRequest(OidModulatorIfModulation, OidModulatorIfModulationLen, 1);
		else
			SendSetIntRequest(OidModulatorIfModulation, OidModulatorIfModulationLen, 2);
		SendGetRequest(OidModulatorIfModulation, OidModulatorIfModulationLen);
	}
}

void CModIfTableForm::OnModLevelButton() 
{
	double Level = m_Level;
	BOOL bChanged = ChangeDoubleValue(Level, -35, +5);
	if (bChanged)
	{
		SendSetIntRequest(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen, (int)(Level*10));
		SendGetRequest(OidModulatorIfOutputLevel, OidModulatorIfOutputLevelLen);
	}
}

void CModIfTableForm::OnModOnoffButton() 
{
	BOOL bOFF = !m_OnOff.Compare(pszOff);
	BOOL bChanged = OnOff(bOFF);
	if (bChanged)
	{
		if (bOFF)
			SendSetIntRequest(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen, 1);
		else
			SendSetIntRequest(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen, 2);
		SendGetRequest(OidModulatorIfOutputEnabled, OidModulatorIfOutputEnabledLen);
	}
}

void CModIfTableForm::OnModSpectrumButton() 
{
	BOOL bNormal = !m_Spectrum.Compare(pszNormal);
	BOOL bChanged = ChangeSpectrum(bNormal);
	if (bChanged)
	{
		if (bNormal)
			SendSetIntRequest(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen, 1);
		else
			SendSetIntRequest(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen, 2);
		SendGetRequest(OidModulatorIfSpectrum, OidModulatorIfSpectrumLen);
	}
}

void CModIfTableForm::OnQueryNowButton() 
{
	DoTimer();
}

/////////////////////////////////////////////////////////////////////////////
// CModIfTableForm diagnostics

#ifdef _DEBUG
void CModIfTableForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CModIfTableForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CModDataTableForm

IMPLEMENT_DYNCREATE(CModDataTableForm, CControlForm)

BEGIN_MESSAGE_MAP(CModDataTableForm, CControlForm)
	//{{AFX_MSG_MAP(CModDataTableForm)
	ON_BN_CLICKED(IDC_MOD_DATA_BITRATE_BUTTON, OnModDataBitrateButton)
	ON_BN_CLICKED(IDC_MOD_DATA_DIFF_BUTTON, OnModDataDiffButton)
	ON_BN_CLICKED(IDC_MOD_DATA_CODERATE_BUTTON, OnModDataCoderateButton)
	ON_BN_CLICKED(IDC_MOD_DATA_SCRAMBLER_BUTTON, OnModDataScramblerButton)
	ON_BN_CLICKED(IDC_QUERY_NOW_BUTTON, OnQueryNowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CModDataTableForm::CModDataTableForm() : CControlForm(IDD_MOD_DATA_TABLE_FORM)
{
	//{{AFX_DATA_INIT(CModDataTableForm)
	m_BitRate = 0;
	m_ClockSrc = _T("");
	m_CodeRate = _T("");
	m_Diff = _T("");
	m_Scrambler = _T("");
	m_ReplyTime = _T("");
	//}}AFX_DATA_INIT
}

CModDataTableForm::~CModDataTableForm()
{
}

//virtual
void CModDataTableForm::Entering()
{
}

//virtual
void CModDataTableForm::DoTimer()
{
	SendGetRequest(OidModulatorDataBitRate, OidModulatorDataBitRateLen);
	SendGetRequest(OidModulatorDataCodeRate, OidModulatorDataCodeRateLen);
	SendGetRequest(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen);
	SendGetRequest(OidModulatorDataScrambler, OidModulatorDataScramblerLen);
	SendGetRequest(OidModulatorDataClockSrc, OidModulatorDataClockSrcLen);
}

//virtual
void CModDataTableForm::UpdateControls(LPCTSTR pszReplyTime)
{
	m_BitRate = g_ModDataBitRate;
	m_CodeRate = g_ModDataCodeRate;
	m_Diff = g_ModDataDiff;
	m_Scrambler = g_ModDataScrambler;
	m_ClockSrc = g_ModDataClockSource;
	if (strlen(pszReplyTime))
		m_ReplyTime = pszReplyTime;
	UpdateData(FALSE);
}

void CModDataTableForm::DoDataExchange(CDataExchange *pDX)
{
	CControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModDataTableForm)
	DDX_Text(pDX, IDC_DATA_BITRATE_EDIT, m_BitRate);
	DDX_Text(pDX, IDC_DATA_CLOCK_SRC_EDIT, m_ClockSrc);
	DDX_Text(pDX, IDC_DATA_CODERATE_EDIT, m_CodeRate);
	DDX_Text(pDX, IDC_DATA_DIFF_EDIT, m_Diff);
	DDX_Text(pDX, IDC_DATA_SCRAMBLER_EDIT, m_Scrambler);
	DDX_Text(pDX, IDC_SECONDS_EDIT, m_ReplyTime);
	//}}AFX_DATA_MAP
}

void CModDataTableForm::OnInitialUpdate()
{
	CControlForm::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	
	LOGICAL bAdm = IsAdministrator();
	GetDescendantWindow(IDC_MOD_DATA_BITRATE_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_MOD_DATA_CODERATE_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_MOD_DATA_DIFF_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_MOD_DATA_SCRAMBLER_BUTTON)->EnableWindow(bAdm);
}


void CModDataTableForm::OnModDataBitrateButton() 
{
	unsigned int bitrate = m_BitRate;
	BOOL bChanged = ChangeUnsignedIntValue(bitrate, 2400, 2048000);
	if (bChanged)
	{
		SendSetUINTRequest(OidModulatorDataBitRate, OidModulatorDataBitRateLen, bitrate);
		SendGetRequest(OidModulatorDataBitRate, OidModulatorDataBitRateLen);
	}
}

void CModDataTableForm::OnModDataCoderateButton() 
{
	int coderate = 0;
	if (!m_CodeRate.Compare(psz12))
		coderate = 1;
	else if (!m_CodeRate.Compare(psz34))
		coderate = 2;
	else if (!m_CodeRate.Compare(psz78))
		coderate = 3;
	BOOL bChanged = ChangeCodeRate(coderate);
	if (bChanged)
	{
		SendSetIntRequest(OidModulatorDataCodeRate, OidModulatorDataCodeRateLen, coderate);
		SendGetRequest(OidModulatorDataCodeRate, OidModulatorDataCodeRateLen);
	}
}

void CModDataTableForm::OnModDataDiffButton() 
{
	BOOL bOFF = !m_Diff.Compare(pszOff);
	BOOL bChanged = OnOff(bOFF);
	if (bChanged)
	{
		if (bOFF)
			SendSetIntRequest(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen, 1);
		else
			SendSetIntRequest(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen, 2);
		SendGetRequest(OidModulatorDataDiffEncoder, OidModulatorDataDiffEncoderLen);
	}
}

void CModDataTableForm::OnModDataScramblerButton() 
{
	int scrambler = 0;
	if (!m_Scrambler.Compare(pszDisable))
		scrambler = 1;
	else if (!m_Scrambler.Compare(pszIess308))
		scrambler = 2;
	else if (!m_Scrambler.Compare(pszIess308))
		scrambler = 2;
	else if (!m_Scrambler.Compare(pszIess309))
		scrambler = 3;
	else if (!m_Scrambler.Compare(pszV35))
		scrambler = 4;
	else if (!m_Scrambler.Compare(pszAltV35))
		scrambler = 5;
	else if (!m_Scrambler.Compare(pszIntelsat))
		scrambler = 6;
	else if (!m_Scrambler.Compare(pszAltIntelsat))
		scrambler = 7;
	else if (!m_Scrambler.Compare(pszTpcSync))
		scrambler = 8;
	BOOL bChanged = ChangeScrambler(scrambler);
	if (bChanged)
	{
		SendSetIntRequest(OidModulatorDataScrambler, OidModulatorDataScramblerLen, scrambler);
		SendGetRequest(OidModulatorDataScrambler, OidModulatorDataScramblerLen);
	}
}

void CModDataTableForm::OnQueryNowButton() 
{
	DoTimer();
}

/////////////////////////////////////////////////////////////////////////////
// CModDataTableForm diagnostics

#ifdef _DEBUG
void CModDataTableForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CModDataTableForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CModBucTableForm

IMPLEMENT_DYNCREATE(CModBucTableForm, CControlForm)

BEGIN_MESSAGE_MAP(CModBucTableForm, CControlForm)
	//{{AFX_MSG_MAP(CModBucTableForm)
	ON_BN_CLICKED(IDC_QUERY_NOW_BUTTON, OnQueryNowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CModBucTableForm::CModBucTableForm() : CControlForm(IDD_MOD_BUC_TABLE_FORM)
{
	//{{AFX_DATA_INIT(CModBucTableForm)
	m_ReplyTime = _T("");
	m_Voltage = 0.0;
	m_Current = 0.0;
	m_10MHz = _T("");
	m_Power = _T("");
	//}}AFX_DATA_INIT
}

CModBucTableForm::~CModBucTableForm()
{
}

//virtual
void CModBucTableForm::Entering()
{
}

//virtual
void CModBucTableForm::DoTimer()
{
	SendGetRequest(OidModulatorBucVoltageOut, OidModulatorBucVoltageOutLen);
	SendGetRequest(OidModulatorBucCurrentOut, OidModulatorBucCurrentOutLen);
	SendGetRequest(OidModulatorBucPower, OidModulatorBucPowerLen);
	SendGetRequest(OidModulatorBuc10MHz, OidModulatorBuc10MHzLen);
}

//virtual
void CModBucTableForm::UpdateControls(LPCTSTR pszReplyTime)
{
	m_Voltage = g_ModBucVoltage;
	m_Current = g_ModBucVoltage;
	m_10MHz = g_ModBuc10MHz;
	m_Power = g_ModBucPower;
	if (strlen(pszReplyTime))
		m_ReplyTime = pszReplyTime;
	UpdateData(FALSE);
}

void CModBucTableForm::DoDataExchange(CDataExchange *pDX)
{
	CControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModBucTableForm)
	DDX_Text(pDX, IDC_SECONDS_EDIT, m_ReplyTime);
	DDX_Text(pDX, IDC_MOD_VOLTAGE_EDIT, m_Voltage);
	DDX_Text(pDX, IDC_MOD_CURRENT_EDIT, m_Current);
	DDX_Text(pDX, IDC_MOD_10MHZ_EDIT, m_10MHz);
	DDX_Text(pDX, IDC_BUC_POWER_EDIT, m_Power);
	//}}AFX_DATA_MAP
}

void CModBucTableForm::OnInitialUpdate()
{
	CControlForm::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}

void CModBucTableForm::OnQueryNowButton() 
{
	DoTimer();	
}

/////////////////////////////////////////////////////////////////////////////
// CModBucTableForm diagnostics

#ifdef _DEBUG
void CModBucTableForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CModBucTableForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}

#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CDemodIfTableForm

IMPLEMENT_DYNCREATE(CDemodIfTableForm, CControlForm)

BEGIN_MESSAGE_MAP(CDemodIfTableForm, CControlForm)
	//{{AFX_MSG_MAP(CDemodIfTableForm)
	ON_BN_CLICKED(IDC_DEMOD_FREQUENCY_BUTTON, OnDemodFrequencyButton)
	ON_BN_CLICKED(IDC_DEMOD_SWEEP_RANGE_BUTTON, OnDemodSweepRangeButton)
	ON_BN_CLICKED(IDC_DEMOD_MODULATION_BUTTON, OnDemodModulationButton)
	ON_BN_CLICKED(IDC_DEMOD_SPECTRUM_BUTTON, OnDemodSpectrumButton)
	ON_BN_CLICKED(IDC_QUERY_NOW_BUTTON, OnQueryNowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDemodIfTableForm::CDemodIfTableForm() : CControlForm(IDD_DEMOD_IF_TABLE_FORM)
{
	//{{AFX_DATA_INIT(CDemodIfTableForm)
	m_Frequency = 0;
	m_Modulation = _T("");
	m_Spectrum = _T("");
	m_SweepRange = 0;
	m_SweepMode = _T("");
	m_ReplyTime = _T("");
	//}}AFX_DATA_INIT
}

CDemodIfTableForm::~CDemodIfTableForm()
{
}

//virtual
void CDemodIfTableForm::Entering()
{
}

//virtual
void CDemodIfTableForm::DoTimer()
{
	SendGetRequest(OidDemodulatorIfFrequency, OidDemodulatorIfFrequencyLen);
	SendGetRequest(OidDemodulatorIfSweepRange, OidDemodulatorIfSweepRangeLen);
	SendGetRequest(OidDemodulatorIfSweepMode, OidDemodulatorIfSweepModeLen);
	SendGetRequest(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen);
	SendGetRequest(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen);
}

//virtual
void CDemodIfTableForm::UpdateControls(LPCTSTR pszReplyTime)
{
	m_Frequency = g_DemodIfFrequency;
	m_SweepRange = g_DemodIfSweepRange;
	m_SweepMode = g_DemodIfSweepMode;
	m_Modulation = g_DemodIfModulation;
	m_Spectrum = g_DemodIfSpectrum;
	if (strlen(pszReplyTime))
		m_ReplyTime = pszReplyTime;
	UpdateData(FALSE);
}

void CDemodIfTableForm::DoDataExchange(CDataExchange *pDX)
{
	CControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemodIfTableForm)
	DDX_Text(pDX, IDC_DEMOD_IF_FREQUENCY_EDIT, m_Frequency);
	DDX_Text(pDX, IDC_DEMOD_IF_MODULATION_EDIT, m_Modulation);
	DDX_Text(pDX, IDC_DEMOD_IF_SPECTRUM_EDIT, m_Spectrum);
	DDX_Text(pDX, IDC_DEMOD_IF_SWEEP_RANGE_EDIT, m_SweepRange);
	DDX_Text(pDX, IDC_DEMOD_IF_SWEEP_MODE_EDIT, m_SweepMode);
	DDX_Text(pDX, IDC_SECONDS_EDIT, m_ReplyTime);
	//}}AFX_DATA_MAP
}

void CDemodIfTableForm::OnInitialUpdate()
{
	CControlForm::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	LOGICAL bAdm = IsAdministrator();
	GetDescendantWindow(IDC_DEMOD_FREQUENCY_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_DEMOD_SWEEP_RANGE_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_DEMOD_MODULATION_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_DEMOD_SPECTRUM_BUTTON)->EnableWindow(bAdm);
}

void CDemodIfTableForm::OnDemodFrequencyButton() 
{
	unsigned int freq = m_Frequency;
	BOOL bChanged = ChangeUnsignedIntValue(freq, 950000, 1350000);
	if (bChanged)
	{
		SendSetUINTRequest(OidDemodulatorIfFrequency, OidDemodulatorIfFrequencyLen, freq*1000);
		SendGetRequest(OidDemodulatorIfFrequency, OidDemodulatorIfFrequencyLen);
	}
}

void CDemodIfTableForm::OnDemodSweepRangeButton() 
{
	unsigned int range = m_SweepRange;
	BOOL bChanged = ChangeUnsignedIntValue(range, 0, 100000);
	if (bChanged)
	{
		SendSetUINTRequest(OidDemodulatorIfSweepRange, OidDemodulatorIfSweepRangeLen, range);
		SendGetRequest(OidDemodulatorIfSweepRange, OidDemodulatorIfSweepRangeLen);
	}
}

void CDemodIfTableForm::OnDemodModulationButton() 
{
	BOOL bBPSK = !m_Modulation.Compare(pszBPSK);
	BOOL bChanged = ChangeModulation(bBPSK);
	if (bChanged)
	{
		if (bBPSK)
			SendSetIntRequest(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen, 1);
		else
			SendSetIntRequest(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen, 2);
		SendGetRequest(OidDemodulatorIfModulation, OidDemodulatorIfModulationLen);
	}
}

void CDemodIfTableForm::OnDemodSpectrumButton() 
{
	BOOL bNormal = !m_Spectrum.Compare(pszNormal);
	BOOL bChanged = ChangeSpectrum(bNormal);
	if (bChanged)
	{
		if (bNormal)
			SendSetIntRequest(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen, 1);
		else
			SendSetIntRequest(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen, 2);
		SendGetRequest(OidDemodulatorIfSpectrum, OidDemodulatorIfSpectrumLen);
	}
}

void CDemodIfTableForm::OnQueryNowButton() 
{
	DoTimer();	
}

/////////////////////////////////////////////////////////////////////////////
// CDemodIfTableForm diagnostics

#ifdef _DEBUG
void CDemodIfTableForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CDemodIfTableForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CDemodIfTableForm message handlers


/////////////////////////////////////////////////////////////////////////////
// CDemodDataTableForm

IMPLEMENT_DYNCREATE(CDemodDataTableForm, CControlForm)

BEGIN_MESSAGE_MAP(CDemodDataTableForm, CControlForm)
	//{{AFX_MSG_MAP(CDemodDataTableForm)
	ON_BN_CLICKED(IDC_DEMOD_DATA_BITRATE_BUTTON, OnDemodDataBitrateButton)
	ON_BN_CLICKED(IDC_DEMOD_DATA_DIFF_BUTTON, OnDemodDataDiffButton)
	ON_BN_CLICKED(IDC_DEMOD_DATA_CODERATE_BUTTON, OnDemodDataCoderateButton)
	ON_BN_CLICKED(IDC_DEMOD_DATA_DESCRAMBLER_BUTTON, OnDemodDataDescramblerButton)
	ON_BN_CLICKED(IDC_QUERY_NOW_BUTTON, OnQueryNowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDemodDataTableForm::CDemodDataTableForm() : CControlForm(IDD_DEMOD_DATA_TABLE_FORM)
{
	//{{AFX_DATA_INIT(CDemodDataTableForm)
	m_BitRate = 0;
	m_ClockSrc = _T("");
	m_CodeRate = _T("");
	m_Descrambler = _T("");
	m_Diff = _T("");
	m_ReplyTime = _T("");
	//}}AFX_DATA_INIT
}

CDemodDataTableForm::~CDemodDataTableForm()
{
}

//virtual
void CDemodDataTableForm::Entering()
{
}

//virtual
void CDemodDataTableForm::DoTimer()
{
	SendGetRequest(OidDemodulatorDataBitRate, OidDemodulatorDataBitRateLen);
	SendGetRequest(OidDemodulatorDataCodeRate, OidDemodulatorDataCodeRateLen);
	SendGetRequest(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen);
	SendGetRequest(OidDemodulatorDataDescrambler, OidDemodulatorDataDescramblerLen);
	SendGetRequest(OidDemodulatorDataClockSrc, OidDemodulatorDataClockSrcLen);
}

//virtual
void CDemodDataTableForm::UpdateControls(LPCTSTR pszReplyTime)
{
	m_BitRate = g_DemodDataBitRate;
	m_CodeRate = g_DemodDataCodeRate;
	m_Diff = g_DemodDataDiff;
	m_Descrambler = g_DemodDataDescrambler;
	m_ClockSrc = g_DemodDataClockSource;
	if (strlen(pszReplyTime))
		m_ReplyTime = pszReplyTime;
	UpdateData(FALSE);
}

void CDemodDataTableForm::DoDataExchange(CDataExchange *pDX)
{
	CControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemodDataTableForm)
	DDX_Text(pDX, IDC_DATA_BITRATE_EDIT, m_BitRate);
	DDX_Text(pDX, IDC_DATA_CLOCK_SRC_EDIT, m_ClockSrc);
	DDX_Text(pDX, IDC_DATA_CODERATE_EDIT, m_CodeRate);
	DDX_Text(pDX, IDC_DATA_DESCRAMBLER_EDIT, m_Descrambler);
	DDX_Text(pDX, IDC_DATA_DIFF_EDIT, m_Diff);
	DDX_Text(pDX, IDC_SECONDS_EDIT, m_ReplyTime);
	//}}AFX_DATA_MAP
}

void CDemodDataTableForm::OnInitialUpdate()
{
	CControlForm::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	LOGICAL bAdm = IsAdministrator();
	GetDescendantWindow(IDC_DEMOD_DATA_BITRATE_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_DEMOD_DATA_CODERATE_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_DEMOD_DATA_DIFF_BUTTON)->EnableWindow(bAdm);
	GetDescendantWindow(IDC_DEMOD_DATA_DESCRAMBLER_BUTTON)->EnableWindow(bAdm);
}

void CDemodDataTableForm::OnDemodDataBitrateButton() 
{
	unsigned int bitrate = m_BitRate;
	BOOL bChanged = ChangeUnsignedIntValue(bitrate, 2400, 2048000);
	if (bChanged)
	{
		SendSetUINTRequest(OidDemodulatorDataBitRate, OidDemodulatorDataBitRateLen, bitrate);
		SendGetRequest(OidDemodulatorDataBitRate, OidDemodulatorDataBitRateLen);
	}
}

void CDemodDataTableForm::OnDemodDataCoderateButton() 
{
	int coderate = 0;
	if (!m_CodeRate.Compare(psz12))
		coderate = 1;
	else if (!m_CodeRate.Compare(psz34))
		coderate = 2;
	else if (!m_CodeRate.Compare(psz78))
		coderate = 3;
	BOOL bChanged = ChangeCodeRate(coderate);
	if (bChanged)
	{
		SendSetIntRequest(OidDemodulatorDataCodeRate, OidDemodulatorDataCodeRateLen, coderate);
		SendGetRequest(OidDemodulatorDataCodeRate, OidDemodulatorDataCodeRateLen);
	}
}

void CDemodDataTableForm::OnDemodDataDiffButton() 
{
	BOOL bOFF = !m_Diff.Compare(pszOff);
	BOOL bChanged = OnOff(bOFF);
	if (bChanged)
	{
		if (bOFF)
			SendSetIntRequest(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen, 1);
		else
			SendSetIntRequest(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen, 2);
		SendGetRequest(OidDemodulatorDataDiffDecoder, OidDemodulatorDataDiffDecoderLen);
	}
}


void CDemodDataTableForm::OnDemodDataDescramblerButton() 
{
	int descrambler = 0;
	if (!m_Descrambler.Compare(pszDisable))
		descrambler = 1;
	else if (!m_Descrambler.Compare(pszIess308))
		descrambler = 2;
	else if (!m_Descrambler.Compare(pszIess308))
		descrambler = 2;
	else if (!m_Descrambler.Compare(pszIess309))
		descrambler = 3;
	else if (!m_Descrambler.Compare(pszV35))
		descrambler = 4;
	else if (!m_Descrambler.Compare(pszAltV35))
		descrambler = 5;
	else if (!m_Descrambler.Compare(pszIntelsat))
		descrambler = 6;
	else if (!m_Descrambler.Compare(pszAltIntelsat))
		descrambler = 7;
	else if (!m_Descrambler.Compare(pszTpcSync))
		descrambler = 8;
	BOOL bChanged = ChangeScrambler(descrambler);
	if (bChanged)
	{
		SendSetIntRequest(OidDemodulatorDataDescrambler, OidDemodulatorDataDescramblerLen, descrambler);
		SendGetRequest(OidDemodulatorDataDescrambler, OidDemodulatorDataDescramblerLen);
	}
}

void CDemodDataTableForm::OnQueryNowButton() 
{
	DoTimer();
}

/////////////////////////////////////////////////////////////////////////////
// CDemodDataTableForm diagnostics

#ifdef _DEBUG
void CDemodDataTableForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CDemodDataTableForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CDemodLnbTableForm

IMPLEMENT_DYNCREATE(CDemodLnbTableForm, CControlForm)

BEGIN_MESSAGE_MAP(CDemodLnbTableForm, CControlForm)
	//{{AFX_MSG_MAP(CDemodLnbTableForm)
	ON_BN_CLICKED(IDC_QUERY_NOW_BUTTON, OnQueryNowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDemodLnbTableForm::CDemodLnbTableForm() : CControlForm(IDD_DEMOD_LNB_TABLE_FORM)
{
	//{{AFX_DATA_INIT(CDemodLnbTableForm)
	m_ReplyTime = _T("");
	//}}AFX_DATA_INIT
}

CDemodLnbTableForm::~CDemodLnbTableForm()
{
}

//virtual
void CDemodLnbTableForm::Entering()
{
}

//virtual
void CDemodLnbTableForm::DoTimer()
{
	SendGetRequest(OidDemodulatorLnbCurrentOut, OidDemodulatorLnbCurrentOutLen);
}

//virtual
void CDemodLnbTableForm::UpdateControls(LPCTSTR pszReplyTime)
{
	if (strlen(pszReplyTime))
		m_ReplyTime = pszReplyTime;
	UpdateData(FALSE);
}

void CDemodLnbTableForm::DoDataExchange(CDataExchange *pDX)
{
	CControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemodLnbTableForm)
	DDX_Text(pDX, IDC_SECONDS_EDIT, m_ReplyTime);
	//}}AFX_DATA_MAP
}

void CDemodLnbTableForm::OnInitialUpdate()
{
	CControlForm::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}

void CDemodLnbTableForm::OnQueryNowButton() 
{
	DoTimer();	
}

/////////////////////////////////////////////////////////////////////////////
// CDemodLnbTableForm diagnostics

#ifdef _DEBUG
void CDemodLnbTableForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CDemodLnbTableForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}

#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CDemodStatusForm

IMPLEMENT_DYNCREATE(CDemodStatusForm, CControlForm)

BEGIN_MESSAGE_MAP(CDemodStatusForm, CControlForm)
	//{{AFX_MSG_MAP(CDemodStatusForm)
	ON_BN_CLICKED(IDC_QUERY_NOW_BUTTON, OnQueryNowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDemodStatusForm::CDemodStatusForm() : CControlForm(IDD_DEMOD_STATUS_FORM)
{
	//{{AFX_DATA_INIT(CDemodStatusForm)
	m_BER = 0.0;
	m_EbNo = 0.0;
	m_Level = 0.0;
	m_Lock = _T("");
	m_Offset = 0;
	m_SER = 0.0;
	m_Buffer = 0;
	m_ReplyTime = _T("");
	//}}AFX_DATA_INIT
}

CDemodStatusForm::~CDemodStatusForm()
{
}

//virtual
void CDemodStatusForm::Entering()
{
}

//virtual
void CDemodStatusForm::DoTimer()
{
	SendGetRequest(OidDemodulatorStatusLock, OidDemodulatorStatusLockLen);
	SendGetRequest(OidDemodulatorStatusOffset, OidDemodulatorStatusOffsetLen);
	SendGetRequest(OidDemodulatorStatusInputLevel, OidDemodulatorStatusInputLevelLen);
	SendGetRequest(OidDemodulatorStatusEbNo, OidDemodulatorStatusEbNoLen);
	SendGetRequest(OidDemodulatorStatusBER, OidDemodulatorStatusBERLen);
	SendGetRequest(OidDemodulatorStatusSER, OidDemodulatorStatusSERLen);
	SendGetRequest(OidDemodulatorStatusBuffer, OidDemodulatorStatusBufferLen);
}

//virtual
void CDemodStatusForm::UpdateControls(LPCTSTR pszReplyTime)
{
	m_BER = g_DemodStatusBER;
	m_SER = g_DemodStatusSER;
	m_Offset = g_DemodStatusOffset;
	m_Level = g_DemodStatusLevel;
	m_EbNo = g_DemodStatusEbNo;
	m_Lock = g_DemodStatusLock;
	m_Buffer = g_DemodStatusBuffer;
	if (strlen(pszReplyTime))
		m_ReplyTime = pszReplyTime;
	UpdateData(FALSE);
}

void CDemodStatusForm::DoDataExchange(CDataExchange *pDX)
{
	CControlForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemodStatusForm)
	DDX_Text(pDX, IDC_DEMOD_STATUS_BER_EDIT, m_BER);
	DDX_Text(pDX, IDC_DEMOD_STATUS_EBNO_EDIT, m_EbNo);
	DDX_Text(pDX, IDC_DEMOD_STATUS_INPUT_LEVEL_EDIT, m_Level);
	DDX_Text(pDX, IDC_DEMOD_STATUS_LOCK_EDIT, m_Lock);
	DDX_Text(pDX, IDC_DEMOD_STATUS_OFFSET_EDIT, m_Offset);
	DDX_Text(pDX, IDC_DEMOD_STATUS_SER_EDIT, m_SER);
	DDX_Text(pDX, IDC_DEMOD_STATUS_BUFFER_EDIT, m_Buffer);
	DDX_Text(pDX, IDC_SECONDS_EDIT, m_ReplyTime);
	//}}AFX_DATA_MAP
}

void CDemodStatusForm::OnInitialUpdate()
{
	CControlForm::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}

void CDemodStatusForm::OnQueryNowButton() 
{
	DoTimer();
}

/////////////////////////////////////////////////////////////////////////////
// CDemodStatusForm diagnostics

#ifdef _DEBUG
void CDemodStatusForm::AssertValid() const
{
	CControlForm::AssertValid();
}

void CDemodStatusForm::Dump(CDumpContext &dc) const
{
	CControlForm::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CDemodStatusForm message handlers

void CParamsForm::OnApplyButton() 
{
	UpdateData();
	g_DeltaTime = m_DeltaTime;
}

