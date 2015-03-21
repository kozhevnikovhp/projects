#include "stdafx.h"
#include "UdpThreadSocket.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// Class CUdpThreadSocket implementation

CUdpThreadSocket::CUdpThreadSocket()
{
	memset(&m_Overlapped, 0, sizeof(m_Overlapped));
	m_Overlapped.hEvent = m_hAbortThreadEvent;
}

//virtual
CUdpThreadSocket::~CUdpThreadSocket()
{
	TRACE("CUdpThreadSocket object is destructed\n");
}

