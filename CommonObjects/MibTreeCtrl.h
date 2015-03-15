/* MibTreeCtrl.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	20 Oct 2008 - initial creation

*/

#ifndef __MIB_TREE_CTRL_H_INCLUDED__
#define __MIB_TREE_CTRL_H_INCLUDED__

// Forward declarations
class CMibTree;
class CMibNode;

typedef void (*P_TREE_ITEM_CREATED_CALLBACK)(CTreeCtrl *pTreeCtrl, HTREEITEM hItem, CMibNode *pNode);
void LoadMibTreeToCtrl(CTreeCtrl *pTreeCtrl, CMibTree &Tree, P_TREE_ITEM_CREATED_CALLBACK pfnItemCreatedCallback = NULL);

#endif // __MIB_TREE_CTRL_H_INCLUDED__