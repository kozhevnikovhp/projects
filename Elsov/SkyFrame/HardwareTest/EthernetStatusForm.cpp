// EthernetStatusForm.cpp : implementation file
//

#include "stdafx.h"
#include "EthernetStatusForm.h"
#include "StelaM.h"
#include "Attached.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEthernetStatusForm

IMPLEMENT_DYNCREATE(CEthernetStatusForm, CAbstractForm)

CEthernetStatusForm::CEthernetStatusForm()
	: CAbstractForm(CEthernetStatusForm::IDD)
{
	//{{AFX_DATA_INIT(CEthernetStatusForm)
	m_InputBytes = _T("");
	m_InputDrops = _T("");
	m_InputErrors = _T("");
	m_InputPackets = _T("");
	m_OutputBytes = _T("");
	m_OutputDrops = _T("");
	m_OutputErrors = _T("");
	m_OutputPackets = _T("");
	m_ReceiveBadCRC = _T("");
	m_ReceiveCollision = _T("");
	m_ReceiveDiscarded = _T("");
	m_ReceiveGiant = _T("");
	m_ReceiveNonOctet = _T("");
	m_ReceiveOverrun = _T("");
	m_ReceiveShort = _T("");
	m_SendDeffered = _T("");
	m_SendHeartBeat = _T("");
	m_SendLateCollisions = _T("");
	m_SendLostCarrier = _T("");
	m_SendRetransmitLimits = _T("");
	m_SendRetries = _T("");
	m_SendUnderrun = _T("");
	//}}AFX_DATA_INIT
	m_pSDMS = NULL;
}

CEthernetStatusForm::~CEthernetStatusForm()
{
}

void CEthernetStatusForm::DoDataExchange(CDataExchange *pDX)
{
	CAbstractForm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEthernetStatusForm)
	DDX_Text(pDX, IDC_INPUT_BYTES_EDIT, m_InputBytes);
	DDX_Text(pDX, IDC_INPUT_DROPS_EDIT, m_InputDrops);
	DDX_Text(pDX, IDC_INPUT_ERRORS_EDIT, m_InputErrors);
	DDX_Text(pDX, IDC_INPUT_PACKETS_EDIT, m_InputPackets);
	DDX_Text(pDX, IDC_OUTPUT_BYTES_EDIT, m_OutputBytes);
	DDX_Text(pDX, IDC_OUTPUT_DROPS_EDIT, m_OutputDrops);
	DDX_Text(pDX, IDC_OUTPUT_ERRORS_EDIT, m_OutputErrors);
	DDX_Text(pDX, IDC_OUTPUT_PACKETS_EDIT, m_OutputPackets);
	DDX_Text(pDX, IDC_RECEIVE_BAD_CRC_EDIT, m_ReceiveBadCRC);
	DDX_Text(pDX, IDC_RECEIVE_COLLISION_EDIT, m_ReceiveCollision);
	DDX_Text(pDX, IDC_RECEIVE_DISCARDED_EDIT, m_ReceiveDiscarded);
	DDX_Text(pDX, IDC_RECEIVE_GIANT_EDIT, m_ReceiveGiant);
	DDX_Text(pDX, IDC_RECEIVE_NON_OCTET_EDIT, m_ReceiveNonOctet);
	DDX_Text(pDX, IDC_RECEIVE_OVERRUN_EDIT, m_ReceiveOverrun);
	DDX_Text(pDX, IDC_RECEIVE_SHORT_EDIT, m_ReceiveShort);
	DDX_Text(pDX, IDC_SEND_DEFFERED_EDIT, m_SendDeffered);
	DDX_Text(pDX, IDC_SEND_HEARTBEAT_EDIT, m_SendHeartBeat);
	DDX_Text(pDX, IDC_SEND_LATE_COLLISIONS_EDIT, m_SendLateCollisions);
	DDX_Text(pDX, IDC_SEND_LOST_CARRIER_EDIT, m_SendLostCarrier);
	DDX_Text(pDX, IDC_SEND_RETRANSMIT_LIMITS_EDIT, m_SendRetransmitLimits);
	DDX_Text(pDX, IDC_SEND_RETRIES_EDIT, m_SendRetries);
	DDX_Text(pDX, IDC_SEND_UNDERRUN_EDIT, m_SendUnderrun);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEthernetStatusForm, CAbstractForm)
	//{{AFX_MSG_MAP(CEthernetStatusForm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEthernetStatusForm diagnostics

#ifdef _DEBUG
void CEthernetStatusForm::AssertValid() const
{
	CAbstractForm::AssertValid();
}

void CEthernetStatusForm::Dump(CDumpContext &dc) const
{
	CAbstractForm::Dump(dc);
}
#endif //_DEBUG

BOOL CEthernetStatusForm::SetupControls()
{
	CDevice *pDevice = m_pAttached->m_pDevice;
	m_pSDMS = (CSdmsAPI *)pDevice;

	UpdateStatus();

	return TRUE;
}

BOOL CEthernetStatusForm::DoTimer()
{
	UpdateStatus();
	return TRUE;
}

void CEthernetStatusForm::UpdateStatus()
{
	SInterfaceStatus status;
	SEthernetErrors errors;
	MC_ErrorCode EC = m_pSDMS->GetEthernetInterfaceStatus(status, errors);
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

	// receive errors
	m_ReceiveShort.Format(pszFormatStr, errors.ReceiveShort);
	m_ReceiveGiant.Format(pszFormatStr, errors.ReceiveGiant);
	m_ReceiveNonOctet.Format(pszFormatStr, errors.ReceiveNonOctet);
	m_ReceiveBadCRC.Format(pszFormatStr, errors.ReceiveCRC);
	m_ReceiveOverrun.Format(pszFormatStr, errors.ReceiveOverrun);
	m_ReceiveCollision.Format(pszFormatStr, errors.ReceiveCollision);
	m_ReceiveDiscarded.Format(pszFormatStr, errors.ReceiveDiscarded);
	
	// send errors
	m_SendUnderrun.Format(pszFormatStr, errors.SendUnderrun);
	m_SendLateCollisions.Format(pszFormatStr, errors.SendLateCollisions);
	m_SendDeffered.Format(pszFormatStr, errors.SendDeffered);
	m_SendLostCarrier.Format(pszFormatStr, errors.SendLostCarrier);
	m_SendHeartBeat.Format(pszFormatStr, errors.SendHeartBeat);
	m_SendRetries.Format(pszFormatStr, errors.SendRetries);
	m_SendRetransmitLimits.Format(pszFormatStr, errors.SendRetransmitLimits);


	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CEthernetStatusForm message handlers

