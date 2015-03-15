/* WorldRenderer.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	02 Mar 2007 - initial creation
	02 Apr 2007 - idle moving-swapping
	07 Apr 2007 - warning/alarm frame around host
	15 May 2007 - efficient idle processing, object name in status bar
	20 Oct 2007 - OpenGL tree view instead of MFC tree view
	12 Feb 2008 - sorted items in tree view
	24 Apr 2008 - list of WAN connections instead of list of hosts, connection visibility etc
	13 May 2008 - dynamically recalculated WAN connection elasticity (based on tracert counts)
	20 May 2008 - CPlaneHost and CHost objects are merged to CHost
	23 Jun 2008 - switchable display lists for whole net rendering
*/

#ifndef __WORLD_PLANE_RENDERER_H_INCLUDED__
#define __WORLD_PLANE_RENDERER_H_INCLUDED__

#include "OpenGLRenderer.h"
#include <vector>
#include "Network.h"

// Forward declarations
class CWholeNetTreeRenderer;
class CWholeNetPlaneRenderer;
class IGuiAPI;

class CWholeNetRenderer : public COpenGL_2D
{
// Construction/destruction
public:
	CWholeNetRenderer();
	virtual ~CWholeNetRenderer();

// Public overridable methods
public:
	virtual CNetObject *GetObjectAt(int Xscreen, int Yscreen) = 0;

// Protected overridable methods
protected:
	virtual void OnInitialized();
	virtual void OnRenderScene();
	virtual void RenderToDisplayList() = 0;
	// Event handlers
	virtual void OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y);

// Public methods
public:
	void RefreshView(LOGICAL bRefreshAnyway);
	void MakeDirty() { m_bDirty = LOGICAL_TRUE; }
	void ObjectSelectionChanged(CNetObject *pObject, void *pWhereChanged);

// Protected methods
protected:
	void RebuildDisplayList();
	void SelectObject(CNetObject *pObject);
	void BlockNetworkChange(char *pszText = NULL);
	void UnblockNetworkChange(char *pszText = NULL);

// Private methods
private:

// Public members
public:

// Protected members
protected:
//	CPortableCriticalSection m_CriticalSection;
	MouseDragTypeEnum m_MouseClickDragType;

	CNetObject *m_pSelectedObject;
	CNetwork *m_pNetwork;
	LOGICAL m_bDirty;

// Private members
private:
	GLuint m_WorkingDisplayList, m_SpareDisplayList;
};


///////////////////////////////////////////////////////////////////////////////
// Tree renderer related stuff
typedef struct STreeItemBox
{
	double left, right, top, bottom;
	STreeItemBox()
	{
		Invalidate();
	}
	LOGICAL PointIn(double x, double y)
	{
		return (x > left && x < right && y < top && y > bottom);
	}
	void Invalidate()
	{
		left = right = top = bottom = 0;
	}
} STreeItemBox;

class CTreeNetObject
{
	friend class CWholeNetTreeRenderer;
//Constuctors/destructors
public:
	CTreeNetObject();
	virtual ~CTreeNetObject();

// Public methods
public:

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	void CalcWholeBox(CWholeNetRenderer *pRenderer, double x, double y, double ObjectHeight);
	void InvalidateWholeBox() { m_WholeBox.Invalidate(); }
	void RenderWholeBox(CWholeNetRenderer *pRenderer);
	LOGICAL PointInBox(double x, double y) { return m_WholeBox.PointIn(x, y); }

// Protected overridables
protected:

// Protected members
protected:
	STreeItemBox m_WholeBox;

// Private methods
private:

// Private members
private:
};

