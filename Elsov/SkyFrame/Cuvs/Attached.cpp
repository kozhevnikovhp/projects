#include "stdafx.h"
#include "Station.h"

//static
CCuvsDoc *CAttachedBlock::m_pDoc = NULL;

// Constructors-destructors
CAttachedBlock::CAttachedBlock(int ViewIndex, CAbstractForm *pForm)
{
	m_ViewIndex=ViewIndex;
	m_pAbstractForm = pForm;
}

CAttachedBlock::~CAttachedBlock()
{
}

BOOL CAttachedBlock::CanDragItem()
{
	return m_pAbstractForm->CanDragItem(this);
}

BOOL CAttachedBlock::CanDropItem()
{
	return m_pAbstractForm->CanDropItem(this);
}

long CAttachedBlock::GetStationID()
{
	return m_pStation->GetStationID();
}
