/* BackgroundThread.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	28 Mar 2006 - initial creation
	10 Aug 2006 - RIP full table requests

*/


#include "stdafx.h"
#include "BackgroundThread.h"
#include "Network.h"
#include "Adapter.h"
#include "Portable.h"


/////////////////////////////////////////////////////////////////////
// CBackgroundThread class implementation

//Constuctors/destructors
CBackgroundThread::CBackgroundThread(CAdapter *pAdapter)
{
	m_pAdapter = pAdapter;
	m_CurrentSubnet.addr = 0;
	m_CurrentKeepAliveHost.addr = 0;
	m_CurrentTrafficStoringHost.addr = 0;
}

//virtual
CBackgroundThread::~CBackgroundThread()
{
}

//virtual
void CBackgroundThread::OnThreadStarted()
{
}

//virtual
LOGICAL CBackgroundThread::ThreadBody()
{
	if (NeedContinueThread())
	{
		CSubnet *pSubnet = m_pAdapter->GetNextSubnet(m_CurrentSubnet.addr);
		if (pSubnet)
		{
			//TRACE("Subnet: %s/%s\n", pSubnet->GetIpAddressString(), pSubnet->GetSubnetMaskString());
			CIcmpSocket *pPinger = m_pAdapter->GetBestPinger(pSubnet->GetIpAddress());
		//	pSubnet->BroadcastPing(pPinger);
		//	pSubnet->SendRouterSolicitation(pPinger);
		//	pSubnet->DiscoverNextHost(pPinger);
			portableSleep(50);
		}
	}

	//if (NeedContinueThread())
	//	m_pAdapter->SendRipFullTableRequest();
	
	if (NeedContinueThread())
	{
		m_pAdapter->UpdateTrafficDatabase();
		portableSleep(50);
	}
	
	if (NeedContinueThread())
	{
		CHost *pHost = m_pAdapter->GetNextHost(m_CurrentKeepAliveHost.addr);
		if (pHost)
		{
			//m_pAdapter->KeepAliveHost(pHost);
			portableSwitchContext();
		}
	}

	return TRUE;
}

//virtual
void CBackgroundThread::OnThreadEnded()
{
}

/////////////////////////////////////////////////////////////////////
// CTracertThread class implementation

//Constuctors/destructors
CTracertThread::CTracertThread(CAdapter *pAdapter)
{
	m_pAdapter = pAdapter;
	m_CurrentHost.addr = 0;
}

//virtual
CTracertThread::~CTracertThread()
{
}

//virtual
void CTracertThread::OnThreadStarted()
{
}

//virtual
LOGICAL CTracertThread::ThreadBody()
{
	if (NeedContinueThread())
	{
		CHost *pHost = m_pAdapter->GetNextHost(m_CurrentHost.addr);
		if (pHost)
		{
			m_pAdapter->TracertHost(pHost);
		}
		portableSleep(50);
	}

	return TRUE;
}

//virtual
void CTracertThread::OnThreadEnded()
{
}