class CTreeHost : public CTreeNetObject
{
	friend class CWholeNetTreeRenderer;
//Constuctors/destructors
public:
	CTreeHost(CHost *pHost);
	virtual ~CTreeHost();

// Public methods
public:
	CHost *GetHost() { return m_pHost; }

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	void Render(CWholeNetTreeRenderer *pRenderer, double &x, double &y, double ObjectHeight, double Indent);
	CNetObject *GetNetObject(double x, double y);

// Protected overridables
protected:

// Protected members
protected:
	CHost *m_pHost;

// Private methods
private:

// Private members
private:
};

class CTreeSubnet : public CTreeNetObject
{
	friend class CWholeNetTreeRenderer;
//Constuctors/destructors
public:
	CTreeSubnet(CSubnet *pSubnet);
	virtual ~CTreeSubnet();

// Public methods
public:
	CSubnet *GetSubnet() { return m_pSubnet; }

// Public overridables
public:

// Public members
public:

// Protected methods
protected:
	void Render(CWholeNetTreeRenderer *pRenderer, double &x, double &y, double ObjectHeight, double Indent);
	CNetObject *GetNetObject(double x, double y);
	inline LOGICAL IsVisible();
	LOGICAL IsExpanded() { return m_bExpanded; }
	void ToggleExpand() { m_bExpanded = !m_bExpanded; }
	void CalcExpandBox(CWholeNetTreeRenderer *pRenderer, double x, double y, double ObjectHeight, double Indent);
	void RenderExpandBox(CWholeNetTreeRenderer *pRenderer);
	LOGICAL PointInExpandBox(double x, double y) { return m_ExpandBox.PointIn(x, y); }

// Protected overridables
protected:

// Protected members
protected:
	CSubnet *m_pSubnet;
	LOGICAL m_bExpanded;
	STreeItemBox m_ExpandBox;

// Private methods
private:

// Private members
private:
};



class CListOfTreeHosts : public std::vector<CTreeHost *>
{
};

class CListOfTreeSubnets : public std::vector<CTreeSubnet *>
{
};

class CWholeNetTreeRenderer : public CWholeNetRenderer
{
// Construction/destruction
public:
	CWholeNetTreeRenderer();
	virtual ~CWholeNetTreeRenderer();

// Public overridable methods
public:
	virtual void OnInitialized();
	virtual void RenderToDisplayList();
	virtual void OnSetDefaultOptions();
	virtual CNetObject *GetObjectAt(int Xscreen, int Yscreen);
	virtual LOGICAL IsMouseMovePanXEnabled() { return LOGICAL_FALSE; }
	virtual LOGICAL IsMouseMovePanYEnabled() { return LOGICAL_TRUE; }

// Protected overridable methods
protected:
	virtual LOGICAL IsIsotropic() { return LOGICAL_FALSE; }
	// Event handlers
	virtual void OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y);

// Public methods
public:
	CTreeHost *HostCreated(CHost *pHost);
	CTreeSubnet *SubnetCreated(CSubnet *pSubnet);
	void SubnetChanged(CSubnet *pSubnet);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostChanged(CHost *pHost);

	LOGICAL OnIdleNonMFCThread();
	void NetObjectIgnored(CNetObject *pObject);
	void NetObjectHidden(CNetObject *pObject);
	void ShowHiddenObjects();
	void ShowIgnoredObjects();
	void SortByName();
	LOGICAL IsSortedByName();
	void SortByIpAddress();
	LOGICAL IsSortedByIpAddress();

// Protected methods
protected:
	CTreeHost *FindTreeHost(CHost *pHost);
	void SortItems();

// Private methods
private:

// Public members
public:

// Protected members
protected:
	CListOfTreeHosts m_Hosts;
	CListOfTreeSubnets m_Subnets;
	unsigned int m_uFontBase;
	double m_HostHeight;
	double m_Indentation;
// Private members
private:
};


///////////////////////////////////////////////////////////////////////////////
// Plane renderer related stuff

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


const unsigned long SKIP_OCCUPIED_FLAG			= 1 << 0;
const unsigned long SKIP_OUT_OF_SUBNET_FLAG		= 1 << 1;
const unsigned long SKIP_THE_SAME_SUBNET_FLAG	= 1 << 2;

