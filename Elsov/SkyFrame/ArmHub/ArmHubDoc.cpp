// ArmHubDoc.cpp : implementation of the CArmHubDoc class
//

#include "stdafx.h"
#include "ArmHub.h"

#include "ArmHubDoc.h"
#include "StationsView.h"
#include "StationsView.h"
#include "TmiOptionsDialog.h"
#include "Thread.h"
#include "IpSockets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CPingSocket : public CIcmpThreadedSocket
{
//Attributes
public:
	CPingSocket(CArmHubDoc *pDoc) { m_pDoc = pDoc; }
	virtual ~CPingSocket() {}
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
// Protected overridables
	virtual BOOL IsOurPacketSequenceNumber(SIcmpHeader *a_pIcmpHdr) { return TRUE; }
	virtual void OnEchoReceived(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned long SourceIP, char *pData, unsigned int nDataSize);
	virtual void OnDestUnreachable(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned long SourceIP, char *pData, unsigned int nDataSize);
// Protected members
	CArmHubDoc *m_pDoc;
};

//virtual
void CTmiSocket::OnDatagramReceived(unsigned long IpAddress, int DatagramLength)
{
	// what is the source station?
	m_pStation->ProcessPpdTmiDatagram(m_pDatagramBuffer, DatagramLength);
//	m_pDoc->Save();
}

//virtual
void CPingSocket::OnEchoReceived(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned long SourceIP, char *pData, unsigned int nDataSize)
{
	m_pDoc->DispatchPingEchoReply(pIpHeader, pIcmpHeader, SourceIP);
}

//virtual
void CPingSocket::OnDestUnreachable(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned long SourceIP, char *pData, unsigned int nDataSize)
{
	m_pDoc->DispatchPingDestUnreachable(pIpHeader, pIcmpHeader);
}

static char *pszSaveConfigFileName="stations.cfg";

/////////////////////////////////////////////////////////////////////////////
// CArmHubDoc

IMPLEMENT_DYNCREATE(CArmHubDoc, CDocument)

BEGIN_MESSAGE_MAP(CArmHubDoc, CDocument)
	//{{AFX_MSG_MAP(CArmHubDoc)
	ON_COMMAND(ID_STATIONS_ADD, OnStationsAdd)
	ON_COMMAND(ID_STATIONS_DELETE, OnStationsDelete)
	ON_COMMAND(ID_STATIONS_IMPORT, OnStationsImport)
	ON_COMMAND(ID_STATIONS_EXPORT, OnStationsExport)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArmHubDoc construction/destruction

CArmHubDoc::CArmHubDoc()
{
	InitializeCriticalSection(&m_Critical);
	m_StationIteratorPos = NULL;
	m_pPingSocket = new CPingSocket(this);
	m_pPingSocket->SetPingTimeout(5000);
	CStation::m_pPingSocket = m_pPingSocket;
	m_Scheduler.SetDocument(this);
	m_Scheduler.Run(&m_Scheduler);
	SetModifiedFlag();
}

CArmHubDoc::~CArmHubDoc()
{
	m_Scheduler.KillYourself();
	LockStations();
	StationList.Clear();
	UnlockStations();
	DeleteCriticalSection(&m_Critical);
	delete m_pPingSocket;

	TRACE("CArmHubDoc object is destructed\n");
}

BOOL CArmHubDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	StationList.Clear();
	m_pTreeView->Clear();

	CFile file;
	if (!file.Open(pszSaveConfigFileName, CFile::modeRead))
		return TRUE;
	CArchive archive(&file, CArchive::load);
	Serialize(archive);
	LockStations();
	for (CStation *pStation = GetFirstStation(); pStation; pStation = GetNextStation())
	{
		m_pTreeView->AddStation(pStation);
	}
	UnlockStations();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CArmHubDoc serialization

void CArmHubDoc::Serialize(CArchive& ar)
{
	LockStations();
	StationList.Serialize(ar);
	UnlockStations();
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

void CArmHubDoc::AddNewForm(FI_FormIndex IndexInViewArray, CView *pViewToAdd, CWnd *pParent)
{
	m_pForms[IndexInViewArray]=(CAbstractForm *)pViewToAdd;
	pViewToAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CFrameWnd::rectDefault, pParent,
		AFX_IDW_PANE_FIRST+IndexInViewArray+1, NULL);
	AddView(pViewToAdd);
}

/////////////////////////////////////////////////////////////////////////////
// CArmHubDoc diagnostics

#ifdef _DEBUG
void CArmHubDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CArmHubDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArmHubDoc commands

void CArmHubDoc::OnStationsAdd() 
{
	CStation *pStation=new CStation("New station");
	AddStation(pStation);
	m_pTreeView->AddStation(pStation);
}

void CArmHubDoc::OnStationsDelete() 
{
	CStation *pStationToDelete=m_pTreeView->GetSelectedStation();
	if (!pStationToDelete)
		return;
	DeleteStation(pStationToDelete);
}

void CArmHubDoc::AddStation(CStation *pStationToAdd)
{
	LockStations();
	StationList.AddTail(pStationToAdd);
	UnlockStations();
}

