#ifndef GEOMETRY2D_OBJECT_H
#define GEOMETRY2D_OBJECT_H


#include "gransim2d_object.h"
#include "portable/rgba.h"

class CGeometry2DObject : public CGranSim2DObject
{
//Constuctors/destructors
public:
	CGeometry2DObject();
	virtual ~CGeometry2DObject();

// Public methods
public:
	common::portable::SRGBA *GetRGBA() { return &m_RGBA; }
	void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	void SetColor(common::portable::SRGBA *pRGBA);
	void SetGeometry(const char *pszGeometry);
	void SetGeometry(const common::portable::CTextString &Geometry) { SetGeometry(Geometry.GetText()); }
	bool IsSelected() { return m_bSelected; }
//	void Select(void *pWhoSelects);
//	void Unselect(void *pWhoSelects);

// Public overridables
public:
	virtual const char *GetKeyWord() = 0;
	virtual void SaveToFile(FILE *fd);

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	bool m_bSelected;
	common::portable::SRGBA m_RGBA;

// Private methods
private:

// Private overridables
private:

// Private members
private:

};


#endif // GEOMETRY2D_OBJECT_H
