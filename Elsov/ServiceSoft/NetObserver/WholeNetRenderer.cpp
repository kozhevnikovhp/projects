/* WorldRenderer.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	02 Mar 2007 - initial creation
	02 Apr 2007 - idle moving-swapping
	07 Apr 2007 - warning/alarm frame around host
	15 May 2007 - efficient idle processing, object name in status bar
	20 Oct 2007 - OpenGL tree view instead of MFC tree view
	12 Feb 2008 - sorted items in tree view
	24 Apr 2008 - list of WAN connections instead of list of hosts, connection visibility etc
	13 May 2008 - dynamically recalculated WAN connection elasticity (based on tracert counts)
	20 May 2008 - CPlaneHost and CHost objects are merged to CHost
	23 Jun 2008 - switchable display lists for whole net rendering
*/

#include "stdafx.h"
#include "WholeNetRenderer.h"
#include <math.h>
#include "Network.h"
#include "GuiAPI.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////
// CWholeNetRenderer

CWholeNetRenderer::CWholeNetRenderer()
{
	m_MouseClickDragType = MouseClickDrag_Select;
	m_pSelectedObject = NULL;
	m_pNetwork = NULL;
	MakeDirty();
}

//virtual
CWholeNetRenderer::~CWholeNetRenderer()
{
}

void CWholeNetRenderer::BlockNetworkChange(char *pszText)
{
	if (g_pWholeNetwork)
		g_pWholeNetwork->TakeExclusiveRights(pszText);
}

void CWholeNetRenderer::UnblockNetworkChange(char *pszText)
{
	if (g_pWholeNetwork)
		g_pWholeNetwork->ReleaseExclusiveRights(pszText);
}

//virtual
void CWholeNetRenderer::OnInitialized()
{
	COpenGL_2D::OnInitialized();
	m_SpareDisplayList = glGenLists(1);
	m_WorkingDisplayList = glGenLists(1);
	RebuildDisplayList();
}

//virtual
void CWholeNetRenderer::OnRenderScene()
{
	glCallList(m_WorkingDisplayList);
}

//virtual
void CWholeNetRenderer::OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y)
{
	if (IsInSelectMode())
	{
		CNetObject *pObject = GetObjectAt(x, y);
		if (!pObject)
			return;
		SelectObject(pObject);
		if (g_pGui)
			g_pGui->ObjectSelectionChanged(pObject, this);
	}
}

void CWholeNetRenderer::RebuildDisplayList()
{
	BlockNetworkChange("Render");
	MakeGLContextCurrent();
	glNewList(m_SpareDisplayList, GL_COMPILE);
		RenderToDisplayList();
	glEndList();
	// Swap working and spare lists
	GLuint dummy = m_SpareDisplayList;
	m_SpareDisplayList = m_WorkingDisplayList;
	m_WorkingDisplayList = dummy;
	MakeGLContextNotCurrent();
	m_bDirty = LOGICAL_FALSE;
	UnblockNetworkChange("Render");
}

void CWholeNetRenderer::RefreshView(LOGICAL bRefreshAnyway)
{
	if (bRefreshAnyway || m_bDirty)
		RebuildDisplayList();
	Redraw();
}

void CWholeNetRenderer::SelectObject(CNetObject *pObject)
{
	if (m_pSelectedObject)
		m_pSelectedObject->BeSelected(LOGICAL_FALSE);
	pObject->BeSelected(LOGICAL_TRUE);
	m_pSelectedObject = pObject;
	MakeDirty();
}

void CWholeNetRenderer::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	if (pWhereChanged == this)
		return;
	SelectObject(pObject);
}

///////////////////////////////////////////////////////////////////////////
// CTreeNetObject

CTreeNetObject::CTreeNetObject()
{
}

//virtual
CTreeNetObject::~CTreeNetObject()
{
}

void CTreeNetObject::CalcWholeBox(CWholeNetRenderer *pRenderer, double x, double y, double ObjectHeight)
{
	m_WholeBox.left = pRenderer->MostLeftVisibleX();
	m_WholeBox.right = pRenderer->MostRightVisibleX();
	m_WholeBox.top = y + ObjectHeight;
	m_WholeBox.bottom = y;
}

void CTreeNetObject::RenderWholeBox(CWholeNetRenderer *pRenderer)
{
	glBegin(GL_QUADS);
		glVertex2d(m_WholeBox.left, m_WholeBox.bottom);
		glVertex2d(m_WholeBox.left, m_WholeBox.top);
		glVertex2d(m_WholeBox.right, m_WholeBox.top);
		glVertex2d(m_WholeBox.right, m_WholeBox.bottom);
	glEnd();
}

///////////////////////////////////////////////////////////////////////////
// CTreeHost

CTreeHost::CTreeHost(CHost *pHost)
{
	m_pHost = pHost;
}

//virtual
CTreeHost::~CTreeHost()
{
}

void CTreeHost::Render(CWholeNetTreeRenderer *pRenderer, double &x, double &y, double ObjectHeight, double Indent)
{
	if (!m_pHost->IsVisible())
		return;
	pRenderer->StoreModelViewMatrix();

	if (m_pHost->IsIgnored() || m_pHost->IsHidden())
		glEnable(GL_BLEND);
	CalcWholeBox(pRenderer, x, y, ObjectHeight);
	if (m_pHost->IsSelected())
	{
		NetObjectStatusEnum status = m_pHost->GetGeneralStatusCode();
		switch (status)
		{
		case STATUS_WARNING:
				glColor4ubv(m_pHost->GetWarningSelectedColor());
			break;
		case STATUS_ALARM:
				glColor4ubv(m_pHost->GetAlarmSelectedColor());
			break;
		case STATUS_UNKNOWN:
		case STATUS_OK:
				glColor4ubv(m_pHost->GetOrdinarySelectedColor());
			break;
		default:
			break;
		}
		RenderWholeBox(pRenderer);
	}
	
	glColor4ubv(m_pHost->GetTextColor());
	glRasterPos2d(x+Indent, y+2*ObjectHeight/10);
	char szName[256];
	sprintf(szName, "%s %s", m_pHost->GetIpAddressString(), m_pHost->GetName());
	pRenderer->Draw2DText(szName);
	y -= ObjectHeight;
	glDisable(GL_BLEND);
	pRenderer->RestoreModelViewMatrix();
}
	
