#include "stdafx.h"
#include "Attached.h"

	// Constructors-destructors
CAttachedBlock::CAttachedBlock(int ViewIndex)
{
	m_ViewIndex=ViewIndex;
	m_pStation=NULL;
}

CAttachedBlock::~CAttachedBlock()
{
}
