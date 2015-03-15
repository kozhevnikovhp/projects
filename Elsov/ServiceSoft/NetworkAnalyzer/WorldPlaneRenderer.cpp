/* WorldPlaneRenderer.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	02 Mar 2007 - initial creation
	02 Apr 2007 - idle moving-swapping
	07 Apr 2007 - warning/alarm frame around host
	15 May 2007 - efficient idle processing, object name in status bar

*/

#include "stdafx.h"
#include "WorldPlaneRenderer.h"
#include <math.h>
#include "Network.h"
#include "GuiAPI.h"

// misc
int GetShift(int iCol, int iRow, int W, int E, int N, int S)
{
	int shift = iCol - W + (E-W+1)*(iRow-S);
	ASSERT(shift >= 0);
	ASSERT(shift < (E-W+1)*(N-S+1));
	return shift;
}

///////////////////////////////////////////////////////////////////////////
// CPlaneObject

CPlaneObject::CPlaneObject()
{
}

//virtual
CPlaneObject::~CPlaneObject()
{
}


///////////////////////////////////////////////////////////////////////////
// CPlaneHost

CPlaneHost::CPlaneHost(CHost *pHost, CWorldPlaneRenderer *pWorld, int iCol, int iRow)
{
	m_pHost = pHost;
	m_pPlaneSubnet = NULL;
	m_pWorld = pWorld;
	m_iCol = iCol;
	m_iRow = iRow;
	m_NameDiusplayContext.bInitialized = m_IpAddressDisplayContext.bInitialized = LOGICAL_FALSE;
}

//virtual
CPlaneHost::~CPlaneHost()
{
}

int CPlaneHost::GetNeighbourCount()
{
	int count = 0;
	if (m_pWorld->GetPlaneHostAt(m_iCol-1, m_iRow))
		++count;
	if (m_pWorld->GetPlaneHostAt(m_iCol+1, m_iRow))
		++count;
	if (m_pWorld->GetPlaneHostAt(m_iCol, m_iRow-1))
		++count;
	if (m_pWorld->GetPlaneHostAt(m_iCol, m_iRow+1))
		++count;

	return count;
}