CNetObject *CTreeHost::GetNetObject(double x, double y)
{
	if (m_pHost->IsVisible() && PointInBox(x, y))
		return m_pHost;
	return NULL;
}

///////////////////////////////////////////////////////////////////////////
// CTreeSubnet

CTreeSubnet::CTreeSubnet(CSubnet *pSubnet)
{
	m_pSubnet = pSubnet;
	m_bExpanded = LOGICAL_TRUE;
}

//virtual
CTreeSubnet::~CTreeSubnet()
{
}

void CTreeSubnet::Render(CWholeNetTreeRenderer *pRenderer, double &x, double &y, double ObjectHeight, double Indent)
{
	if (!IsVisible())
		return;
	pRenderer->StoreModelViewMatrix();
	CalcWholeBox(pRenderer, x, y, ObjectHeight);
	if (m_pSubnet->IsSelected())
	{
		glColor4ubv(m_pSubnet->GetSelectedColor()->GetRGBAptr());
		RenderWholeBox(pRenderer);
	}
	
	if (m_pSubnet->IsSelected())
		glColor4ubv(m_pSubnet->GetTextSelectedColor()->GetRGBAptr());
	else
		glColor4ubv(m_pSubnet->GetTextColor()->GetRGBAptr());
	CalcExpandBox(pRenderer, x, y, ObjectHeight, Indent);
	RenderExpandBox(pRenderer);

	glRasterPos2d(x+Indent, y+2*ObjectHeight/10);
	char szName[256];
	sprintf(szName, "%s/%s %s", m_pSubnet->GetIpAddressString(), m_pSubnet->GetSubnetMaskString(), m_pSubnet->GetName());
	pRenderer->Draw2DText(szName);
	y -= ObjectHeight;
	pRenderer->RestoreModelViewMatrix();
}
	
CNetObject *CTreeSubnet::GetNetObject(double x, double y)
{
	if (PointInBox(x, y))
		return m_pSubnet;
	return NULL;
}

//inline
LOGICAL CTreeSubnet::IsVisible()
{
	return m_pSubnet->IsVisible();
}

void CTreeSubnet::CalcExpandBox(CWholeNetTreeRenderer *pRenderer, double x, double y, double ObjectHeight, double Indent)
{
	m_ExpandBox.left = x+1*Indent/10;
	m_ExpandBox.bottom = y+1*ObjectHeight/10;
	m_ExpandBox.right = x+8*Indent/10;
	m_ExpandBox.top = y+7*ObjectHeight/10;
}

void CTreeSubnet::RenderExpandBox(CWholeNetTreeRenderer *pRenderer)
{
	glBegin(GL_LINE_LOOP);
		glVertex2d(m_ExpandBox.left, m_ExpandBox.bottom);
		glVertex2d(m_ExpandBox.left, m_ExpandBox.top);
		glVertex2d(m_ExpandBox.right, m_ExpandBox.top);
		glVertex2d(m_ExpandBox.right, m_ExpandBox.bottom);
	glEnd();
	glBegin(GL_LINES);
		glVertex2d(m_ExpandBox.left*0.8+m_ExpandBox.right*0.2, (m_ExpandBox.top+m_ExpandBox.bottom)/2);
		glVertex2d(m_ExpandBox.left*0.2+m_ExpandBox.right*0.8, (m_ExpandBox.top+m_ExpandBox.bottom)/2);
		if (!m_bExpanded)
		{
			glVertex2d((m_ExpandBox.left+m_ExpandBox.right)/2, m_ExpandBox.top*0.8+m_ExpandBox.bottom*0.2);
			glVertex2d((m_ExpandBox.left+m_ExpandBox.right)/2, m_ExpandBox.top*0.2+m_ExpandBox.bottom*0.8);
		}
	glEnd();
}

///////////////////////////////////////////////////////////////////////////
// CWorldTreeRenderer

const char *pszTreeFontName = "Courier New";
const int TreeFontHeight = 20;
enum
{
	SortedByNameEnum,
	SortedByIpAddressEnum
} g_SortType = SortedByIpAddressEnum;

static int CompareNetObjects(CNetObject *pObject1, CNetObject *pObject2)
{
	if (g_SortType == SortedByNameEnum)
	{
		int nCompare = strcmp(pObject1->GetName(), pObject2->GetName());
		if (nCompare != 0)
			return nCompare;
		return pObject1->CompareIpAddress(pObject2);
	}
	else if (g_SortType == SortedByIpAddressEnum)
	{
		return pObject1->CompareIpAddress(pObject2);
	}
	else
		return 0;
}

CWholeNetTreeRenderer::CWholeNetTreeRenderer()
{
	m_uFontBase = 0;
}

CWholeNetTreeRenderer::~CWholeNetTreeRenderer()
{
	for (int iHost = 0; iHost < m_Hosts.size(); iHost++)
	{
		CTreeHost *pTreeHost = m_Hosts[iHost];
		delete pTreeHost;
	}
	for (int iSubnet = 0; iSubnet < m_Subnets.size(); iSubnet++)
	{
		CTreeSubnet *pTreeSubnet = m_Subnets[iSubnet];
		delete pTreeSubnet;
	}
}


