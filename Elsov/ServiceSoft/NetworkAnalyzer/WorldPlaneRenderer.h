/* WorldPlaneRenderer.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	02 Mar 2007 - initial creation
	02 Apr 2007 - idle moving-swapping
	07 Apr 2007 - warning/alarm frame around host
	15 May 2007 - efficient idle processing, object name in status bar

*/

#ifndef __WORLD_PLANE_RENDERER_H_INCLUDED__
#define __WORLD_PLANE_RENDERER_H_INCLUDED__

#include "OpenGLRenderer.h"
#include <vector>

const double VERY_BIG = 1.0e+30;

// Forward declarations
class CPlaneSubnet;
class CWorldPlaneRenderer;
class CNetObject;
class CHost;
class CSubnet;
class IGuiAPI;

const double fHostSize = 0.75;
const double fHostFrameWidth = 0.03;
const double fSubnetSize = 0.85;

typedef struct
{
	double top, bottom;
	double left, right;
	double Width() { return (right - left); }
	double Height() { return (top - bottom); }
} STextRect;

const unsigned long ALIGN_LEFT			= 1 << 0;
const unsigned long ALIGN_RIGHT			= 1 << 1;
const unsigned long ALIGN_HOR_CENTER	= 1 << 2;
const unsigned long ALIGN_TOP			= 1 << 3;
const unsigned long ALIGN_BOTTOM		= 1 << 4;
const unsigned long ALIGN_VERT_CENTER	= 1 << 5;

typedef struct
{
	double ScaleX, ScaleY;
	double ShiftX, ShiftY;
	LOGICAL bInitialized;
} STextDisplayContext;

class CPlaneObject
{
//Constuctors/destructors
public:
	CPlaneObject();
	virtual ~CPlaneObject();

// Public methods
public:

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:

// Private methods
private:

// Private members
private:
};

class CPlaneHost : public CPlaneObject
{
	friend class CWorldPlaneRenderer;
//Constuctors/destructors
public:
	CPlaneHost(CHost *pHost, CWorldPlaneRenderer *pWorld, int iCol, int iRow);
	virtual ~CPlaneHost();

// Public methods
public:
	int GetNeighbourCount();
	int GetColumn() { return m_iCol; }
	int GetRow() { return m_iRow; }
	void ContextChanged() { m_IpAddressDisplayContext.bInitialized = m_NameDiusplayContext.bInitialized = LOGICAL_FALSE; }

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	void Render(CWorldPlaneRenderer *pRenderer);
	inline int GetWanConnectionCount();
	inline CHost *GetWanConnection(int i);
	inline int GetHardwareConnectionCount();
	inline CHost *GetHardwareConnection(int i);
	CNetObject *GetNetObject(double x, double y);
	CHost *GetHost() { return m_pHost; }
	void SetLocation(int iCol, int iRow) { m_iCol = iCol; m_iRow = iRow; }

// Protected overridables
protected:

// Protected members
protected:
	CHost *m_pHost;
	CPlaneSubnet *m_pPlaneSubnet;
	int m_iCol, m_iRow;
	CWorldPlaneRenderer *m_pWorld;
	STextDisplayContext m_IpAddressDisplayContext, m_NameDiusplayContext;
/*	CPlaneHost *m_pNBuddy; // Northbound neighbour
	CPlaneHost *m_pSBuddy; // Southbound neighbour
	CPlaneHost *m_pWBuddy; // Westbound neighbour
	CPlaneHost *m_pEBuddy; // Eastbound neighbour*/

// Private methods
private:

// Private members
private:
};


class CPlaneSubnet : public CPlaneObject
{
//Constuctors/destructors
public:
	CPlaneSubnet(CSubnet *pSubnet);
	virtual ~CPlaneSubnet();

// Public methods
public:
	CSubnet *GetSubnet() { return m_pSubnet; }
	void AddHost(CPlaneHost *pPlaneHost);

// Public overridables
public:

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	CSubnet *m_pSubnet;
	std::vector<CPlaneHost *> m_Hosts;

// Private methods
private:

// Private members
private:
};


enum MouseClickDragType {
	MouseClickDrag_Select,
	MouseClickDrag_ZoomRectangle,
	MouseClickDrag_DragScroll
};

typedef std::vector<CPlaneHost *> CListOfPlaneHosts;

const unsigned long SKIP_OCCUPIED_FLAG			= 1 << 0;
const unsigned long SKIP_OUT_OF_SUBNET_FLAG		= 1 << 1;
const unsigned long SKIP_THE_SAME_SUBNET_FLAG	= 1 << 2;

