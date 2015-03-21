#include "stdafx.h"
#include "IpThreadedSockets.h"

// Class CThreaded implementation

CThreaded::CThreaded()
{
	m_bAlive = TRUE;
	m_hContinueThreadEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	m_hAbortThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ThreadID = 0;
	memset(&m_Overlapped, 0, sizeof(m_Overlapped));
	m_Overlapped.hEvent = m_hAbortThreadEvent;
}

//virtual
CThreaded:: ~CThreaded()
{
	KillOwnThread();
	CloseHandle(m_hAbortThreadEvent);
	CloseHandle(m_hContinueThreadEvent);
}

BOOL CThreaded::KillOwnThread()
{
	m_bAlive = FALSE;
	if (!NeedContinueThread())
		ResetEvent(m_hContinueThreadEvent);  // thread MUST set this event to signalled state before its ending
	SetEvent(m_hAbortThreadEvent);  // thread MUST wait for signalled state of this event to abort even on blocking operations,
								// break blocking operations and end itself setting m_hContinueThread to signalled state
								// (this method will wait for it to be completed correctly)
	WaitForSingleObject(m_hContinueThreadEvent, INFINITE);
	return TRUE;
}

BOOL CThreaded::RunOwnThread(unsigned ( __stdcall *a_lpfnStartAddress)(void *), void *a_pArgList)
{
	m_bAlive = TRUE; // set this value BEFORE thread is run (sometimes thread starts faster than assignment is completed so thread stops itself)
	if (RunThread(a_lpfnStartAddress, a_pArgList, PRIORITY_NORMAL, &m_ThreadID) == 0)
		m_bAlive = FALSE;
	return m_bAlive;
}