//virtual
void CWholeNetTreeRenderer::RenderToDisplayList()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_HostHeight = fabs(ScreenToWorldY(TreeFontHeight)-ScreenToWorldY(0));
	m_Indentation = fabs(ScreenToWorldX(TreeFontHeight)-ScreenToWorldX(0));
	double X = -GetWorldSizeX()/2;
	double Y = 0.9*GetWorldSizeY()/2;
	glListBase(m_uFontBase);
	for (int iSubnet = 0; iSubnet < m_Subnets.size(); iSubnet++)
	{
		CTreeSubnet *pTreeSubnet = m_Subnets[iSubnet];
		pTreeSubnet->Render(this, X, Y, m_HostHeight, m_Indentation);
		X += m_Indentation;
		for (int iHost = 0; iHost < m_Hosts.size(); iHost++)
		{
			CTreeHost *pTreeHost = m_Hosts[iHost];
			CSubnet *pSubnet = pTreeHost->GetHost()->GetSubnet();
			if (pSubnet == pTreeSubnet->GetSubnet())
			{
				if (pTreeSubnet->IsExpanded())
					pTreeHost->Render(this, X, Y, m_HostHeight, m_Indentation);
				else if (pSubnet)
					pTreeHost->InvalidateWholeBox();
			}
		}
		X -= m_Indentation;
	}
	for (int iHost = 0; iHost < m_Hosts.size(); iHost++)
	{
		CTreeHost *pTreeHost = m_Hosts[iHost];
		if (pTreeHost->GetHost()->GetSubnet())
			continue;
		pTreeHost->Render(this, X, Y, m_HostHeight, m_Indentation);
	}
}

//virtual
void CWholeNetTreeRenderer::OnInitialized()
{
	m_uFontBase = Create2DRasterFont(pszTreeFontName, TreeFontHeight);
	CWholeNetRenderer::OnInitialized();
}

//virtual
void CWholeNetTreeRenderer::OnSetDefaultOptions()
{
}

//virtual 
void CWholeNetTreeRenderer::OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y)
{
	double Xworld = ScreenToWorldX(x);
	double Yworld = ScreenToWorldY(y);

	for (CListOfTreeSubnets::iterator it = m_Subnets.begin(); it != m_Subnets.end(); it++)
	{
		CTreeSubnet *pTreeSubnet = *it;
		if (pTreeSubnet->PointInExpandBox(Xworld, Yworld))
		{
			pTreeSubnet->ToggleExpand();
			MakeDirty();
			return;
		}
	}
	CWholeNetRenderer::OnEvent_LeftMouseButtonPressed(uFlags, x, y);
}

void CWholeNetTreeRenderer::SubnetChanged(CSubnet *pSubnet)
{
	MakeDirty();
}

void CWholeNetTreeRenderer::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	MakeDirty();
}

void CWholeNetTreeRenderer::HostChanged(CHost *pHost)
{
	MakeDirty();
}

CTreeHost *CWholeNetTreeRenderer::HostCreated(CHost *pHost)
{
	CTreeHost *pNew = new CTreeHost(pHost);

	LOGICAL bInserted = LOGICAL_FALSE;
	for (CListOfTreeHosts::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		CTreeHost *pExisting = *it;
		if (CompareNetObjects(pExisting->GetHost(), pHost) > 0)
		{
			m_Hosts.insert(it, pNew);
			bInserted = LOGICAL_TRUE;
			break;
		}
	}
	if (!bInserted)
		m_Hosts.push_back(pNew);

//	RefreshView();
	MakeDirty();
	return pNew;
}

CTreeSubnet *CWholeNetTreeRenderer::SubnetCreated(CSubnet *pSubnet)
{
	CTreeSubnet *pNew = new CTreeSubnet(pSubnet);

	LOGICAL bInserted = LOGICAL_FALSE;
	for (CListOfTreeSubnets::iterator it = m_Subnets.begin(); it != m_Subnets.end(); it++)
	{
		CTreeSubnet *pExisting = *it;
		if (CompareNetObjects(pExisting->GetSubnet(), pSubnet) > 0)
		{
			m_Subnets.insert(it, pNew);
			bInserted = LOGICAL_TRUE;
			break;
		}
	}
	if (!bInserted)
		m_Subnets.push_back(pNew);
	
//	RefreshView();
	MakeDirty();
	return pNew;
}

CTreeHost *CWholeNetTreeRenderer::FindTreeHost(CHost *pHost)
{
	for (CListOfTreeHosts::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		CTreeHost *pTreeHost = *it;
		if (pTreeHost->GetHost() == pHost)
			return pTreeHost;
	}

	return NULL;
}

//virtual
CNetObject *CWholeNetTreeRenderer::GetObjectAt(int Xscreen, int Yscreen)
{
	double Xworld = ScreenToWorldX(Xscreen);
	double Yworld = ScreenToWorldY(Yscreen);
	for (CListOfTreeHosts::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		CTreeHost *pTreeHost = *it;
		CNetObject *pNetObject = pTreeHost->GetNetObject(Xworld, Yworld);
		if (pNetObject)
			return pNetObject;
	}
	for (CListOfTreeSubnets::iterator it2 = m_Subnets.begin(); it2 != m_Subnets.end(); it2++)
	{
		CTreeSubnet *pTreeSubnet = *it2;
		CNetObject *pNetObject = pTreeSubnet->GetNetObject(Xworld, Yworld);
		if (pNetObject)
			return pNetObject;
	}
	return NULL;
}

LOGICAL CWholeNetTreeRenderer::OnIdleNonMFCThread()
{
	return LOGICAL_FALSE;
}

void CWholeNetTreeRenderer::NetObjectIgnored(CNetObject *pObject)
{
//	CTreeHost *pTreeHost = FindTreeHost((CHost *)pObject);
//	ASSERT(pTreeHost);
//	RefreshView();
	MakeDirty();
}

void CWholeNetTreeRenderer::NetObjectHidden(CNetObject *pObject)
{
	NetObjectIgnored(pObject);
}

