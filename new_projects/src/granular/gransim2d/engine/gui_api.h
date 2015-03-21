/* GuiAPI.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	17 Jul 2008 - initial creation
	13 Oct 2008 - object selection changed
	16 Oct 2008 - particle types introduced
	13 Nov 2008 - materials
	10 Dec 2008 - "cannot simulate" diagnostics
	05 Jan 2009 - objects deletion
*/

#ifndef __GUI_API_H_INCLUDED__
#define __GUI_API_H_INCLUDED__

// Forward declarations
class CGranSim2DObject;
class CGeometry2DObject;
class CMaterial;
class CParticleType;
class CParticleSource;
class CReflectingWall;
class CReflectingRectangle;
class CParticlePointSource;
class CParticleLinearSource;

class IGuiAPI
{
// Public interface
public:
	
	// Events
	virtual void DesignCleaned() = 0;
	virtual void DesignChanged() = 0;
	virtual void ReflectingWallCreated(CReflectingWall *pWall, void *pWhere) = 0;
	virtual void ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere) = 0;
	virtual void MaterialCreated(CMaterial *pMaterial, void *pWhere) = 0;
	virtual void ParticleTypeCreated(CParticleType *pType, void *pWhere) = 0;
	virtual void ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere) = 0;
	virtual void ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere) = 0;
	virtual void ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere) = 0;
	virtual void ObjectDeleted(CGranSim2DObject *pObject, void *pWhere) = 0;
	virtual void CannotSimulate(const char *pszWhy) = 0;
};

class CGuiStub : public IGuiAPI
{
// Public interface
public:
	
	// Events
	virtual void DesignCleaned() {}
	virtual void DesignChanged() {}
	virtual void ReflectingWallCreated(CReflectingWall *pWall, void *pWhere) {}
	virtual void ReflectingRectangleCreated(CReflectingRectangle *pRectangle, void *pWhere) {}
	virtual void MaterialCreated(CMaterial *pMaterial, void *pWhere) {}
	virtual void ParticleTypeCreated(CParticleType *pType, void *pWhere) {}
	virtual void ParticlePointSourceCreated(CParticlePointSource *pSource, void *pWhere) {}
	virtual void ParticleLinearSourceCreated(CParticleLinearSource *pSource, void *pWhere) {}
	virtual void ObjectSelectionChanged(CGranSim2DObject *pObject, void *pWhere) {}
	virtual void ObjectDeleted(CGranSim2DObject *pObject, void *pWhere) {}
	virtual void CannotSimulate(const char *pszWhy) {}
};

extern IGuiAPI *g_pGui;

#endif //__GUI_API_H_INCLUDED__