void CPlaneHost::Render(CWorldPlaneRenderer *pRenderer)
{
	pRenderer->StoreModelViewMatrix();
	CSubnet *pSubnet = m_pHost->GetSubnet();
	NetObjectStatusEnum status = m_pHost->GetGeneralStatusCode();

	glTranslated(m_iCol, m_iRow, 0);
	glBegin(GL_QUADS);
		//Subnet rectangle
		if (pSubnet)
		{
			if (pSubnet->IsSelected())
				glColor4ubv(pRenderer->SelectedSubnetColor());
			else
				glColor4ubv(pRenderer->SubnetColor());
			glVertex2d(-fSubnetSize/2, -fSubnetSize/2);
			glVertex2d(-fSubnetSize/2, +fSubnetSize/2);
			glVertex2d(+fSubnetSize/2, +fSubnetSize/2);
			glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
			LOGICAL bNorth, bSouth, bWest, bEast;
			// North
			CPlaneHost *pBuddy = m_pWorld->GetPlaneHostAt(m_iCol, m_iRow+1);
			if (pBuddy && pBuddy->m_pHost->GetSubnet() == pSubnet)
			{
				glVertex2d(-fSubnetSize/2, -fSubnetSize/2);
				glVertex2d(-fSubnetSize/2, +0.5);
				glVertex2d(+fSubnetSize/2, +0.5);
				glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				bNorth = LOGICAL_TRUE;
			}
			else
				bNorth = LOGICAL_FALSE;
			// South
			pBuddy = m_pWorld->GetPlaneHostAt(m_iCol, m_iRow-1);
			if (pBuddy && pBuddy->m_pHost->GetSubnet() == pSubnet)
			{
				glVertex2d(-fSubnetSize/2, -fSubnetSize/2);
				glVertex2d(-fSubnetSize/2, -0.5);
				glVertex2d(+fSubnetSize/2, -0.5);
				glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				bSouth = LOGICAL_TRUE;
			}
			else
				bSouth = LOGICAL_FALSE;
			// West
			pBuddy = m_pWorld->GetPlaneHostAt(m_iCol-1, m_iRow);
			if (pBuddy && pBuddy->m_pHost->GetSubnet() == pSubnet)
			{
				glVertex2d(-0.5, -fSubnetSize/2);
				glVertex2d(-0.5, +fSubnetSize/2);
				glVertex2d(+fSubnetSize/2, +fSubnetSize/2);
				glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				bWest = LOGICAL_TRUE;
			}
			else
				bWest = LOGICAL_FALSE;
			// East
			pBuddy = m_pWorld->GetPlaneHostAt(m_iCol+1, m_iRow);
			if (pBuddy && pBuddy->m_pHost->GetSubnet() == pSubnet)
			{
				glVertex2d(+0.5, -fSubnetSize/2);
				glVertex2d(+0.5, +fSubnetSize/2);
				glVertex2d(+fSubnetSize/2, +fSubnetSize/2);
				glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				bEast = LOGICAL_TRUE;
			}
			else
				bEast = LOGICAL_FALSE;
			// North-West
			if (bNorth && bWest)
			{
				pBuddy = m_pWorld->GetPlaneHostAt(m_iCol-1, m_iRow+1);
				if (pBuddy && pBuddy->m_pHost->GetSubnet() == pSubnet)
				{
					glVertex2d(-0.5, +fSubnetSize/2);
					glVertex2d(-0.5, +0.5);
					glVertex2d(-fSubnetSize/2, +0.5);
					glVertex2d(-fSubnetSize/2, +fSubnetSize/2);
				}
			}
			// North-East
			if (bNorth && bEast)
			{
				pBuddy = m_pWorld->GetPlaneHostAt(m_iCol+1, m_iRow+1);
				if (pBuddy && pBuddy->m_pHost->GetSubnet() == pSubnet)
				{
					glVertex2d(+0.5, +fSubnetSize/2);
					glVertex2d(+0.5, +0.5);
					glVertex2d(+fSubnetSize/2, +0.5);
					glVertex2d(+fSubnetSize/2, +fHostSize/2);
				}
			}
			// South-East
			if (bSouth && bEast)
			{
				pBuddy = m_pWorld->GetPlaneHostAt(m_iCol+1, m_iRow-1);
				if (pBuddy && pBuddy->m_pHost->GetSubnet() == pSubnet)
				{
					glVertex2d(+0.5, -fSubnetSize/2);
					glVertex2d(+0.5, -0.5);
					glVertex2d(+fSubnetSize/2, -0.5);
					glVertex2d(+fSubnetSize/2, -fSubnetSize/2);
				}
			}
			// South-West
			if (bSouth && bWest)
			{
				pBuddy = m_pWorld->GetPlaneHostAt(m_iCol-1, m_iRow-1);
				if (pBuddy && pBuddy->m_pHost->GetSubnet() == pSubnet)
				{
					glVertex2d(-0.5, -fSubnetSize/2);
					glVertex2d(-0.5, -0.5);
					glVertex2d(-fSubnetSize/2, -0.5);
					glVertex2d(-fSubnetSize/2, -fSubnetSize/2);
				}
			}
		}
		
		switch (status)
		{
		case STATUS_OK:
			break;
		case STATUS_WARNING:
			glColor4ubv(pRenderer->WarningHostColor());
			break;
		case STATUS_ALARM:
			glColor4ubv(pRenderer->AlarmHostColor());
			break;
		case STATUS_UNKNOWN:
			break;
		default:
			break;
		}

		if (status == STATUS_WARNING || status == STATUS_ALARM)
		{
			glVertex2d(-fHostSize/2-fHostFrameWidth, -fHostSize/2-fHostFrameWidth);
			glVertex2d(-fHostSize/2-fHostFrameWidth, +fHostSize/2+fHostFrameWidth);
			glVertex2d(+fHostSize/2+fHostFrameWidth, +fHostSize/2+fHostFrameWidth);
			glVertex2d(+fHostSize/2+fHostFrameWidth, -fHostSize/2-fHostFrameWidth);
		}
		
		if (m_pHost->IsSelected())
			glColor4ubv(pRenderer->SelectedHostColor());
		else
			glColor4ubv(pRenderer->HostColor());
		glVertex2d(-fHostSize/2, -fHostSize/2);
		glVertex2d(-fHostSize/2, +fHostSize/2);
		glVertex2d(+fHostSize/2, +fHostSize/2);
		glVertex2d(+fHostSize/2, -fHostSize/2);
	glEnd();

	if (status == STATUS_WARNING || status == STATUS_ALARM)
	{
		if (status == STATUS_WARNING)
			glColor4ubv(pRenderer->WarningHostColor());
		else if (status == STATUS_ALARM)
			glColor4ubv(pRenderer->AlarmHostColor());

		glBegin(GL_LINE_LOOP);
			glVertex2d(-fHostSize/2, -fHostSize/2);
			glVertex2d(-fHostSize/2, +fHostSize/2);
			glVertex2d(+fHostSize/2, +fHostSize/2);
			glVertex2d(+fHostSize/2, -fHostSize/2);
		glEnd();
	}

	glColor4ubv(pRenderer->HostTextColor());
	pRenderer->WriteHostIPAddress(m_pHost->GetIpAddressString(), m_IpAddressDisplayContext);
	if (m_pHost->GetGivenName()[0])
		pRenderer->WriteHostName(m_pHost->GetGivenName(), m_NameDiusplayContext);
	else
		pRenderer->WriteHostName(m_pHost->GetName(), m_NameDiusplayContext);
	
	pRenderer->RestoreModelViewMatrix();
}
	
int CPlaneHost::GetWanConnectionCount()
{
	return m_pHost->GetWanConnectionCount();
}

CHost *CPlaneHost::GetWanConnection(int i)
{
	return m_pHost->GetWanConnection(i);
}