void CWholeNetTreeRenderer::ShowHiddenObjects()
{
/*	for (CListOfTreeHosts::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		CTreeHost *pTreeHost = *it;
		VisualizePlaneHost(pTreeHost);
	}*/
//	RefreshView();
	MakeDirty();
}

void CWholeNetTreeRenderer::ShowIgnoredObjects()
{
	ShowHiddenObjects();
}

class CSubnetsComparator
{
public:
	bool operator() (CTreeSubnet *p1, CTreeSubnet *p2)
	{
		return (CompareNetObjects(p1->GetSubnet(), p2->GetSubnet()) < 0);
	}
};

class CHostsComparator
{
public:
	bool operator() (CTreeHost *p1, CTreeHost *p2)
	{
		return (CompareNetObjects(p1->GetHost(), p2->GetHost()) < 0);
	}
};

void CWholeNetTreeRenderer::SortItems()
{
	BlockNetworkChange("SortItems");
	CSubnetsComparator SubnetComparator;
	std::sort(m_Subnets.begin(), m_Subnets.end(), SubnetComparator);
	CHostsComparator HostComparator;
	std::sort(m_Hosts.begin(), m_Hosts.end(), HostComparator);
	UnblockNetworkChange("SortItems");
}

void CWholeNetTreeRenderer::SortByName()
{
	g_SortType = SortedByNameEnum;
	SortItems();

//	RefreshView();
	MakeDirty();
}

LOGICAL CWholeNetTreeRenderer::IsSortedByName()
{
	return (g_SortType == SortedByNameEnum);
}

void CWholeNetTreeRenderer::SortByIpAddress()
{
	g_SortType = SortedByIpAddressEnum;
	SortItems();

//	RefreshView();
	MakeDirty();
}

LOGICAL CWholeNetTreeRenderer::IsSortedByIpAddress()
{
	return (g_SortType == SortedByIpAddressEnum);
}



// misc
int GetShift(int iCol, int iRow, int W, int E, int N, int S)
{
	int shift = iCol - W + (E-W+1)*(iRow-S);
	ASSERT(shift >= 0);
	ASSERT(shift < (E-W+1)*(N-S+1));
	return shift;
}

CHost **AllocMemory(int W, int E, int N, int S)
{
	int nLength = (E-W+1)*(N-S+1);
	ASSERT(nLength > 0);
	return (CHost **)calloc(nLength, sizeof(CHost *));
}

///////////////////////////////////////////////////////////////////////////
// CWholeNetPlaneRenderer

CWholeNetPlaneRenderer::CWholeNetPlaneRenderer()
{
	m_West = m_East = m_North = m_South = 0;
	m_ppHostMatrix = AllocMemory(m_West, m_East, m_North, m_South);
	m_nIdleID = 0;

	m_NameRect.top = 0;
	m_NameRect.bottom = -fHostSize/4;
	m_NameRect.left = -fHostSize/2*0.95;
	m_NameRect.right = +fHostSize/2*0.95;

	m_IPAddressRect.top = -fHostSize/4;
	m_IPAddressRect.bottom = -fHostSize/2;
	m_IPAddressRect.left = -fHostSize/2*0.9;
	m_IPAddressRect.right = +fHostSize/2*0.9;
	m_bAntialiasingEnabled = LOGICAL_FALSE;
}

CWholeNetPlaneRenderer::~CWholeNetPlaneRenderer()
{
	free(m_ppHostMatrix);
}


//virtual
void CWholeNetPlaneRenderer::RenderToDisplayList()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (m_bAntialiasingEnabled)
	{
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
	}
	else
	{
		glDisable(GL_POLYGON_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
	}
	
	// World boundary
	glColor3f(0.3f, 0.3f, 0.3f);
	glBegin(GL_QUADS);
		glVertex2d(m_West-0.5, m_South-0.5);
		glVertex2d(m_West-0.5, m_North+0.5);
		glVertex2d(m_East+0.5, m_North+0.5);
		glVertex2d(m_East+0.5, m_South-0.5);
	glEnd();

	// Hosts
	for (CListOfNetObjects::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		CHost *pHost = (CHost *)*it;
		pHost->Render(this);
	}

	// WAN connections
	if (g_pWholeNetwork)
	{
		glColor4ub(255, 255, 0, 0);
		glBegin(GL_LINES);
			for (CListOfConnections::iterator ConnIt = g_pWholeNetwork->GetWanConnections()->begin(); ConnIt != g_pWholeNetwork->GetWanConnections()->end(); ConnIt++)
			{
				CConnection *pConnection = *ConnIt;
				if (!pConnection->IsVisible())
					continue;
				CHost *pPlaneHost1 = pConnection->GetPeer1();
				if (!pPlaneHost1->IsPlaced())
					continue;
				CHost *pPlaneHost2 = pConnection->GetPeer2();
				if (!pPlaneHost2->IsPlaced())
					continue;

				double x1 = pPlaneHost1->GetColumn();
				double x2 = pPlaneHost2->GetColumn();
				if (pPlaneHost1->GetColumn() < pPlaneHost2->GetColumn())
				{
					x1 += fHostSize/2;
					x2 -= fHostSize/2;
				}
				else if (pPlaneHost1->GetColumn() > pPlaneHost2->GetColumn())
				{
					x1 -= fHostSize/2;
					x2 += fHostSize/2;
				}

				double y1 = pPlaneHost1->GetRow();
				double y2 = pPlaneHost2->GetRow();
				if (pPlaneHost1->GetRow() < pPlaneHost2->GetRow())
				{
					y1 += fHostSize/2;
					y2 -= fHostSize/2;
				}
				else if (pPlaneHost1->GetRow() > pPlaneHost2->GetRow())
				{
					y1 -= fHostSize/2;
					y2 += fHostSize/2;
				}

				glVertex2d(x1, y1);
				glVertex2d(x2, y2);
			}
		glEnd();
	}
	
	// HW connections
	if (g_pWholeNetwork)
	{
		glColor4ub(255, 128, 0, 0);
		glBegin(GL_LINES);
			for (CListOfConnections::iterator ConnIt = g_pWholeNetwork->GetHardwareConnections()->begin(); ConnIt != g_pWholeNetwork->GetHardwareConnections()->end(); ConnIt++)
			{
				CConnection *pConnection = *ConnIt;
				if (!pConnection->IsVisible())
					continue;
				CHost *pPlaneHost1 = pConnection->GetPeer1();
				if (!pPlaneHost1 || !pPlaneHost1->IsPlaced())
					continue;
				CHost *pPlaneHost2 = pConnection->GetPeer2();
				if (!pPlaneHost2 || !pPlaneHost2->IsPlaced())
					continue;

				double x1 = pPlaneHost1->GetColumn();
				double y1 = pPlaneHost1->GetRow();
				double x2 = pPlaneHost2->GetColumn();
				double y2 = pPlaneHost2->GetRow();

				glVertex2d(x1, y1);
				glVertex2d(x2, y2);
			}
		glEnd();
	}

//	COpenGL_2D::OnRenderScene();
}

