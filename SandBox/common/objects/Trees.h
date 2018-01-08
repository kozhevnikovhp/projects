#ifndef __TREES_H__
#define __TREES_H__

#include "ObjLists.h"

class CSimpleNode : public CObject
{
	CSimpleNode *m_pParentNode;
	CListOfObjects m_NodeList;
	POSITION m_CurrentPosition;
// Methods
public:
	CSimpleNode();
	virtual ~CSimpleNode();
	inline CListOfObjects *GetNodeList() { return &m_NodeList; }

	CSimpleNode *AddNode();
	CSimpleNode *AddChildToTail(CSimpleNode *pNode);
	CSimpleNode *AddChildToHead(CSimpleNode *pNode);
	inline int HowManyChildren() { return m_NodeList.GetCount(); }
	inline BOOL HaveChildren() { return !m_NodeList.IsEmpty(); }

	// iterator
	CSimpleNode *HeadNode();
	CSimpleNode *TailNode();
	CSimpleNode *NextNode();
	CSimpleNode *PrevNode();

	virtual void Clear();
	virtual CSimpleNode *New() { return new CSimpleNode(); }

	inline void *GetData() { return m_pData; }
	virtual void *SetData(void *pData);

// Members
protected:
	inline CSimpleNode *GetParent() { return m_pParentNode; }
	inline void SetParent(CSimpleNode *pNode) { m_pParentNode=pNode; }
	void *m_pData;
};

#endif //__TREES_H__