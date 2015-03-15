// SerialStatusForm.cpp : implementation file
//

#include "stdafx.h"
#include "SerialStatusForm.h"
#include "StelaM.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSerialStatusForm

IMPLEMENT_DYNCREATE(CSerialStatusForm, CAbstractForm)

CSerialStatusForm::CSerialStatusForm()
	: CAbstractForm(CSerialStatusForm::IDD)
{
	//{{AFX_DATA_INIT(CSerialStatusForm)
	m_InputPackets = _T("");
	m_InputBytes = _T("");
	m_InputDrops = _T("");
	m_InputErrors = _T("");
	m_OutputBytes = _T("");
	m_OutputDrops = _T("");
	m_OutputErrors = _T("");
	m_OutputPackets = _T("");
	m_ReceiveCRC = _T("");
	m_ReceiveAbortSeq = _T("");
	m_ReceiveDpll = _T("");
	m_ReceiveFrameViolation = _T("");
	m_ReceiveLostCarrier = _T("");
	m_ReceiveNonOctet = _T("");
	m_ReceiveOverrun = _T("");
	m_SendUnderrun = _T("");
	m_SendKeepaliveState = _T("");
	m_SendLastPoll = _T("");
	m_SendLostCTS = _T("");
	m_SendRxSeq = _T("");
	m_SendTxSeq = _T("");
	//}}AFX_DATA_INIT
	m_pSDMS = NULL;
}

CSerialStatusForm::~CSerialStatusForm()
{
}

void CSerialStatusForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialStatusForm)
	DDX_Text(pDX, IDC_INPUT_PACKETS_EDIT, m_InputPackets);
	DDX_Text(pDX, IDC_INPUT_BYTES_EDIT, m_InputBytes);
	DDX_Text(pDX, IDC_INPUT_DROPS_EDIT, m_InputDrops);
	DDX_Text(pDX, IDC_INPUT_ERRORS_EDIT, m_InputErrors);
	DDX_Text(pDX, IDC_OUTPUT_BYTES_EDIT, m_OutputBytes);
	DDX_Text(pDX, IDC_OUTPUT_DROPS_EDIT, m_OutputDrops);
	DDX_Text(pDX, IDC_OUTPUT_ERRORS_EDIT, m_OutputErrors);
	DDX_Text(pDX, IDC_OUTPUT_PACKETS_EDIT, m_OutputPackets);
	DDX_Text(pDX, IDC_RECEIVE_CRC_EDIT, m_ReceiveCRC);
	DDX_Text(pDX, IDC_RECEIVE_ABORT_SEQ_EDIT, m_ReceiveAbortSeq);
	DDX_Text(pDX, IDC_RECEIVE_DPLL_EDIT, m_ReceiveDpll);
	DDX_Text(pDX, IDC_RECEIVE_FRAME_VIOLATIONS_EDIT, m_ReceiveFrameViolation);
	DDX_Text(pDX, IDC_RECEIVE_LOST_CARRIER_EDIT, m_ReceiveLostCarrier);
	DDX_Text(pDX, IDC_RECEIVE_NON_OCTET_EDIT, m_ReceiveNonOctet);
	DDX_Text(pDX, IDC_RECEIVE_OVERRUN_EDIT, m_ReceiveOverrun);
	DDX_Text(pDX, IDC_SEND_UNDERRUN_EDIT, m_SendUnderrun);
	DDX_Text(pDX, IDC_SEND_KEEPALIVE_EDIT, m_SendKeepaliveState);
	DDX_Text(pDX, IDC_SEND_LAST_POLL_EDIT, m_SendLastPoll);
	DDX_Text(pDX, IDC_SEND_LOST_CTS_EDIT, m_SendLostCTS);
	DDX_Text(pDX, IDC_SEND_RX_SEQ_EDIT, m_SendRxSeq);
	DDX_Text(pDX, IDC_SEND_TX_SEQ_EDIT, m_SendTxSeq);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSerialStatusForm, CAbstractForm)
	//{{AFX_MSG_MAP(CSerialStatusForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSerialStatusForm diagnostics

#ifdef _DEBUG
void CSerialStatusForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CSerialStatusForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CSerialStatusForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pSDMS = (CSdmsAPI *)pDevice;

	UpdateStatus();

	return TRUE;
}

BOOL CSerialStatusForm::DoTimer()
{
	UpdateStatus();
	return TRUE;
}

void CSerialStatusForm::UpdateStatus()
{
	SInterfaceStatus status;
	SSerialErrors errors;
	MC_ErrorCode EC = m_pSDMS->GetSerialInterfaceStatus(status, errors);
	const char *pszFormatStr = "%ld";

	// status
	m_InputPackets.Format(pszFormatStr, status.InputPackets);
	m_InputBytes.Format(pszFormatStr, status.InputBytes);
	m_InputErrors.Format(pszFormatStr, status.InputErrors);
	m_InputDrops.Format(pszFormatStr, status.InputDrops);
	m_OutputPackets.Format(pszFormatStr, status.OutputPackets);
	m_OutputBytes.Format(pszFormatStr, status.OutputBytes);
	m_OutputErrors.Format(pszFormatStr, status.OutputErrors);
	m_OutputDrops.Format(pszFormatStr, status.OutputDrops);

	// receive info
	m_ReceiveCRC.Format(pszFormatStr, errors.ReceiveCRC);
	m_ReceiveAbortSeq.Format(pszFormatStr, errors.ReceiveAbortSequence);
	m_ReceiveDpll.Format(pszFormatStr, errors.ReceiveDpll);
	m_ReceiveFrameViolation.Format(pszFormatStr, errors.ReceiveFrameViolations);
	m_ReceiveLostCarrier.Format(pszFormatStr, errors.ReceiveLostCarrier);
	m_ReceiveNonOctet.Format(pszFormatStr, errors.ReceiveNonOctet);
	m_ReceiveOverrun.Format(pszFormatStr, errors.ReceiveOverrun);

	// send info
	m_SendUnderrun.Format(pszFormatStr, errors.SendUnderrun);
	m_SendKeepaliveState.Format(pszFormatStr, errors.SendKeepaliveState);
	m_SendLastPoll.Format(pszFormatStr, errors.SendLastPoll);
	m_SendLostCTS.Format(pszFormatStr, errors.SendLostCTS);
	m_SendRxSeq.Format(pszFormatStr, errors.SendRxSeq);
	m_SendTxSeq.Format(pszFormatStr, errors.SendTxSeq);

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CSerialStatusForm message handlers

