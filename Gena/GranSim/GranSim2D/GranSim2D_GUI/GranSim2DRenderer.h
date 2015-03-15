/* GranSim2DRenderer.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	15 Mar 2009 - initial creation (from GranSim2DDesign.h)
	28 Mar 2009 - split design and editor/renderer stuff
*/

#ifndef __GRANSIM2D_RENDERER_H_INCLUDED__
#define __GRANSIM2D_RENDERER_H_INCLUDED__

#include "OpenGLEditor.h"
#include "..\GranSim2DDesign.h"

// Forward declarations
class CGranSim2DProjectView;

class CGranSim2DRenderer : public COpenGL_Editor2D
{
// Construction/destruction
public:
	CGranSim2DRenderer();
	virtual ~CGranSim2DRenderer();

// Public overridable methods
public:

// Protected overridable methods
protected:
	virtual double GetWorldSizeX() { return m_pDesign->GetSize(); }
	virtual double GetWorldSizeY() { return GetWorldSizeX(); }
	virtual double GetDefaultWorldCenterX() { return m_Extents.GetCenterX(); }
	virtual double GetDefaultWorldCenterY() { return m_Extents.GetCenterY(); }
//	virtual double GetWorldScaleX() { return 1; }
//	virtual double GetWorldScaleY() { return 1; }
	virtual void OnRenderScene();
	virtual unsigned char *GetEditRubberColor() { return ucYellowColor; }
	// Event handlers
	virtual void OnEventSelectionChanged(CEditorGeometry2D *pSelectedGeometry, void *pWhere);
	// Geometry creation/changing
	virtual void *OnPointCreated(CEditorPoint2D *pEditorPoint);
	virtual void *OnLineCreated(CEditorLine2D *pEditorLine);
	virtual void *OnRectangleCreated(CEditorRectangle2D *pEditorRectangle);
	virtual void OnGeometryChanged(CEditorGeometry2D *pGeometry, CControlPoint2D *pControlPoint);

// Public methods
public:
	void ReflectingWallCreated(CReflectingWall *pWall, void *pWhere);
	void ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere);
	void MaterialCreated(CMaterial *pMaterial, void *pWhere);
	void ParticleTypeCreated(CParticleType *pType, void *pWhere);
	void ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere);
	void ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere);
	void ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere);
	void ObjectDeleted(CGranSim2DObject *pObject, void *pWhere);

	void Select(CGranSim2DObject *pObject);
	void ZoomIn();
	void ZoomOut();
	void ToggleReflectingWallCreation();
	void ToggleReflectingRectangleCreation();
	void ToggleParticleLinearSourceCreation();
	void ToggleParticlePointSourceCreation();
	void DeleteSelectedObject();
	char *GetDisplayedText(int x, int y);
	void SetDesign(CGranSim2DDesign *pDesign) { m_pDesign = pDesign; }
	void SetProjectView(CGranSim2DProjectView *pProjectView) { m_pProjectView = pProjectView; }

// Protected methods
protected:
	void RenderParticles();

// Private methods
private:

// Public members
public:

// Protected members
protected:
	CGranSim2DObject *m_pSelectedObject;
	CGranSim2DDesign *m_pDesign;
	CGranSim2DProjectView *m_pProjectView;
	char m_szText[512];
	LOGICAL m_bCreationFromFile;
	
//	CNetObject *m_pSelectedObject;

// Private members
private:
};

// Misc
void FineRenderParticleType(CParticleType *pType);

#endif //__GRANSIM2D_RENDERER_H_INCLUDED__