//virtual
void CWholeNetPlaneRenderer::OnInitialized()
{
	m_VectorFontListBaseID = CreateVectorFont("Times New", 0, m_GlyphMetrics);
	CWholeNetRenderer::OnInitialized();
}

//virtual
void CWholeNetPlaneRenderer::OnSetDefaultOptions()
{
}

LOGICAL CWholeNetPlaneRenderer::IsAnyNeighbourOfTheSameSubnet(int iCol, int iRow, CHost *pHost)
{
	return (pHost->EqualSubnets(GetHostAt(iCol, iRow-1)) ||
			pHost->EqualSubnets(GetHostAt(iCol, iRow+1)) ||
			pHost->EqualSubnets(GetHostAt(iCol-1, iRow)) ||
			pHost->EqualSubnets(GetHostAt(iCol+1, iRow)) );

}

void CWholeNetPlaneRenderer::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	if (!pHost->GetSubnet())
		pHost = pHost;//debug
	ASSERT(pHost->GetSubnet());
	if (!pHost->IsPlaced())
		PutPlaneHostAtAnyLocation(pHost);

	if (pHost->IsPlaced())
	{
		if (!IsAnyNeighbourOfTheSameSubnet(pHost->GetColumn(), pHost->GetRow(), pHost))
		{ // no any neighbour from the same subnet -> need to find another place
			LOGICAL bNotTheOnlyInSubnet = LOGICAL_FALSE;
			for (CListOfNetObjects::iterator p = m_Hosts.begin(); p != m_Hosts.end(); p++)
			{
				CHost *pAnotherHost = (CHost *)*p;
				if (pAnotherHost == pHost)
					continue;
				if (!pAnotherHost->EqualSubnets(pHost))
					continue;
				bNotTheOnlyInSubnet = LOGICAL_TRUE;
				break;
			}
			if (bNotTheOnlyInSubnet)
			{
				int iCol, iRow;
				LOGICAL bFound = FindPlaceToInsert(pHost, iCol, iRow, SKIP_OCCUPIED_FLAG | SKIP_OUT_OF_SUBNET_FLAG | SKIP_THE_SAME_SUBNET_FLAG);
				if (bFound)
				{
					FreeOccupiedPlace(pHost);
					PutPlaneHostAt(pHost, iCol, iRow);
				}
			}
		}
	}
	MakeDirty();
}

void CWholeNetPlaneRenderer::HostChanged(CHost *pHost)
{
	if (pHost)
	{
		pHost->ContextChanged();
		if (!pHost->IsPlaced())
		{
			PutPlaneHostAtAnyLocation(pHost);
		}
	}
	MakeDirty();
}

void CWholeNetPlaneRenderer::PutPlaneHostAtAnyLocation(CHost *pHost)
{
	if (!pHost->IsVisible())
		return;
	// Find location to place new host at
	int BestCol, BestRow;
	LOGICAL bFound = FindPlaceToInsert(pHost, BestCol, BestRow, SKIP_OCCUPIED_FLAG);
	if (bFound)
		PutPlaneHostAt(pHost, BestCol, BestRow);
	else
	{
		ASSERT(FALSE); //debug
		bFound = FindPlaceToInsert(pHost, BestCol, BestRow, SKIP_OCCUPIED_FLAG);
	}
}

void CWholeNetPlaneRenderer::PutPlaneHostAt(CHost *pHost, int iCol, int iRow)
{
	if (iCol < m_West || iCol > m_East || iRow < m_South || iRow > m_North)
		ExtendWorld();

	CHost **ppPlaceToInsert = GetPlaceForPlaneHostAt(iCol, iRow);
	ASSERT(*ppPlaceToInsert == NULL);
	*ppPlaceToInsert = pHost;
	pHost->SetLocation(iCol, iRow);
	MakeDirty();
}

void CWholeNetPlaneRenderer::FreeOccupiedPlace(CHost *pHost)
{
	ASSERT(pHost->IsPlaced());
	CHost **ppPlaceToInsert = GetPlaceForPlaneHostAt(pHost->GetColumn(), pHost->GetRow());
	ASSERT(*ppPlaceToInsert != NULL);
	*ppPlaceToInsert = NULL;
	pHost->Unplace();
	MakeDirty();
}

void CWholeNetPlaneRenderer::MovePlaneHost(CHost *pHost, int iCol, int iRow)
{
	FreeOccupiedPlace(pHost);
	PutPlaneHostAt(pHost, iCol, iRow);
	//RefreshView();
}

void CWholeNetPlaneRenderer::SwapPlaneHosts(CHost *pHost1, CHost *pHost2)
{
	int iCol = pHost1->GetColumn();
	int iRow = pHost1->GetRow();
	FreeOccupiedPlace(pHost1);
	FreeOccupiedPlace(pHost2);
	PutPlaneHostAt(pHost1, pHost2->GetColumn(), pHost2->GetRow());
	PutPlaneHostAt(pHost2, iCol, iRow);
	//RefreshView();
}