int CPlaneHost::GetHardwareConnectionCount()
{
	return m_pHost->GetHardwareConnectionCount();
}

CHost *CPlaneHost::GetHardwareConnection(int i)
{
	return m_pHost->GetHardwareConnection(i);
}

CNetObject *CPlaneHost::GetNetObject(double x, double y)
{
	if (x < (m_iCol - 0.5)) return NULL;
	if (x > (m_iCol + 0.5)) return NULL;
	if (y < (m_iRow - 0.5)) return NULL;
	if (y > (m_iRow + 0.5)) return NULL;

	if (x > (m_iCol - fHostSize/2) && x < (m_iCol + fHostSize/2) &&
		y > (m_iRow - fHostSize/2) && y < (m_iRow + fHostSize/2))
		return m_pHost;

	return m_pHost->GetSubnet();
}

/*LOGICAL CPlaneHost::AcceptBuddy(CPlaneHost *pBuddy)
{
	if (!m_pNBuddy) // Northbound neighbour
	{
		m_pNBuddy = pBuddy;
		pBuddy->m_pSBuddy = this;
		pBuddy->iRow = iRow+1;
		pBuddy->iCol = iCol;
		return LOGICAL_TRUE;
	}
	else if (!m_pSBuddy) // Southbound neighbour
	{
		m_pSBuddy = pBuddy;
		pBuddy->m_pNBuddy = this;
		pBuddy->iRow = iRow-1;
		pBuddy->iCol = iCol;
		return LOGICAL_TRUE;
	}
	else if (!m_pWBuddy) // Westbound neighbour
	{
		m_pWBuddy = pBuddy;
		pBuddy->m_pEBuddy = this;
		pBuddy->iRow = iRow;
		pBuddy->iCol = iCol-1;
		return LOGICAL_TRUE;
	}
	else if (!m_pEBuddy) // Eastbound neighbour
	{
		m_pEBuddy = pBuddy;
		pBuddy->m_pWBuddy = this;
		pBuddy->iRow = iRow;
		pBuddy->iCol = iCol+1;
		return LOGICAL_TRUE;
	}

	return LOGICAL_FALSE;
}*/

///////////////////////////////////////////////////////////////////////////
// CPlaneSubnet

CPlaneSubnet::CPlaneSubnet(CSubnet *pSubnet)
{
	m_pSubnet = pSubnet;
}

//virtual
CPlaneSubnet::~CPlaneSubnet()
{
}

void CPlaneSubnet::AddHost(CPlaneHost *pPlaneHost)
{
	m_Hosts.push_back(pPlaneHost);
}

CPlaneHost **AllocMemory(int W, int E, int N, int S)
{
	TRACE("Alloc\n");
	return (CPlaneHost **)calloc((E-W+1)*(N-S+1), sizeof(CPlaneHost *));
}

///////////////////////////////////////////////////////////////////////////
// CWorldPlaneRenderer

CWorldPlaneRenderer::CWorldPlaneRenderer(HWND hWnd) : COpenGL_2D(hWnd)
{
	m_West = m_East = m_North = m_South = 0;
	m_ppHostMatrix = AllocMemory(m_West, m_East, m_North, m_South);
	m_MouseClickDragType = MouseClickDrag_Select;
	m_pSelectedObject = NULL;
	m_pGui = NULL;
	m_nIdleID = 0;

	m_NameRect.top = 0;
	m_NameRect.bottom = -fHostSize/4;
	m_NameRect.left = -fHostSize/2*0.95;
	m_NameRect.right = +fHostSize/2*0.95;

	m_IPAddressRect.top = -fHostSize/4;
	m_IPAddressRect.bottom = -fHostSize/2;
	m_IPAddressRect.left = -fHostSize/2*0.9;
	m_IPAddressRect.right = +fHostSize/2*0.9;
	m_bAntialiasingEnabled = FALSE;
}

CWorldPlaneRenderer::~CWorldPlaneRenderer()
{
	for (int iHost = 0; iHost < m_Hosts.size(); iHost++)
	{
		CPlaneHost *pPlaneHost = m_Hosts[iHost];
		delete pPlaneHost;
	}
	free(m_ppHostMatrix);
}