class CWholeNetPlaneRenderer : public CWholeNetRenderer
{
// Construction/destruction
public:
	CWholeNetPlaneRenderer();
	virtual ~CWholeNetPlaneRenderer();

// Public overridable methods
public:
	virtual void OnInitialized();
	virtual void RenderToDisplayList();
	virtual void OnSetDefaultOptions();
	virtual CNetObject *GetObjectAt(int Xscreen, int Yscreen);

// Protected overridable methods
protected:
	virtual double GetWorldSizeX() { return 2+max((m_East-m_West), (m_North-m_South)); }
	virtual double GetWorldSizeY() { return GetWorldSizeX(); }
	// Event handlers
	//virtual void OnEvent_LeftMouseButtonPressed(unsigned int uFlags, int x, int y);

// Public methods
public:
	void HostCreated(CHost *pHost);
	void HostAddedToSubnet(CHost *pHost, CSubnet *pSubnet);
	void HostChanged(CHost *pHost);

	CHost **GetPlaceForPlaneHostAt(int iCol, int iRow);
	CHost *GetHostAt(int iCol, int iRow);

	void ZoomIn();
	void ZoomOut();
	LOGICAL OnIdleNonMFCThread();
	void WriteHostIPAddress(char *pszIpAddressString, STextDisplayContext &Context);
	void WriteHostName(char *pszHostName, STextDisplayContext &Context);
	LOGICAL AntialiasingEnabled() { return m_bAntialiasingEnabled; }
	void ToggleAntialiasing() { m_bAntialiasingEnabled = !m_bAntialiasingEnabled; Redraw(); }
	void NetObjectIgnored(CNetObject *pObject);
	void NetObjectHidden(CNetObject *pObject);
	void ShowHiddenObjects();
	void ShowIgnoredObjects();
	double GravityAt(int iCol, int iRow);
	double ChemicalInteraction(CHost *pHost, CHost *pNeighbour);

// Protected methods
protected:
	double EnergyAt(CHost *pHost, int iCol, int iRow);
	double Energy(CHost *pPlaneHost);
	LOGICAL IsAnyNeighbourOfTheSameSubnet(int iCol, int iRow, CHost *pHost);
	void PutPlaneHostAtAnyLocation(CHost *pPlaneHost);
	void PutPlaneHostAt(CHost *pPlaneHost, int iCol, int iRow);
	void FreeOccupiedPlace(CHost *pPlaneHost);
	void MovePlaneHost(CHost *pPlaneHost, int iCol, int iRow);
	void SwapPlaneHosts(CHost *pPlaneHost1, CHost *pPlaneHost2);
	void VisualizePlaneHost(CHost *pPlaneHost);
	LOGICAL RandomMove();
	LOGICAL RandomSwap();
	LOGICAL RemoveEmptyRowsCols();
	void RemoveWesternCol();
	void RemoveEasternCol();
	void RemoveSouthernRow();
	void RemoveNorthernRow();
	CHost *RandomPlaneHost();
	void WriteText(char *pszText, STextRect &rect, unsigned int alignment, STextDisplayContext &Context);

// Private methods
private:
	LOGICAL FindPlaceToInsert(CHost *pHost, int &BestCol, int &BestRow, long uFlags);
	void ExtendWorld();

// Public members
public:

// Protected members
protected:
	CListOfNetObjects m_Hosts;
	int m_West, m_East, m_North, m_South;

// Private members
private:
	CHost **m_ppHostMatrix; // 2D-array
	int m_nIdleID;

	unsigned int m_VectorFontListBaseID;
	GLYPHMETRICSFLOAT m_GlyphMetrics[256];
	STextRect m_NameRect, m_IPAddressRect;
	LOGICAL m_bAntialiasingEnabled;
};


#endif //__WORLD_PLANE_RENDERER_H_INCLUDED__