LOGICAL CWholeNetPlaneRenderer::FindPlaceToInsert(CHost *pHost, int &BestCol, int &BestRow, long uFlags)
{
	double Energy, MinEnergy = VERY_BIG;
	LOGICAL bFound = LOGICAL_FALSE;
	for (int i = m_West-1; i <= m_East+1; i++)
	{
		for (int j = m_South-1; j <= m_North+1; j++)
		{
			CHost *pPlaneHost = GetHostAt(i, j);
			if (uFlags & SKIP_OCCUPIED_FLAG)
			{
				if (pPlaneHost)
					continue;
			}
			if (uFlags & SKIP_OUT_OF_SUBNET_FLAG)
			{
				if (!IsAnyNeighbourOfTheSameSubnet(i, j, pHost))
					continue;
			}
			if (uFlags & SKIP_THE_SAME_SUBNET_FLAG)
			{
				if (pPlaneHost && pHost->EqualSubnets(pPlaneHost))
					continue;
			}

			Energy = EnergyAt(pHost, i,j);
			if (Energy < MinEnergy)
			{
				MinEnergy = Energy;
				BestCol = i;
				BestRow = j;
				bFound = LOGICAL_TRUE;
			}
		}
	}

	return bFound;
}

void CWholeNetPlaneRenderer::ExtendWorld()
{
	int West = m_West-1;
	int East = m_East+1;
	int nSizeWE = East-West+1;
	int South = m_South-1;
	int North = m_North+1;
	CHost **ppHostMatrix = AllocMemory(West, East, North, South); 
	for (int i = m_West; i <= m_East; i++)
	{
		for (int j = m_South; j <= m_North; j++)
		{
			int shift = GetShift(i, j, West, East, North, South);
			CHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	m_West = West;
	m_East = East;
	m_South = South;
	m_North = North;
	MakeDirty();
}

void CWholeNetPlaneRenderer::HostCreated(CHost *pHost)
{
	pHost->SetPlaneWorld(this);
	PutPlaneHostAtAnyLocation(pHost);
//	RefreshView();
	MakeDirty();

	m_Hosts.push_back(pHost);
}

CHost **CWholeNetPlaneRenderer::GetPlaceForPlaneHostAt(int iCol, int iRow)
{
	if (iCol < m_West || iCol > m_East)
		return NULL; // nothing outside boundaries
	if (iRow < m_South || iRow > m_North)
		return NULL; // nothing outside boundaries

	int shift = GetShift(iCol, iRow, m_West, m_East, m_North, m_South);

	return m_ppHostMatrix+shift;
}

CHost *CWholeNetPlaneRenderer::GetHostAt(int iCol, int iRow)
{
	CHost **ppPlaceForHost = GetPlaceForPlaneHostAt(iCol, iRow);
	if (!ppPlaceForHost)
		return NULL;
	CHost *pPlaneHost = *ppPlaceForHost;
	if (pPlaneHost)
	{
		ASSERT(pPlaneHost->IsVisible());
	}
	return pPlaneHost;
}

void CWholeNetPlaneRenderer::ZoomIn()
{
	IncreaseZoom(2);
}

void CWholeNetPlaneRenderer::ZoomOut()
{
	IncreaseZoom(0.5);
}

double CWholeNetPlaneRenderer::GravityAt(int iCol, int iRow)
{
	double Kx = (double)m_WindowHeight/(m_WindowHeight+m_WindowWidth);
	double Ky = 1. - Kx;
	double G = 0.8*max(Kx*abs(iCol), Ky*abs(iRow)) + 0.2*sqrt(Kx*iCol*iCol + Ky*iRow*iRow);
	//G += 0.05*(iCol-m_West+iRow-m_South); // move everything to west and south
	return G;
}

double CWholeNetPlaneRenderer::ChemicalInteraction(CHost *pHost, CHost *pNeighbour)
{
	const double fChemicalAttraction = -2.0;
	const double fChemicalRepulsion = +0.2;
	
	double fRouterFactor = 1;
	if (pHost->IsRouter())
		fRouterFactor *= 0.5; // router must tend to subnet boundary
	
	double E = 0;
	if (pNeighbour && pNeighbour != pHost)
	{
		if (pHost->EqualSubnets(pNeighbour))
			E = fChemicalAttraction*fRouterFactor;
		else if (pHost->GetSubnet() && pNeighbour->GetSubnet())
			E = fChemicalRepulsion; // different subnets
		else
			E = fChemicalAttraction/2; // one of them has no subnet so weaker attraction

	}
	return E;
}

double CWholeNetPlaneRenderer::EnergyAt(CHost *pHost, int iCol, int iRow)
{
	return pHost->EnergyAt(iCol, iRow);
}

double CWholeNetPlaneRenderer::Energy(CHost *pHost)
{
	return EnergyAt(pHost, pHost->GetColumn(), pHost->GetRow());	
}

//virtual
CNetObject *CWholeNetPlaneRenderer::GetObjectAt(int Xscreen, int Yscreen)
{
	double Xworld = ScreenToWorldX(Xscreen);
	double Yworld = ScreenToWorldY(Yscreen);
	for (CListOfNetObjects::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		CHost *pPlaneHost = (CHost *)*it;
		if (!pPlaneHost->IsPlaced())
			continue;
		CNetObject *pNetObject = pPlaneHost->GetNetObject(Xworld, Yworld);
		if (pNetObject)
			return pNetObject;
	}
	return NULL;
}

LOGICAL CWholeNetPlaneRenderer::OnIdleNonMFCThread()
{
	if (IsLDragging())
		return LOGICAL_FALSE;
	BlockNetworkChange("CWorldPlaneRenderer::OnIdleNonMFCThread");
	++m_nIdleID;
	m_nIdleID = m_nIdleID % 3;
	LOGICAL bSomethingChanged = LOGICAL_FALSE;
	
	switch (m_nIdleID)
	{
	case 0:
		bSomethingChanged = RandomMove();
		break;
	case 1:
		bSomethingChanged = RandomSwap();
		break;
	case 2:
		bSomethingChanged = RemoveEmptyRowsCols();
		break;
	default:
		break;
	}
	if (bSomethingChanged)
		MakeDirty();

	UnblockNetworkChange("CWorldPlaneRenderer::OnIdleNonMFCThread");

	return bSomethingChanged;
}

void CWholeNetPlaneRenderer::WriteHostIPAddress(char *pszIpAddressString, STextDisplayContext &Context)
{
	WriteText(pszIpAddressString, m_IPAddressRect, ALIGN_HOR_CENTER | ALIGN_VERT_CENTER, Context);
}

void CWholeNetPlaneRenderer::WriteHostName(char *pszHostName, STextDisplayContext &Context)
{
	WriteText(pszHostName, m_NameRect, ALIGN_HOR_CENTER | ALIGN_VERT_CENTER, Context);
}

void CWholeNetPlaneRenderer::WriteText(char *pszText, STextRect &rect, unsigned int alignment, STextDisplayContext &Context)
{
	if (strlen(pszText) == 0)
		return; // do nothing

	// 1. Calc string width
	if (!Context.bInitialized)
	{
		double width = 0;
		char *pszCurrentSymbol = pszText;
		while (*pszCurrentSymbol)
		{
			width += m_GlyphMetrics[*pszCurrentSymbol].gmfCellIncX;
			++pszCurrentSymbol;
		}
		// 2. Height of the string
		double height = m_GlyphMetrics[65].gmfBlackBoxY;

		// Scale
		Context.ScaleX = rect.Width()/width;
		Context.ScaleY = rect.Height()/height;
		if (Context.ScaleX < Context.ScaleY)
			Context.ScaleY = Context.ScaleX;
		else
			Context.ScaleX = Context.ScaleY;
		width *= Context.ScaleX;
		height *= Context.ScaleY;

		// 3. Hor alignment
		if (alignment & ALIGN_RIGHT)
			Context.ShiftX = rect.right - width;
		else if (alignment & ALIGN_LEFT)
			Context.ShiftX = rect.left;
		else if (alignment & ALIGN_HOR_CENTER)
			Context.ShiftX = -width/2;

		// 3. Vert alignment
		if (alignment & ALIGN_TOP)
			Context.ShiftY = rect.top - height;
		else if (alignment & ALIGN_BOTTOM)
			Context.ShiftY = rect.bottom;
		else if (alignment & ALIGN_VERT_CENTER)
			Context.ShiftY = rect.bottom + (rect.Height()- height)/2;

		Context.bInitialized = LOGICAL_TRUE;
	}

	StoreModelViewMatrix();
	glTranslated(Context.ShiftX, Context.ShiftY, 0);
	glScaled(Context.ScaleX, Context.ScaleY, 1);
	glListBase(m_VectorFontListBaseID);
	Draw2DText(pszText);
	RestoreModelViewMatrix();
}

LOGICAL CWholeNetPlaneRenderer::RandomMove()
{
	int iCol2, iRow2;
	CHost *pPlaneHost1 = RandomPlaneHost();
	CHost *pPlaneHost2 = NULL;
	LOGICAL bFound = LOGICAL_FALSE;
	if (pPlaneHost1)
	{
		double E1 = Energy(pPlaneHost1);
		for (int i = m_West-1; i <= m_East+1; i++)
		{
			for (int j = m_South-1; j <= m_North+1; j++)
			{
				CHost *pPlaneHost2 = GetHostAt(i, j);
				if (pPlaneHost2)
					continue;
				double E2 = EnergyAt(pPlaneHost1, i, j);
				if (E2 < (E1-0.001))
				{
					bFound = LOGICAL_TRUE;
					E1 = E2;
					iCol2 = i;
					iRow2 = j;
				}
			}
		}
		if (bFound)
		{
			MovePlaneHost(pPlaneHost1, iCol2, iRow2);
		//	RefreshView();
			MakeDirty();
		}
	}
	return bFound;
}

LOGICAL CWholeNetPlaneRenderer::RandomSwap()
{
	CHost *pPlaneHost1 = RandomPlaneHost();
	CHost *pPlaneHost2best = NULL;
	LOGICAL bFound = LOGICAL_FALSE;
	double DeltaEbest = 0;
	if (pPlaneHost1)
	{
		double E1 = Energy(pPlaneHost1);
		for (int i = m_West; i <= m_East; i++)
		{
			for (int j = m_South; j <= m_North; j++)
			{
				CHost *pPlaneHost2 = GetHostAt(i, j);
				if (!pPlaneHost2)
					continue;
				if (pPlaneHost2 == pPlaneHost1)
					continue;
				if (pPlaneHost1->IsPlaneEqual(pPlaneHost2))
					continue;
				double Ebefore = E1 + Energy(pPlaneHost2);
				SwapPlaneHosts(pPlaneHost1, pPlaneHost2); // swap forward
				double Eafter = Energy(pPlaneHost1) + Energy(pPlaneHost2);
				double DeltaE = Eafter-Ebefore;
				if (DeltaE < (DeltaEbest-0.001))
				{
					bFound = LOGICAL_TRUE;
					DeltaEbest = DeltaE;
					pPlaneHost2best = pPlaneHost2;
				}
				SwapPlaneHosts(pPlaneHost1, pPlaneHost2); // swap back
			}

		}
		if (bFound)
		{
			SwapPlaneHosts(pPlaneHost1, pPlaneHost2best);
			MakeDirty();
		}
	}
	return bFound;
}

LOGICAL CWholeNetPlaneRenderer::RemoveEmptyRowsCols()
{
	LOGICAL bFound;
	int i;
	
	// West
	if (m_West != m_East)
	{
		bFound = LOGICAL_FALSE;
		for (i = m_South; i <= m_North; i++)
		{
			if (GetHostAt(m_West, i) != NULL)
			{
				bFound = LOGICAL_TRUE;
				break;
			}
		}
		if (!bFound)
		{
			RemoveWesternCol();
			return LOGICAL_TRUE;
		}
	}

	// East
	if (m_West != m_East)
	{
		bFound = LOGICAL_FALSE;
		for (i = m_South; i <= m_North; i++)
		{
			if (GetHostAt(m_East, i) != NULL)
			{
				bFound = LOGICAL_TRUE;
				break;
			}
		}
		if (!bFound)
		{
			RemoveEasternCol();
			return LOGICAL_TRUE;
		}
	}

	// South
	if (m_South != m_North)
	{
		bFound = LOGICAL_FALSE;
		for (i = m_West; i <= m_East; i++)
		{
			if (GetHostAt(i, m_South) != NULL)
			{
				bFound = LOGICAL_TRUE;
				break;
			}
		}
		if (!bFound)
		{
			RemoveSouthernRow();
			return LOGICAL_TRUE;
		}
	}

	// North
	if (m_South != m_North)
	{
		bFound = LOGICAL_FALSE;
		for (i = m_West; i <= m_East; i++)
		{
			if (GetHostAt(i, m_North) != NULL)
			{
				bFound = LOGICAL_TRUE;
				break;
			}
		}
		if (!bFound)
		{
			RemoveNorthernRow();
			return LOGICAL_TRUE;
		}
	}

	return LOGICAL_FALSE;
}

void CWholeNetPlaneRenderer::RemoveWesternCol()
{
	CHost **ppHostMatrix = AllocMemory(m_West+1, m_East, m_North, m_South);
	for (int i = m_West+1; i <= m_East; i++)
	{
		for (int j = m_South; j <= m_North; j++)
		{
			int shift = GetShift(i, j, m_West+1, m_East, m_North, m_South);
			ASSERT(shift >= 0);
			ASSERT(shift < (m_East-m_West)*(m_North-m_South+1));
			CHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	++m_West;
	MakeDirty();
}

void CWholeNetPlaneRenderer::RemoveEasternCol()
{
	CHost **ppHostMatrix = AllocMemory(m_West, m_East-1, m_North, m_South);
	for (int i = m_West; i <= (m_East-1); i++)
	{
		for (int j = m_South; j <= m_North; j++)
		{
			int shift = GetShift(i, j, m_West, m_East-1, m_North, m_South);
			ASSERT(shift >= 0);
			ASSERT(shift < (m_East-m_West)*(m_North-m_South+1));
			CHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	--m_East;
	MakeDirty();
}

void CWholeNetPlaneRenderer::RemoveSouthernRow()
{
	CHost **ppHostMatrix = AllocMemory(m_West, m_East, m_North, m_South+1);
	for (int i = m_West; i <= m_East; i++)
	{
		for (int j = m_South+1; j <= m_North; j++)
		{
			int shift = GetShift(i, j, m_West, m_East, m_North, m_South+1);
			ASSERT(shift >= 0);
			ASSERT(shift < (m_East-m_West+1)*(m_North-m_South));
			CHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	++m_South;
	MakeDirty();
}

void CWholeNetPlaneRenderer::RemoveNorthernRow()
{
	CHost **ppHostMatrix = AllocMemory(m_West, m_East, m_North-1, m_South);
	for (int i = m_West; i <= m_East; i++)
	{
		for (int j = m_South; j <= (m_North-1); j++)
		{
			int shift = GetShift(i, j, m_West, m_East, m_North-1, m_South);
			ASSERT(shift >= 0);
			ASSERT(shift < (m_East-m_West+1)*(m_North-m_South));
			CHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	--m_North;
	MakeDirty();
}

CHost *CWholeNetPlaneRenderer::RandomPlaneHost()
{
	if (m_Hosts.size() == 0)
		return NULL;
	CHost *pPlaneHost = NULL;
	for (CListOfNetObjects::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		pPlaneHost = (CHost *)*it;
		if (pPlaneHost->IsPlaced())
		{
			if (!pPlaneHost->HasBuddy())
				return pPlaneHost;
		}
	}

	while (1)
	{
		int size = m_East-m_West + 1;
		int iCol = m_West + (rand() % size);
		size = m_North-m_South + 1;
		int iRow = m_South + (rand() % size);
		pPlaneHost = GetHostAt(iCol, iRow);
		if (pPlaneHost)
			break;
	}
	
	return pPlaneHost;
}

void CWholeNetPlaneRenderer::NetObjectIgnored(CNetObject *pObject)
{
	CHost *pPlaneHost = (CHost *)pObject;
	ASSERT(pPlaneHost);
	VisualizePlaneHost(pPlaneHost);
}

void CWholeNetPlaneRenderer::NetObjectHidden(CNetObject *pObject)
{
	NetObjectIgnored(pObject);
}

void CWholeNetPlaneRenderer::VisualizePlaneHost(CHost *pPlaneHost)
{
	if (pPlaneHost->IsVisible())
	{
		if (!pPlaneHost->IsPlaced())
			PutPlaneHostAtAnyLocation(pPlaneHost);
	}
	else
	{
		FreeOccupiedPlace(pPlaneHost);
	}
	MakeDirty();
}

void CWholeNetPlaneRenderer::ShowHiddenObjects()
{
	for (CListOfNetObjects::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		CHost *pPlaneHost = (CHost *)*it;
		VisualizePlaneHost(pPlaneHost);
	}
	MakeDirty();
}

void CWholeNetPlaneRenderer::ShowIgnoredObjects()
{
	ShowHiddenObjects();
}

