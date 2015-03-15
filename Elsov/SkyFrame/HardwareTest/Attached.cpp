#include "stdafx.h"
#include "Attached.h"

// Constructors-destructors
CAttachedBlock::CAttachedBlock(int ViewIndex, CDevice *pDevice)
{
	m_ViewIndex=ViewIndex;
	m_pDevice=pDevice;
	m_DeviceNumber=0;
}

CAttachedBlock::~CAttachedBlock()
{
}
