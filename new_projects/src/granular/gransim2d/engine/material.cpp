#include "material.h"
#include "design_text_string.h"
#include "text_const.h"
#include "gransim2d_design.h"

///////////////////////////////////////////////////////////////////////////
// CMaterial

static SFormDesc MaterialFormDescs[] = {
	{ OBJECT_COMMON_FORM_INDEX, PSZ_EMPTY, 5 },
	{ MATERIAL_CONSTANTS_INDEX, "Material constants", 2 }
};

CMaterial::CMaterial()
{
	m_Nu = 0.3;
	m_E = 1.0E+8;
	m_Ro = 2000;
}

//virtual
CMaterial::~CMaterial()
{
}

//virtual
const char *CMaterial::GetKeyWord()
{
	return PSZ_MATERIAL_KEYWORD;
}

//virtual
int CMaterial::GetFormCount()
{
	return sizeof(MaterialFormDescs)/sizeof(MaterialFormDescs[0]);
}

//virtual
SFormDesc *CMaterial::GetFormDesc(int iFormNumber)
{
	return MaterialFormDescs+iFormNumber;
}

//virtual
void CMaterial::SaveToFile(FILE *fd)
{
	CDesignTextString str;
	// Name
	str.Set(PSZ_NAME_KEYWORD, GetGivenName());
	m_pDesign->WriteString(fd, &str);

	// Material constants
	str.Set(PSZ_POISSON_RATIO_KEYWORD, m_Nu);
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_YOUNG_MODULE_KEYWORD, m_E);
	m_pDesign->WriteString(fd, &str);
	str.Set(PSZ_DENSITY_KEYWORD, m_Ro);
	m_pDesign->WriteString(fd, &str);
}

//virtual
void CMaterial::DeleteFromDesign()
{
	m_pDesign->DeleteMaterial(this);
}