//virtual
void CWorldPlaneRenderer::OnRenderScene()
{
	CStackCriticalSection section(&m_CriticalSection);

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
	for (CListOfPlaneHosts::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		CPlaneHost *pPlaneHost = *it;
		pPlaneHost->Render(this);
	}

	// WAN connections
	glColor4ub(255, 255, 0, 0);
	glBegin(GL_LINES);
		for (it = m_Hosts.begin(); it != m_Hosts.end(); it++)
		{
			CPlaneHost *pPlaneHost1 = *it;
			int nConnections = pPlaneHost1->GetWanConnectionCount();
			for (int i = 0; i < nConnections; i++)
			{
				CHost *pConnectedHost = pPlaneHost1->GetWanConnection(i);
				CPlaneHost *pPlaneHost2 = FindPlaneHost(pConnectedHost);

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
		}
	glEnd();
	
	// WAN connections
	glColor4ub(255, 128, 0, 0);
	glBegin(GL_LINES);
		for (it = m_Hosts.begin(); it != m_Hosts.end(); it++)
		{
			CPlaneHost *pPlaneHost1 = *it;
			int nConnections = pPlaneHost1->GetHardwareConnectionCount();
			for (int i = 0; i < nConnections; i++)
			{
				CHost *pConnectedHost = pPlaneHost1->GetHardwareConnection(i);
				CPlaneHost *pPlaneHost2 = FindPlaneHost(pConnectedHost);

				double x1 = pPlaneHost1->GetColumn();
				double y1 = pPlaneHost1->GetRow();
				double x2 = pPlaneHost2->GetColumn();
				double y2 = pPlaneHost2->GetRow();

				glVertex2d(x1, y1);
				glVertex2d(x2, y2);
			}
		}
	glEnd();
//	COpenGL_2D::OnRenderScene();
}

//virtual
void CWorldPlaneRenderer::OnInitialized()
{
	m_VectorFontListBaseID = CreateVectorFont("Times New", 0, m_GlyphMetrics);
}

//virtual
void CWorldPlaneRenderer::OnSetDefaultOptions()
{
}

LOGICAL CWorldPlaneRenderer::IsAnyNeighbourOfTheSameSubnet(int iCol, int iRow, CHost *pHost)
{
	return (pHost->EqualSubnets(GetHostAt(iCol, iRow-1)) ||
			pHost->EqualSubnets(GetHostAt(iCol, iRow+1)) ||
			pHost->EqualSubnets(GetHostAt(iCol-1, iRow)) ||
			pHost->EqualSubnets(GetHostAt(iCol+1, iRow)) );

}

void CWorldPlaneRenderer::HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet)
{
	m_CriticalSection.Enter();
	ASSERT(pHost->GetSubnet());
	CPlaneHost *pPlaneHost = FindPlaneHost(pHost);
	ASSERT(pPlaneHost);
	if (!pPlaneHost)
		pPlaneHost = AddHost(pHost);

	if (!IsAnyNeighbourOfTheSameSubnet(pPlaneHost->m_iCol, pPlaneHost->m_iRow, pHost))
	{ // no any neighbour from the same subnet -> need to find another place
		LOGICAL bNotTheOnlyInSubnet = LOGICAL_FALSE;
		for (CListOfPlaneHosts::iterator p = m_Hosts.begin(); p != m_Hosts.end(); p++)
		{
			CPlaneHost *pAnotherPlaneHost = *p;
			if (pAnotherPlaneHost == pPlaneHost)
				continue;
			if (!pAnotherPlaneHost->GetHost()->EqualSubnets(pHost))
				continue;
			bNotTheOnlyInSubnet = LOGICAL_TRUE;
			break;
		}
		if (bNotTheOnlyInSubnet)
		{
			int iCol, iRow;
			LOGICAL bFound = FindPlaceToInsert(pHost, iCol, iRow, SKIP_OCCUPIED_FLAG | SKIP_OUT_OF_SUBNET_FLAG | SKIP_THE_SAME_SUBNET_FLAG);
			/*if (!bFound)
			{
				bFound = FindPlaceToInsert(pHost, iCol, iRow, SKIP_OUT_OF_SUBNET_FLAG | SKIP_THE_SAME_SUBNET_FLAG);
				if (!bFound)
				{
					ASSERT(FALSE);
				}
			}*/
			if (bFound)
			{
				FreeOccupiedPlace(pPlaneHost);
				PutPlaneHostAt(pPlaneHost, iCol, iRow);
			}
		}
	}
	m_CriticalSection.Leave();
	Redraw();	
}

void CWorldPlaneRenderer::HostChanged(CHost *pHost)
{
	CPlaneHost *pPlaneHost = FindPlaneHost(pHost);
	if (pPlaneHost)
		pPlaneHost->ContextChanged();
	Redraw();	
}

void CWorldPlaneRenderer::KickOut(CPlaneHost *pObject, CPlaneHost *pSubject, double &DeltaEnergy)
{
	CPlaneHost **ppPlace = GetPlaceForPlaneHostAt(pObject->m_iCol, pObject->m_iRow);
	ASSERT(ppPlace);
	*ppPlace = pSubject; // kick-out it

	double Energy, MinEnergy = DeltaEnergy;
	int BestCol, BestRow;
	LOGICAL bFound = LOGICAL_FALSE;

	for (int i = m_West-1; i <= (m_East+1); i++)
	{
		for (int j = m_South-1; j <= (m_North+1); j++)
		{
			if (!IsAnyNeighbourOfTheSameSubnet(i, j, pObject->GetHost()))
				continue;			

			Energy = EnergyAt(pObject->GetHost(), i, j);
			CHost *pExistingHost = GetHostAt(i, j);
			if (pExistingHost)
				Energy -= EnergyAt(pExistingHost, i, j);
			if (Energy < MinEnergy)
			{
				bFound = LOGICAL_TRUE;
				MinEnergy = Energy;
				BestCol = i;
				BestRow = j;
			}
		}
	}
	if (bFound)
	{
		DeltaEnergy = MinEnergy;
		CPlaneHost *pNewPlaneHost = GetPlaneHostAt(BestCol, BestRow);
		if (pNewPlaneHost)
		{ // kick it out
			KickOut(pNewPlaneHost, pObject, DeltaEnergy);
		}
		else
		{
			PutPlaneHostAt(pObject, BestCol, BestRow);
		}
	}
	else
	{
		PutPlaneHostAtAnyLocation(pObject);
	}

}