BOOL CThreaded::NeedContinueThread()
{
	DWORD dw=WaitForSingleObject(m_hContinueThreadEvent, 0);
	if (dw & WAIT_TIMEOUT)
		return FALSE;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Class CUdpThreadedSocket implementation

CUdpThreadedSocket::CUdpThreadedSocket()
{
	Run();
}

//virtual
CUdpThreadedSocket::~CUdpThreadedSocket()
{
}

unsigned long CUdpThreadedSocket::WaitForDatagram(struct sockaddr_in *a_pSrcSocket)
{
	unsigned long BytesReceived = 0, Flags = 0;
	int fromlen = sizeof(sockaddr_in);
	WSABUF wsa;
	wsa.buf = m_pDatagramBuffer;
	wsa.len = GetMaxDatagramSize();
	WSARecvFrom(GetSocket(), &wsa, 1, &BytesReceived, &Flags, (LPSOCKADDR)a_pSrcSocket, &fromlen, &m_Overlapped, NULL);
	DWORD dw=WaitForSingleObject(m_hAbortThreadEvent, INFINITE);
	WSAGetOverlappedResult(GetSocket(), &m_Overlapped, &BytesReceived, FALSE, &Flags);

	return BytesReceived;
}

//static
unsigned ( __stdcall CUdpThreadedSocket::ThreadProcedure) 
(
	void *a_pParams
)
{
	CUdpThreadedSocket *pThis = (CUdpThreadedSocket *)a_pParams;
	TRACE("CUdpThreadSocket::ThreadProcedure started\n");

	// waiting for socket opened 
	while (pThis->NeedContinueThread() && !pThis->IsOpened())
		Sleep(100);

	struct sockaddr_in SrcSocket;
	memset(&SrcSocket, 0, sizeof(SrcSocket));
	SrcSocket.sin_family = AF_INET;
	SrcSocket.sin_addr.s_addr = htonl(INADDR_ANY);
	while (pThis->IsAlive())
	{
		if (!pThis->NeedContinueThread())
			break; // terminate thread
		DWORD BytesReceived = pThis->WaitForDatagram(&SrcSocket);
		if (BytesReceived)
		{
			TRACE("CUdpThreadedSocket::ThreadProcedure : %d bytes from %d.%d.%d.%d\n", BytesReceived,
				SrcSocket.sin_addr.S_un.S_un_b.s_b1,
				SrcSocket.sin_addr.S_un.S_un_b.s_b2,
				SrcSocket.sin_addr.S_un.S_un_b.s_b3,
				SrcSocket.sin_addr.S_un.S_un_b.s_b4);
			pThis->OnDatagramReceived(SrcSocket.sin_addr.S_un.S_addr, BytesReceived);
		}
	}

	SetEvent(pThis->m_hContinueThreadEvent);
	TRACE("CUdpThreadSocket::ThreadProcedure ended\n");
	_endthreadex(0);
	return 0;
}


// Class CTcpThreadedSocket implementation
CTcpThreadedSocket::CTcpThreadedSocket()
{
}

//virtual
CTcpThreadedSocket::~CTcpThreadedSocket()
{
	TRACE("CTcpThreadedSocket object is destructed\n");
}

BOOL CTcpThreadedSocket::WaitForSymbol(unsigned char &symbol)
{
	unsigned long BytesReceived = 0, Flags = 0;
	WSABUF wsa;
	wsa.buf = (char *)&symbol;
	wsa.len = 1;
	WSARecv(GetSocket(), &wsa, 1, &BytesReceived, &Flags, &m_Overlapped, NULL);
	DWORD dw=WaitForSingleObject(m_hAbortThreadEvent, INFINITE);
	WSAGetOverlappedResult(GetSocket(), &m_Overlapped, &BytesReceived, FALSE, &Flags);

	return (BytesReceived == 1);
}

//virtual
void CTcpThreadedSocket::OnConnected()
{
	RunOwnThread(ThreadProcedure, this);
}

//virtual
void CTcpThreadedSocket::OnDisconnected()
{
	KillOwnThread();
}

//static
unsigned ( __stdcall CTcpThreadedSocket::ThreadProcedure) 
(
	void *a_pParams
)
{
	CTcpThreadedSocket *pThis = (CTcpThreadedSocket *)a_pParams;
	TRACE("CTcpThreadSocket::ThreadProcedure started\n");

	// waiting for socket opened 
	while (pThis->NeedContinueThread() && !pThis->IsConnected())
		Sleep(100);

	while (pThis->IsAlive())
	{
		if (!pThis->NeedContinueThread())
			break; // terminate thread
		unsigned char symbol;
		BOOL bSuccess = pThis->WaitForSymbol(symbol);
		if (pThis->IsAlive() && bSuccess)
		{
			//TRACE("CTcpThreadedSocket::ThreadProcedure : %d (%c)  received\n", symbol, symbol);
			pThis->OnSymbolReceived(symbol);
		}
	}

	SetEvent(pThis->m_hContinueThreadEvent);
	TRACE("CTcpThreadSocket::ThreadProcedure ended\n");
	_endthreadex(0);
	return 0;
}


// Class CIcmpThreadedSocket implementation

CIcmpThreadedSocket::CIcmpThreadedSocket()
{
	RunThread(ThreadProcedure, this, PRIORITY_NORMAL, &m_ThreadID);
}

//virtual
CIcmpThreadedSocket::~CIcmpThreadedSocket()
{
}

//static
unsigned ( __stdcall CIcmpThreadedSocket::ThreadProcedure) 
(
	void *a_pParams
)
{
	CIcmpThreadedSocket *pThis = (CIcmpThreadedSocket *)a_pParams;

	// waiting for socket opened 
	while (pThis->NeedContinueThread() && !pThis->IsOpened())
		Sleep(100);

	while (pThis->IsAlive())
	{
		if (!pThis->NeedContinueThread())
			break; // terminate thread
		pThis->ReceiveEchoReply();
		Sleep(0);
	}

	SetEvent(pThis->m_hContinueThreadEvent);
	TRACE("CIcmpThreadedSocket::ThreadProcedure ended\n");
	_endthreadex(0);
	return 0;
}
