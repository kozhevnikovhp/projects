/* NetworkDocAPI.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	07 Sep 2006 - initial creation
*/

#include "stdafx.h"
#include "NetworkDocAPI.h"
#include "NetworkFrameAPI.h"


/////////////////////////////////////////////////////////////////////////////
// COnIdleVisitor

COnIdleVisitor::COnIdleVisitor()
{
	m_bNeedToBecontinued = FALSE;
}

//virtual
COnIdleVisitor::~COnIdleVisitor()
{
}

//virtual
void COnIdleVisitor::Visit(CNetworkDocAPI *pDoc)
{
	m_bNeedToBecontinued = pDoc->OnIdleProcess();
}

/////////////////////////////////////////////////////////////////////////////
// CGuiCreatedVisitor

CGuiCreatedVisitor::CGuiCreatedVisitor(IGuiAPI *pGui)
{
	m_pGui = pGui;
}

//virtual
CGuiCreatedVisitor::~CGuiCreatedVisitor()
{
}

//virtual
void CGuiCreatedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->GuiCreated(m_pGui);
}


/////////////////////////////////////////////////////////////////////////////
// CNewHostCreatedVisitor

CNewHostCreatedVisitor::CNewHostCreatedVisitor(CHost *pHost)
{
	m_pHost = pHost;
}

//virtual
CNewHostCreatedVisitor::~CNewHostCreatedVisitor()
{
}

//virtual
void CNewHostCreatedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->NewHostCreated(m_pHost);
}


/////////////////////////////////////////////////////////////////////////////
// CHostChangedVisitor

CHostChangedVisitor::CHostChangedVisitor(CHost *pHost)
{
	m_pHost = pHost;
}

//virtual
CHostChangedVisitor::~CHostChangedVisitor()
{
}

//virtual
void CHostChangedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->HostChanged(m_pHost);
}


/////////////////////////////////////////////////////////////////////////////
// CNewSubnetCreatedVisitor

CNewSubnetCreatedVisitor::CNewSubnetCreatedVisitor(CSubnet *pSubnet)
{
	m_pSubnet = pSubnet;
}

//virtual
CNewSubnetCreatedVisitor::~CNewSubnetCreatedVisitor()
{
}

//virtual
void CNewSubnetCreatedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->NewSubnetCreated(m_pSubnet);
}


/////////////////////////////////////////////////////////////////////////////
// CSubnetChangedVisitor

CSubnetChangedVisitor::CSubnetChangedVisitor(CSubnet *pSubnet)
{
	m_pSubnet = pSubnet;
}

//virtual
CSubnetChangedVisitor::~CSubnetChangedVisitor()
{
}

//virtual
void CSubnetChangedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->SubnetChanged(m_pSubnet);
}


/////////////////////////////////////////////////////////////////////////////
// CHostAddedToSubnetVisitor

CHostAddedToSubnetVisitor::CHostAddedToSubnetVisitor(CHost *pHost, CSubnet *pSubnet)
{
	m_pHost = pHost;
	m_pSubnet = pSubnet;
}

//virtual
CHostAddedToSubnetVisitor::~CHostAddedToSubnetVisitor()
{
}

//virtual
void CHostAddedToSubnetVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->HostAddedToSubnet(m_pHost, m_pSubnet);
}


/////////////////////////////////////////////////////////////////////////////
// CHostRemovedFromSubnetVisitor

CHostRemovedFromSubnetVisitor::CHostRemovedFromSubnetVisitor(CHost *pHost, CSubnet *pSubnet)
{
	m_pHost = pHost;
	m_pSubnet = pSubnet;
}

//virtual
CHostRemovedFromSubnetVisitor::~CHostRemovedFromSubnetVisitor()
{
}

//virtual
void CHostRemovedFromSubnetVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->HostRemovedFromSubnet(m_pHost, m_pSubnet);
}


/////////////////////////////////////////////////////////////////////////////
// CObjectSelectionChangedVisitor

CObjectSelectionChangedVisitor::CObjectSelectionChangedVisitor(CNetObject *pObject, void *pWhereChanged)
{
	m_pObject = pObject;
	m_pWhereChanged = pWhereChanged;
}

//virtual
CObjectSelectionChangedVisitor::~CObjectSelectionChangedVisitor()
{
}

//virtual
void CObjectSelectionChangedVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->ObjectSelectionChanged(m_pObject, m_pWhereChanged);
}


/////////////////////////////////////////////////////////////////////////////
// CUpdateGuiVisitor

CUpdateGuiVisitor::CUpdateGuiVisitor()
{
}

//virtual
CUpdateGuiVisitor::~CUpdateGuiVisitor()
{
}

//virtual
void CUpdateGuiVisitor::Visit(CNetworkDocAPI *pDoc)
{
	pDoc->UpdateAllViews(NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CNetworkDocAPI

IMPLEMENT_DYNCREATE(CNetworkDocAPI, CDocument)

BEGIN_MESSAGE_MAP(CNetworkDocAPI, CDocument)
	//{{AFX_MSG_MAP(CNetworkDocAPI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkDocAPI construction/destruction

CNetworkDocAPI::CNetworkDocAPI()
{
	m_pFrame = NULL;
}

CNetworkDocAPI::~CNetworkDocAPI()
{
}

void CNetworkDocAPI::NewHostCreated(CHost *pHost)
{
	if (m_pFrame)
		m_pFrame->NewHostCreated(pHost);
}

void CNetworkDocAPI::HostChanged(CHost *pHost)
{
	if (m_pFrame)
		m_pFrame->HostChanged(pHost);
}

void CNetworkDocAPI::NewSubnetCreated(CSubnet *pSubnet)
{
	if (m_pFrame)
		m_pFrame->NewSubnetCreated(pSubnet);
}

void CNetworkDocAPI::SubnetChanged(CSubnet *pSubnet)
{
	if (m_pFrame)
		m_pFrame->SubnetChanged(pSubnet);
}

void CNetworkDocAPI::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (m_pFrame)
		m_pFrame->HostAddedToSubnet(pHost, pSubnet);
}

void CNetworkDocAPI::HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (m_pFrame)
		m_pFrame->HostRemovedFromSubnet(pHost, pSubnet);
}

void CNetworkDocAPI::GuiCreated(IGuiAPI *pGui)
{
	if (m_pFrame)
		m_pFrame->GuiCreated(pGui);
}

void CNetworkDocAPI::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	if (m_pFrame)
		m_pFrame->ObjectSelectionChanged(pObject, pWhereChanged);
}

LONG CNetworkDocAPI::OnIdleProcess()
{
	if (m_pFrame)
		return m_pFrame->OnIdleProcess();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkDocAPI diagnostics

#ifdef _DEBUG
void CNetworkDocAPI::AssertValid() const
{
	CDocument::AssertValid();
}

void CNetworkDocAPI::Dump(CDumpContext &dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CNetworkDocAPI commands