void CWorldPlaneRenderer::PutPlaneHostAtAnyLocation(CPlaneHost *pPlaneHost)
{
	// Find location to place new host at
	int BestCol, BestRow;
	FindPlaceToInsert(pPlaneHost->GetHost(), BestCol, BestRow, SKIP_OCCUPIED_FLAG);
	PutPlaneHostAt(pPlaneHost, BestCol, BestRow);
}

void CWorldPlaneRenderer::PutPlaneHostAt(CPlaneHost *pPlaneHost, int iCol, int iRow)
{
	if (iCol < m_West || iCol > m_East || iRow < m_South || iRow > m_North)
		ExtendWorld();

	CPlaneHost **ppPlaceToInsert = GetPlaceForPlaneHostAt(iCol, iRow);
	ASSERT(*ppPlaceToInsert == NULL);
	*ppPlaceToInsert = pPlaneHost;
	pPlaneHost->SetLocation(iCol, iRow);
	TRACE("Host %s placed at (%d, %d)\n", pPlaneHost->GetHost()->GetName(), pPlaneHost->m_iCol, pPlaneHost->m_iRow);
}

void CWorldPlaneRenderer::FreeOccupiedPlace(CPlaneHost *pPlaneHost)
{
	CPlaneHost **ppPlaceToInsert = GetPlaceForPlaneHostAt(pPlaneHost->m_iCol, pPlaneHost->m_iRow);
	ASSERT(*ppPlaceToInsert != NULL);
	*ppPlaceToInsert = NULL;
}

void CWorldPlaneRenderer::MovePlaneHost(CPlaneHost *pPlaneHost, int iCol, int iRow)
{
	FreeOccupiedPlace(pPlaneHost);
	PutPlaneHostAt(pPlaneHost, iCol, iRow);
	Redraw();
}

void CWorldPlaneRenderer::SwapPlaneHosts(CPlaneHost *pPlaneHost1, CPlaneHost *pPlaneHost2)
{
	FreeOccupiedPlace(pPlaneHost1);
	FreeOccupiedPlace(pPlaneHost2);
	int iCol = pPlaneHost1->m_iCol;
	int iRow = pPlaneHost1->m_iRow;
	PutPlaneHostAt(pPlaneHost1, pPlaneHost2->m_iCol, pPlaneHost2->m_iRow);
	PutPlaneHostAt(pPlaneHost2, iCol, iRow);
	Redraw();
}