class CWorldPlaneRenderer : public COpenGL_2D
{
// Construction/destruction
public:
	CWorldPlaneRenderer(HWND hWnd);
	virtual ~CWorldPlaneRenderer();

// Public overridable methods
public:
	virtual void OnRenderScene();
	virtual void OnInitialized();
	virtual void OnSetDefaultOptions();

// Protected overridable methods
protected:
	virtual double GetWorldSizeX() { return 2+max((m_East-m_West), (m_North-m_South)); }
	virtual double GetWorldSizeY() { return GetWorldSizeX(); }
	virtual LOGICAL IsMouseMovePanMode() { return IsInDragScrollMode(); }
	virtual LOGICAL IsMouseMoveZoomMode() { return IsInZoomRectangleMode(); }
	// Event handlers
	virtual void OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y);

// Public methods
public:
	CPlaneHost *AddHost(CHost *pHost);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostChanged(CHost *pHost);

	CPlaneHost **GetPlaceForPlaneHostAt(int iCol, int iRow);
	CPlaneHost *GetPlaneHostAt(int iCol, int iRow);
	CHost *GetHostAt(int iCol, int iRow);
	CNetObject *GetObjectAt(int Xscreen, int Yscreen);
	void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);

	unsigned char *HostColor();
	unsigned char *SelectedHostColor();
	unsigned char *HostTextColor();
	unsigned char *WarningHostColor();
	unsigned char *AlarmHostColor();
	unsigned char *SubnetColor();
	unsigned char *SelectedSubnetColor();

	void SetGui(IGuiAPI *pGui) { m_pGui = pGui; }
	void ZoomIn();
	void ZoomOut();
	void SetSelectMode() { m_MouseClickDragType = MouseClickDrag_Select; }
	LOGICAL IsInSelectMode() { return m_MouseClickDragType == MouseClickDrag_Select; }
	void SetDragScrollMode() { m_MouseClickDragType = MouseClickDrag_DragScroll; }
	LOGICAL IsInDragScrollMode() { return m_MouseClickDragType == MouseClickDrag_DragScroll; }
	void SetZoomRectangleMode() { m_MouseClickDragType = MouseClickDrag_ZoomRectangle; }
	LOGICAL IsInZoomRectangleMode(){ return m_MouseClickDragType == MouseClickDrag_ZoomRectangle; }
	LOGICAL OnIdleProcess();
	void WriteHostIPAddress(char *pszIpAddressString, STextDisplayContext &Context);
	void WriteHostName(char *pszHostName, STextDisplayContext &Context);
	LOGICAL AntialiasingEnabled() { return m_bAntialiasingEnabled; }
	void ToggleAntialiasing() { m_bAntialiasingEnabled = !m_bAntialiasingEnabled; Redraw(); }

// Protected methods
protected:
	CPlaneHost *FindPlaneHost(CHost *pHost);
	double GravityAt(int iCol, int iRow);
	double ChemicalInteraction(CHost *pHost, CHost *pNeighbour);
	double EnergyAt(CHost *pHost, int iCol, int iRow);
	double Energy(CPlaneHost *pPlaneHost);
	void SelectObject(CNetObject *pObject);
	LOGICAL IsAnyNeighbourOfTheSameSubnet(int iCol, int iRow, CHost *pHost);
	void KickOut(CPlaneHost *pObject, CPlaneHost *pSubject, double &DeltaEnergy);
	void PutPlaneHostAtAnyLocation(CPlaneHost *pPlaneHost);
	void PutPlaneHostAt(CPlaneHost *pPlaneHost, int iCol, int iRow);
	void FreeOccupiedPlace(CPlaneHost *pPlaneHost);
	void MovePlaneHost(CPlaneHost *pPlaneHost, int iCol, int iRow);
	void SwapPlaneHosts(CPlaneHost *pPlaneHost1, CPlaneHost *pPlaneHost2);
	LOGICAL RandomMove();
	LOGICAL RandomSwap();
	LOGICAL RemoveEmptyRowsCols();
	void RemoveWesternCol();
	void RemoveEasternCol();
	void RemoveSouthernRow();
	void RemoveNorthernRow();
	CPlaneHost *RandomPlaneHost();
	void WriteText(char *pszText, STextRect &rect, unsigned int alignment, STextDisplayContext &Context);

// Private methods
private:
	LOGICAL FindPlaceToInsert(CHost *pHost, int &BestCol, int &BestRow, long uFlags);
	void ExtendWorld();

// Public members
public:

// Protected members
protected:
	CPortableCriticalSection m_CriticalSection;
	CListOfPlaneHosts m_Hosts;
	int m_West, m_East, m_North, m_South;
	MouseClickDragType m_MouseClickDragType;
	CNetObject *m_pSelectedObject;
	IGuiAPI *m_pGui;

// Private members
private:
	CPlaneHost **m_ppHostMatrix; // 2D-array
	int m_nIdleID;

	unsigned int m_VectorFontListBaseID;
	GLYPHMETRICSFLOAT m_GlyphMetrics[256];
	STextRect m_NameRect, m_IPAddressRect;
	LOGICAL m_bAntialiasingEnabled;
};

// End of piece to move

#endif //__WORLD_PLANE_RENDERER_H_INCLUDED__

