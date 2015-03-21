#include "geometry2d_object.h"
#include "gransim2d_design.h"
#include "design_text_string.h"
#include "global.h"
#include "text_const.h"

using namespace common;
using namespace geometry2d;

///////////////////////////////////////////////////////////////////////////
// CGeometry2DObject

CGeometry2DObject::CGeometry2DObject()
{
	m_bSelected = false;
}

//virtual
CGeometry2DObject::~CGeometry2DObject()
{
}

void CGeometry2DObject::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a/* = 255 */)
{
	m_RGBA.Init(r, g, b, a);
	g_pGui->DesignChanged();
}

void CGeometry2DObject::SetColor(common::portable::SRGBA *pRGBA)
{
	m_RGBA.Init(pRGBA);
	g_pGui->DesignChanged();
}

void CGeometry2DObject::SetGeometry(const char *pszGeometry)
{
	CGeometry2D *pGeometry = GetGeometry();
	pGeometry->Unformat(pszGeometry);
	pGeometry->RecalcExtents();
//	m_pRenderer->RebuildDisplayList(pGeometry);
	g_pGui->DesignChanged();
//	m_pRenderer->Redraw();
}

//virtual
void CGeometry2DObject::SaveToFile(FILE *fd)
{
	CDesignTextString str;
	// Name
	str.Set(PSZ_NAME_KEYWORD, GetGivenName());
	m_pDesign->WriteString(fd, &str);

	// Color
	str.Set(PSZ_COLOR_KEYWORD, GetRGBA());
	m_pDesign->WriteString(fd, &str);

	//Geometry
	std::string GeomStr;
	GetGeometry()->Format(GeomStr);
	str.Set(PSZ_GEOMETRY_KEYWORD, GeomStr.c_str());
	m_pDesign->WriteString(fd, &str);
}