LOGICAL CWorldPlaneRenderer::FindPlaceToInsert(CHost *pHost, int &BestCol, int &BestRow, long uFlags)
{
	double Energy, MinEnergy = VERY_BIG;
	LOGICAL bFound = LOGICAL_FALSE;
	for (int i = m_West-1; i <= m_East+1; i++)
	{
		for (int j = m_South-1; j <= m_North+1; j++)
		{
			CPlaneHost *pPlaneHost = GetPlaneHostAt(i, j);
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
				if (pPlaneHost && pHost->EqualSubnets(pPlaneHost->GetHost()))
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

void CWorldPlaneRenderer::ExtendWorld()
{
	int West = m_West-1;
	int East = m_East+1;
	int nSizeWE = East-West+1;
	int South = m_South-1;
	int North = m_North+1;
	CPlaneHost **ppHostMatrix = AllocMemory(West, East, North, South); 
	for (int i = m_West; i <= m_East; i++)
	{
		for (int j = m_South; j <= m_North; j++)
		{
			//int shift = nSizeNS*i + j - South - West*nSizeWE;
			int shift = GetShift(i, j, West, East, North, South);
			CPlaneHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	m_West = West;
	m_East = East;
	m_South = South;
	m_North = North;
}

CPlaneHost *CWorldPlaneRenderer::AddHost(CHost *pHost)
{
	int iRow, iCol;
	
	m_CriticalSection.Enter();
	FindPlaceToInsert(pHost, iCol, iRow, SKIP_OCCUPIED_FLAG);
	CPlaneHost *pNewPlaneHost = new CPlaneHost(pHost, this, iCol, iRow);
	PutPlaneHostAt(pNewPlaneHost, iCol, iRow);
	m_Hosts.push_back(pNewPlaneHost);
	m_CriticalSection.Leave();

	Redraw();
	return pNewPlaneHost;
}

CPlaneHost **CWorldPlaneRenderer::GetPlaceForPlaneHostAt(int iCol, int iRow)
{
	if (iCol < m_West || iCol > m_East)
		return NULL; // nothing outside boundaries
	if (iRow < m_South || iRow > m_North)
		return NULL; // nothing outside boundaries

	int shift = GetShift(iCol, iRow, m_West, m_East, m_North, m_South);

	return m_ppHostMatrix+shift;
}

CPlaneHost *CWorldPlaneRenderer::GetPlaneHostAt(int iCol, int iRow)
{
	CPlaneHost **ppPlaceForHost = GetPlaceForPlaneHostAt(iCol, iRow);
	if (!ppPlaceForHost)
		return NULL;
	return *ppPlaceForHost;
}

CPlaneHost *CWorldPlaneRenderer::FindPlaneHost(CHost *pHost)
{
	for (int iHost = 0; iHost < m_Hosts.size(); iHost++)
	{
		CPlaneHost *pPlaneHost = m_Hosts[iHost];
		if (pPlaneHost->GetHost() == pHost)
			return pPlaneHost;
	}

	return NULL;
}

void CWorldPlaneRenderer::ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged)
{
	if (pWhereChanged == this)
		return;
	SelectObject(pObject);
}

unsigned char *CWorldPlaneRenderer::HostColor()
{
	return m_pGui->PlaneView_HostColor();
}

unsigned char *CWorldPlaneRenderer::SelectedHostColor()
{
	return m_pGui->PlaneView_SelectedHostColor();
}

unsigned char *CWorldPlaneRenderer::HostTextColor()
{
	return m_pGui->PlaneView_HostTextColor();
}

unsigned char *CWorldPlaneRenderer::WarningHostColor()
{
	return m_pGui->PlaneView_WarningHostColor();
}

unsigned char *CWorldPlaneRenderer::AlarmHostColor()
{
	return m_pGui->PlaneView_AlarmHostColor();
}

unsigned char *CWorldPlaneRenderer::SubnetColor()
{
	return m_pGui->PlaneView_SubnetColor();
}

unsigned char *CWorldPlaneRenderer::SelectedSubnetColor()
{
	return m_pGui->PlaneView_SelectedSubnetColor();
}

void CWorldPlaneRenderer::ZoomIn()
{
	IncreaseZoom(2);
}

void CWorldPlaneRenderer::ZoomOut()
{
	IncreaseZoom(0.5);
}

double CWorldPlaneRenderer::GravityAt(int iCol, int iRow)
{
	double Kx = (double)m_WindowHeight/(m_WindowHeight+m_WindowWidth);
	double Ky = 1. - Kx;
	double G = 0.8*max(Kx*abs(iCol), Ky*abs(iRow)) + 0.2*sqrt(Kx*iCol*iCol + Ky*iRow*iRow);
	//G += 0.05*(iCol-m_West+iRow-m_South); // move everything to west and south
	return G;
}

double CWorldPlaneRenderer::ChemicalInteraction(CHost *pHost, CHost *pNeighbour)
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

double CWorldPlaneRenderer::EnergyAt(CHost *pHost, int iCol, int iRow)
{
	const double NameSortPenalty = 0.05;
	const double fWanElasticity = 0.3;
	const double fLanElasticity = 0.3;
	const double fHardwareElasticity = 1.0;

	double m = 1;
	if (pHost->GetSubnet())
		m *= 2; // host belonging to any subnet are heavier
	if (pHost->IsRouter())
		m *= 0.5; // router must tend to world boundary
	double E = m*GravityAt(iCol, iRow);

	CHost *pNeighbour = GetHostAt(iCol-1, iRow);
	E += ChemicalInteraction(pHost, pNeighbour);

	pNeighbour = GetHostAt(iCol+1, iRow);
	E += ChemicalInteraction(pHost, pNeighbour);

	pNeighbour = GetHostAt(iCol, iRow-1);
	E += ChemicalInteraction(pHost, pNeighbour);

	pNeighbour = GetHostAt(iCol, iRow+1);
	E += ChemicalInteraction(pHost, pNeighbour);

	// Wan connections
	int nConnections = pHost->GetWanConnectionCount();
	for (int i = 0; i < nConnections; i++)
	{
		CHost *pConnectedHost = pHost->GetWanConnection(i);
		CPlaneHost *pPlaneHost2 = FindPlaneHost(pConnectedHost);
		int dx = iCol-pPlaneHost2->GetColumn();
		int dy = iRow-pPlaneHost2->GetRow();
		double distance2 = dx*dx+dy*dy;
		E += fWanElasticity * distance2 * (pHost->GetWanConnectionCount() + pPlaneHost2->GetWanConnectionCount())/2;
	}
	
	// Hardware connections
	nConnections = pHost->GetHardwareConnectionCount();
	for (i = 0; i < nConnections; i++)
	{
		CHost *pConnectedHost = pHost->GetHardwareConnection(i);
		CPlaneHost *pPlaneHost2 = FindPlaneHost(pConnectedHost);
		int dx = iCol-pPlaneHost2->GetColumn();
		int dy = iRow-pPlaneHost2->GetRow();
		double distance2 = dx*dx+dy*dy;
		E += fHardwareElasticity * distance2;
	}
	
	// Attraction to subnet
	CSubnet *pSubnet = pHost->GetSubnet();
	if (pSubnet)
	{
		for (CListOfPlaneHosts::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
		{
			CPlaneHost *pPlaneHost = *it;
			if (pPlaneHost->GetHost()->EqualSubnets(pHost))
			{
				E += fLanElasticity*(abs(pPlaneHost->GetColumn() - iCol)+abs(pPlaneHost->GetRow() - iRow));
			}
		}
	}
	
	return E;
}

double CWorldPlaneRenderer::Energy(CPlaneHost *pPlaneHost)
{
	return EnergyAt(pPlaneHost->GetHost(), pPlaneHost->m_iCol, pPlaneHost->m_iRow);	
}

void CWorldPlaneRenderer::SelectObject(CNetObject *pObject)
{
	if (m_pSelectedObject)
		m_pSelectedObject->BeSelected(LOGICAL_FALSE);
	pObject->BeSelected(LOGICAL_TRUE);
	m_pSelectedObject = pObject;
	Redraw();
}

//virtual
void CWorldPlaneRenderer::OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y)
{
	if (IsInSelectMode())
	{
		CNetObject *pObject = GetObjectAt(x, y);
		if (!pObject)
			return;
		SelectObject(pObject);
		if (m_pGui)
			m_pGui->ObjectSelectionChanged(pObject, this);
	}
}

CNetObject *CWorldPlaneRenderer::GetObjectAt(int Xscreen, int Yscreen)
{
	double Xworld = ScreenToWorldX(Xscreen);
	double Yworld = ScreenToWorldY(Yscreen);
	for (int i = 0; i < m_Hosts.size(); i++)
	{
		CPlaneHost *pPlaneHost = m_Hosts[i];
		CNetObject *pNetObject = pPlaneHost->GetNetObject(Xworld, Yworld);
		if (pNetObject)
			return pNetObject;
	}
	return NULL;
}

CHost *CWorldPlaneRenderer::GetHostAt(int iCol, int iRow)
{
	CPlaneHost *pPlaneHost = GetPlaneHostAt(iCol, iRow);
	if (!pPlaneHost)
		return NULL;
	return pPlaneHost->GetHost();
}

LOGICAL CWorldPlaneRenderer::OnIdleProcess()
{
	if (IsLDragging())
		return LOGICAL_FALSE;
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
		Redraw();

	return bSomethingChanged;
}

void CWorldPlaneRenderer::WriteHostIPAddress(char *pszIpAddressString, STextDisplayContext &Context)
{
	WriteText(pszIpAddressString, m_IPAddressRect, ALIGN_HOR_CENTER | ALIGN_VERT_CENTER, Context);
}

void CWorldPlaneRenderer::WriteHostName(char *pszHostName, STextDisplayContext &Context)
{
	WriteText(pszHostName, m_NameRect, ALIGN_HOR_CENTER | ALIGN_VERT_CENTER, Context);
}

void CWorldPlaneRenderer::WriteText(char *pszText, STextRect &rect, unsigned int alignment, STextDisplayContext &Context)
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

LOGICAL CWorldPlaneRenderer::RandomMove()
{
	int iCol2, iRow2;
	CStackCriticalSection section(&m_CriticalSection);

	CPlaneHost *pPlaneHost1 = RandomPlaneHost();
	CPlaneHost *pPlaneHost2 = NULL;
	LOGICAL bFound = LOGICAL_FALSE;
	if (pPlaneHost1)
	{
		double E1 = Energy(pPlaneHost1);
		for (int i = m_West-1; i <= m_East+1; i++)
		{
			for (int j = m_South-1; j <= m_North+1; j++)
			{
				pPlaneHost2 = GetPlaneHostAt(i, j);
				if (pPlaneHost2)
					continue;
				if (pPlaneHost2 == pPlaneHost1)
					continue;
				double E2 = EnergyAt(pPlaneHost1->GetHost(), i, j);
				double DeltaE = E2-E1;
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
		}
	}
	return bFound;
}

LOGICAL CWorldPlaneRenderer::RandomSwap()
{
	CStackCriticalSection section(&m_CriticalSection);

	CPlaneHost *pPlaneHost1 = RandomPlaneHost();
	CPlaneHost *pPlaneHost2best = NULL;
	LOGICAL bFound = LOGICAL_FALSE;
	double DeltaEbest = 0;
	if (pPlaneHost1)
	{
		double E1 = Energy(pPlaneHost1);
		for (int i = m_West; i <= m_East; i++)
		{
			for (int j = m_South; j <= m_North; j++)
			{
				CPlaneHost *pPlaneHost2 = GetPlaneHostAt(i, j);
				if (!pPlaneHost2)
					continue;
				if (pPlaneHost2 == pPlaneHost1)
					continue;
				double Ebefore = E1 + Energy(pPlaneHost2);
				double Eafter = EnergyAt(pPlaneHost1->GetHost(), i, j) + 
					EnergyAt(pPlaneHost2->GetHost(), pPlaneHost1->m_iCol, pPlaneHost1->m_iRow);
				double DeltaE = Eafter-Ebefore;
				if (DeltaE < (DeltaEbest-0.001))
				{
					bFound = LOGICAL_TRUE;
					DeltaEbest = DeltaE;
					pPlaneHost2best = pPlaneHost2;
				}
			}

		}
		if (bFound)
		{
			SwapPlaneHosts(pPlaneHost1, pPlaneHost2best);
		}
	}
	return bFound;
}

LOGICAL CWorldPlaneRenderer::RemoveEmptyRowsCols()
{
	CStackCriticalSection section(&m_CriticalSection);
	// West
	LOGICAL bFound = LOGICAL_FALSE;
	for (int i = m_South; i <= m_North; i++)
	{
		if (GetPlaneHostAt(m_West, i) != NULL)
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

	// East
	bFound = LOGICAL_FALSE;
	for (i = m_South; i <= m_North; i++)
	{
		if (GetPlaneHostAt(m_East, i) != NULL)
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

	// South
	bFound = LOGICAL_FALSE;
	for (i = m_West; i <= m_East; i++)
	{
		if (GetPlaneHostAt(i, m_South) != NULL)
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

	// North
	bFound = LOGICAL_FALSE;
	for (i = m_West; i <= m_East; i++)
	{
		if (GetPlaneHostAt(i, m_North) != NULL)
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

	return LOGICAL_FALSE;
}

void CWorldPlaneRenderer::RemoveWesternCol()
{
	CPlaneHost **ppHostMatrix = AllocMemory(m_West+1, m_East, m_North, m_South);
	for (int i = m_West+1; i <= m_East; i++)
	{
		for (int j = m_South; j <= m_North; j++)
		{
			int shift = GetShift(i, j, m_West+1, m_East, m_North, m_South);
			ASSERT(shift >= 0);
			ASSERT(shift < (m_East-m_West)*(m_North-m_South+1));
			CPlaneHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	++m_West;
}

void CWorldPlaneRenderer::RemoveEasternCol()
{
	CPlaneHost **ppHostMatrix = AllocMemory(m_West, m_East-1, m_North, m_South);
	for (int i = m_West; i <= (m_East-1); i++)
	{
		for (int j = m_South; j <= m_North; j++)
		{
			int shift = GetShift(i, j, m_West, m_East-1, m_North, m_South);
			ASSERT(shift >= 0);
			ASSERT(shift < (m_East-m_West)*(m_North-m_South+1));
			CPlaneHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	--m_East;
}

void CWorldPlaneRenderer::RemoveSouthernRow()
{
	CPlaneHost **ppHostMatrix = AllocMemory(m_West, m_East, m_North, m_South+1);
	for (int i = m_West; i <= m_East; i++)
	{
		for (int j = m_South+1; j <= m_North; j++)
		{
			int shift = GetShift(i, j, m_West, m_East, m_North, m_South+1);
			ASSERT(shift >= 0);
			ASSERT(shift < (m_East-m_West+1)*(m_North-m_South));
			CPlaneHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	++m_South;
}

void CWorldPlaneRenderer::RemoveNorthernRow()
{
	CPlaneHost **ppHostMatrix = AllocMemory(m_West, m_East, m_North-1, m_South);
	for (int i = m_West; i <= m_East; i++)
	{
		for (int j = m_South; j <= (m_North-1); j++)
		{
			int shift = GetShift(i, j, m_West, m_East, m_North-1, m_South);
			ASSERT(shift >= 0);
			ASSERT(shift < (m_East-m_West+1)*(m_North-m_South));
			CPlaneHost **pPlaceForPlaneHost = GetPlaceForPlaneHostAt(i,j);
			ppHostMatrix[shift] = *pPlaceForPlaneHost;
		}
	}

	free(m_ppHostMatrix);
	m_ppHostMatrix = ppHostMatrix;

	--m_North;
}

CPlaneHost *CWorldPlaneRenderer::RandomPlaneHost()
{
	CPlaneHost *pPlaneHost = NULL;
	for (CListOfPlaneHosts::iterator it = m_Hosts.begin(); it != m_Hosts.end(); it++)
	{
		pPlaneHost = *it;
		if (pPlaneHost->GetNeighbourCount() == 0)
			return pPlaneHost;
	}

	while (1)
	{
		int size = m_East-m_West + 1;
		int iCol = m_West + (rand() % size);
		size = m_North-m_South + 1;
		int iRow = m_South + (rand() % size);
		pPlaneHost = GetPlaneHostAt(iCol, iRow);
		if (pPlaneHost)
		{
			if ((portableGetCurrentTimeMsec() % 3) && (pPlaneHost->GetNeighbourCount() == 4))
				continue;
			else
				break;
		}
	}
	
	return pPlaneHost;
}
