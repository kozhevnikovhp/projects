#include "stdafx.h"
#include "Trees.h"

CSimpleNode::CSimpleNode()
{
	m_pParentNode=NULL;
	m_pData=NULL;
	m_CurrentPosition=NULL;
}

CSimpleNode::~CSimpleNode()
{
	Clear();
}

void CSimpleNode::Clear()
{
	m_NodeList.Clear();
	m_pParentNode=NULL;
	m_pData=NULL;
}

void *CSimpleNode::SetData(void *pData)
{
	m_pData=pData;
	return m_pData;
}

CSimpleNode *CSimpleNode::AddNode()
{
	CSimpleNode *pNode=New();
	AddChildToTail(pNode);
	return pNode;
}

CSimpleNode *CSimpleNode::AddChildToTail(CSimpleNode *pNode)
{
	m_NodeList.AddTail(pNode);
	pNode->m_pParentNode=this;
	return pNode;
}

CSimpleNode *CSimpleNode::AddChildToHead(CSimpleNode *pNode)
{
	m_NodeList.AddHead(pNode);
	pNode->m_pParentNode=this;
	return pNode;
}

CSimpleNode *CSimpleNode::HeadNode()
{
	m_CurrentPosition=m_NodeList.GetHeadPosition();
	if (!m_CurrentPosition) return NULL;
	CSimpleNode *pNode=(CSimpleNode *)m_NodeList.GetAt(m_CurrentPosition);
	return pNode;
}

CSimpleNode *CSimpleNode::TailNode()
{
	m_CurrentPosition=m_NodeList.GetTailPosition();
	if (!m_CurrentPosition) return NULL;
	CSimpleNode *pNode=(CSimpleNode *)m_NodeList.GetAt(m_CurrentPosition);
	return pNode;
}

CSimpleNode *CSimpleNode::NextNode()
{
	if (!m_CurrentPosition) return NULL;
	m_NodeList.GetNext(m_CurrentPosition);
	if (!m_CurrentPosition) return NULL;
	CSimpleNode *pNode=(CSimpleNode *)m_NodeList.GetAt(m_CurrentPosition);
	return pNode;
}

CSimpleNode *CSimpleNode::PrevNode()
{
	if (!m_CurrentPosition) return NULL;
	m_NodeList.GetPrev(m_CurrentPosition);
	if (!m_CurrentPosition) return NULL;
	CSimpleNode *pNode=(CSimpleNode *)m_NodeList.GetAt(m_CurrentPosition);
	return pNode;
}


