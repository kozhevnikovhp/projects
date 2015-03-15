/* World.h

Author: Evgeny A. Kozhevnikov, Zelenograd, Russia. E-mail evgeny@kozhevnikov.org

Revision history:
	02 Mar 2007 - initial creation

*/

#ifndef __WORLD_RENDERER_H_INCLUDED__
#define __WORLD_RENDERER_H_INCLUDED__

#include "OpenGLRenderer.h"

// Forward declarations
class CWorld;

class CWorldRenderer : public COpenGL_2D
{
// Construction/destruction
public:
	CWorldRenderer();
	virtual ~CWorldRenderer();

// Public overridable methods
public:

// Protected overridable methods
protected:
	virtual void OnRenderScene();
	virtual void OnInitialized();
	virtual LOGICAL IsIsotropic() { return LOGICAL_FALSE; }
	virtual double GetWorldSizeX();
	virtual double GetWorldSizeY();
	virtual double GetDefaultWorldCenterX();
	virtual double GetDefaultWorldCenterY();

// Public methods
public:
	void SetWorld(CWorld *pWorld) { m_pWorld = pWorld; SetDefaultView(); }

// Protected methods
protected:

// Private methods
private:

// Protected members
protected:
	CWorld *m_pWorld;

// Private members
private:
};


#endif //__WORLD_RENDERER_H_INCLUDED__

