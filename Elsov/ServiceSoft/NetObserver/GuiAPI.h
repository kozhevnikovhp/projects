/* GuiAPI.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	17 Mar 2006 - initial creation
	22 Nov 2006 - database creation
	15 Nov 2007 - global IGuiAPI pointer instead of member variables
	10 Jan 2008 - host group created and changed functionality
*/

#ifndef __GUI_API_H_INCLUDED__
#define __GUI_API_H_INCLUDED__

#include "Portable.h"

// Forward declarations
class CNetObject;
class CHost;
class CSubnet;
class IDatabaseAPI;
class CGroup;

class IGuiAPI
{
// Public interface
public:
	// Database
	virtual IDatabaseAPI *OpenDatabase() = 0;
	
	// Events
	virtual void NewHostCreated(CHost *pHost) = 0;
	virtual void HostChanged(CHost *pHost) = 0;
	virtual void NewSubnetCreated(CSubnet *pSubnet) = 0;
	virtual void SubnetChanged(CSubnet *pSubnet) = 0;
	virtual void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet) = 0;
	virtual void HostRemovedFromSubnet(CHost *pHost, CSubnet *pSubnet) = 0;
	virtual void HostIgnored(CHost *pHost) = 0;
	virtual void HostHidden(CHost *pHost) = 0;
	virtual void IgnoredObjectsVisibilityChanged() = 0;
	virtual void HiddenObjectsVisibilityChanged() = 0;
	virtual void NewGroupCreated(CGroup *pGroup) = 0;
	virtual void GroupChanged(CGroup *pGroup) = 0;
	virtual void HostMovedBetweenGroups(CHost *pHost, CGroup *pOldGroup, CGroup *pNewGroup) = 0;

	virtual void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged) = 0;
	
	// NetObject configuration and context menu
	virtual LOGICAL HostContextMenu(int ScreenX, int ScreenY, CHost *pHost) = 0;
	virtual LOGICAL SubnetContextMenu(int ScreenX, int ScreenY, CSubnet *pSubnet) = 0;

	// Parameters
	virtual time_t GetTrafficStoreInterval() = 0;
	virtual unsigned char GetMaxDistanceInHops() = 0;
	virtual LOGICAL AreHiddenObjectsVisible() = 0;
	virtual LOGICAL AreIgnoredObjectsVisible() = 0;

	// Operations
	
};

extern IGuiAPI *g_pGui;

#endif //__GUI_API_H_INCLUDED__
