/* GuiAPI.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	17 Mar 2006 - initial creation
	22 Nov 2006 - database creation

*/

#ifndef __GUI_API_H_INCLUDED__
#define __GUI_API_H_INCLUDED__

#include "Portable.h"

// Forward declarations
class CNetObject;
class CHost;
class CSubnet;
class IDatabaseAPI;

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

	virtual void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged) = 0;
	
	// NetObject configuration and context menu
	virtual LOGICAL HostContextMenu(int ScreenX, int ScreenY, CHost *pHost) = 0;
	virtual LOGICAL SubnetContextMenu(int ScreenX, int ScreenY, CSubnet *pSubnet) = 0;

	// Colors
	virtual unsigned char *PlaneView_HostColor() = 0;
	virtual unsigned char *PlaneView_SelectedHostColor() = 0;
	virtual unsigned char *PlaneView_HostTextColor() = 0;
	virtual unsigned char *PlaneView_WarningHostColor() = 0;
	virtual unsigned char *PlaneView_AlarmHostColor() = 0;
	virtual unsigned char *PlaneView_SubnetColor() = 0;
	virtual unsigned char *PlaneView_SelectedSubnetColor() = 0;
};

#endif //__GUI_API_H_INCLUDED__
