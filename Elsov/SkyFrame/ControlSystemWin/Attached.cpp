#include "stdafx.h"
#include "Attached.h"
#include "TargetInstance.h"


/////////////////////////////////////////////////////////////////////////////////
// class CAttachedBlock

CAttachedBlock::CAttachedBlock(int ViewIndex, CTargetInstance *pTargetInstance)
{
	m_ViewIndex = ViewIndex;
	m_pTargetInstance = pTargetInstance;
	m_pTarget = m_pTargetInstance->GetTarget();
}

//virtual
CAttachedBlock::~CAttachedBlock()
{
}


/////////////////////////////////////////////////////////////////////////////////
// class CAttachedBlockForObject

CAttachedBlockForObject::CAttachedBlockForObject(int ViewIndex, CTargetInstance *pTargetInstance, CControlObject *pControlObject)
	: CAttachedBlock(ViewIndex, pTargetInstance)
{
	m_pObject = pControlObject;
}

//virtual
CAttachedBlockForObject::~CAttachedBlockForObject()
{
}

//virtual
BOOL CAttachedBlockForObject::CanDropItem(CAttachedBlock *pDragged)
{
	return (EqualInstances(pDragged));
}

//virtual
void CAttachedBlockForObject::OnAfterDrop(CAttachedBlock *pDragged)
{
	pDragged->DropOn(*this);
}

//virtual
void CAttachedBlockForObject::DropOn(CAttachedBlockForObject &DropOn)
{
	m_pTargetInstance->SetObjectParent(m_pObject, DropOn.m_pObject);
}