void CArmHubDoc::DeleteStation(CStation *pStationToDelete)
{
	LockStations();
	for (CStation *pStation = GetFirstStation(); pStation; pStation = GetNextStation())
	{
		if (pStation == pStationToDelete)
		{
			EnterCritical();
			StationList.RemoveAt(m_StationIteratorPos);
			LeaveCritical();
			m_pTreeView->DeleteSelectedStation();
			delete pStationToDelete;
			break;
		}
	}
	UnlockStations();
}

/*void CArmHubDoc::DispatchTmiDatagram
(
	unsigned long IpAddress,	
	char *pBuffer,
	int length
)
{
	LockStations();
	for (CStation *pStation = GetFirstStation(); pStation; pStation = GetNextStation())
	{
		if (pStation->IsMyAddress(IpAddress))
		{
			TRACE(" - directed to %s\n", pStation->GetName());
			pStation->ProcessPpdTmiDatagram(pBuffer, length);
			break;
		}
	}
	UnlockStations();
}*/

void CArmHubDoc::DispatchPingEchoReply(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader, unsigned long SourceIP)
{
	LockStations();
	TRACE("Ping echo reply from %d.%d.%d.%d\n", LOBYTE(LOWORD(SourceIP)), HIBYTE(LOWORD(SourceIP)), LOBYTE(HIWORD(SourceIP)), HIBYTE(HIWORD(SourceIP)));
	for (CStation *pStation = GetFirstStation(); pStation; pStation = GetNextStation())
	{
		if (pStation->IsMyPingEchoReply(SourceIP))
		{
			TRACE(" - directed to %s\n", pStation->GetName());
			pStation->ProcessPingEchoReply(pIpHeader, pIcmpHeader, SourceIP);
			break;
		}
	}
	UnlockStations();
}

void CArmHubDoc::DispatchPingDestUnreachable(SIpHeader *pIpHeader, SIcmpHeader *pIcmpHeader)
{
	unsigned long IpAddress = pIpHeader->destIP;
	LockStations();
	for (CStation *pStation = GetFirstStation(); pStation; pStation = GetNextStation())
	{
		if (pStation->IsMyPingDestUnreachable(IpAddress))
		{
			TRACE("Ping dest unreachable - directed to %s\n", pStation->GetName());
			pStation->ProcessPingDestUnreachable(pIpHeader, pIcmpHeader);
			break;
		}
	}
	LockStations();
}

BOOL CArmHubDoc::SaveModified() 
{
	Save();
	return TRUE;
}

void CArmHubDoc::Save()
{
	CFile file;
	if (!file.Open(pszSaveConfigFileName, CFile::modeCreate | CFile::modeWrite))
		return;
	CArchive archive(&file, CArchive::store);
	Serialize(archive);
}

CStation *CArmHubDoc::GetFirstStation()
{
	EnterCritical();
	CStation *pStation = NULL;
	m_StationIteratorPos = StationList.GetHeadPosition();
	if (m_StationIteratorPos)
		pStation = (CStation *)StationList.GetAt(m_StationIteratorPos);
	LeaveCritical();
	return pStation;
}

CStation *CArmHubDoc::GetNextStation()
{
	EnterCritical();
	if (m_StationIteratorPos)
		StationList.GetNext(m_StationIteratorPos);
	CStation *pStation = NULL;
	if (m_StationIteratorPos)
		pStation = (CStation *)StationList.GetAt(m_StationIteratorPos);
	LeaveCritical();
	return pStation;
}

void CArmHubDoc::EnterCritical()
{
	EnterCriticalSection(&m_Critical);
}

void CArmHubDoc::LeaveCritical()
{
	LeaveCriticalSection(&m_Critical);
}

void CArmHubDoc::OnStationsImport() 
{
	CFileDialog fd(TRUE, _T(".st"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Station files (*.st)|*.st|All files (*.*)|*.*||"));
	fd.m_ofn.lpstrTitle="Point file to load, please";
	if (fd.DoModal()==IDCANCEL) return;
	CString FileName = fd.GetPathName();
	CFile file;
	if (!file.Open(FileName, CFile::modeRead))
		return;
	CArchive archive(&file, CArchive::load);
	CListOfObjects NewStationList;
	NewStationList.Serialize(archive);
	for (POSITION pos = NewStationList.GetHeadPosition(); pos; NewStationList.GetNext(pos))
	{
		CStation *pStation = (CStation *)NewStationList.GetAt(pos);
		CStation *pStationToAdd = new CStation(pStation);
		AddStation(pStationToAdd);
		m_pTreeView->AddStation(pStationToAdd);
	}
	Save();
}


void CArmHubDoc::OnStationsExport() 
{
	CFileDialog fd(FALSE, _T(".st"), NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Station files (*.st)|*.st|All files (*.*)|*.*||"));
	fd.m_ofn.lpstrTitle="Point file to save, please";
	if (fd.DoModal()==IDCANCEL) return;
	CString FileName = fd.GetPathName();
	CFile file;
	if (!file.Open(FileName, CFile::modeCreate | CFile::modeWrite))
		return;
	CArchive archive(&file, CArchive::store);
	LockStations();
	StationList.Serialize(archive);
	UnlockStations();
}
