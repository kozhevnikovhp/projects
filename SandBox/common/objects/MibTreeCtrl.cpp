/* MibTreeCtrl.cpp

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	20 Oct 2008 - initial creation

*/

#include "stdafx.h"
#include "MibTreeCtrl.h"
#include "MibParser.h"
#include <stack>

class CMibLoadToTreeCtrlNodeVisitor : public CMibTreeNodeVisitor
{
// Constructors/destructors
public:
	CMibLoadToTreeCtrlNodeVisitor(CTreeCtrl *pTreeCtrl, HTREEITEM hParentItem, P_TREE_ITEM_CREATED_CALLBACK pfnItemCreatedCallback);

// Public methods
public:

// Public overridables
public:
	virtual LOGICAL Visit(CMibNode *pNode);
	virtual void LowerLevel() { m_ItemStack.push(m_hCurrentItem); }
	virtual void UpperLevel() { m_ItemStack.pop(); }

// Protected overridables
protected:

// Protected members
protected:
	CTreeCtrl *m_pTreeCtrl;
	std::stack<HTREEITEM> m_ItemStack;
	HTREEITEM m_hCurrentItem;
	P_TREE_ITEM_CREATED_CALLBACK m_pfnItemCreatedCallback;
};

CMibLoadToTreeCtrlNodeVisitor::CMibLoadToTreeCtrlNodeVisitor(CTreeCtrl *pTreeCtrl, HTREEITEM hParentItem, P_TREE_ITEM_CREATED_CALLBACK pfnItemCreatedCallback)
{
	m_pTreeCtrl = pTreeCtrl;
	m_hCurrentItem = hParentItem;
	m_ItemStack.push(m_hCurrentItem);
	m_pfnItemCreatedCallback = pfnItemCreatedCallback;
}

//virtual
LOGICAL CMibLoadToTreeCtrlNodeVisitor::Visit(CMibNode *pNode)
{
	char *pszItemText = pNode->GetSymbolicName();
	if (strlen(pszItemText))
	{
		m_hCurrentItem = m_pTreeCtrl->InsertItem(pszItemText, 0, 0, m_ItemStack.top());
		if (m_pfnItemCreatedCallback)
			(*m_pfnItemCreatedCallback)(m_pTreeCtrl, m_hCurrentItem, pNode);
	}
	return LOGICAL_TRUE;
}

void LoadMibTreeToCtrl(CTreeCtrl *pTreeCtrl, CMibTree &Tree, P_TREE_ITEM_CREATED_CALLBACK pfnItemCreatedCallback/* = NULL*/)
{
	pTreeCtrl->DeleteAllItems();
	CMibLoadToTreeCtrlNodeVisitor visitor(pTreeCtrl, TVI_ROOT, pfnItemCreatedCallback);
	Tree.VisitAllNodes(&visitor);
}
