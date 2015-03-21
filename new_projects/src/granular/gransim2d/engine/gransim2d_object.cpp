#include "gransim2d_object.h"
#include "text_const.h"
#include "global.h"

///////////////////////////////////////////////////////////////////////////
// CGranSim2DObject

//static
cGranSim2DDesign *CGranSim2DObject::m_pDesign = NULL;
//static
int CGranSim2DObject::m_nObjectCount = 0;

CGranSim2DObject::CGranSim2DObject()
{
	m_szFullName[0] = 0;
	char szTmpBuffer[128];
	sprintf(szTmpBuffer, "<%s_%d>", PSZ_NONAME, ++m_nObjectCount);
	m_GivenName = szTmpBuffer;
	m_uCheckTime = 0;
}

//virtual
CGranSim2DObject::~CGranSim2DObject()
{
}

char *CGranSim2DObject::GetFullName()
{
	sprintf(m_szFullName, "%s '%s'", GetTypeDesc(), GetGivenName());
	return m_szFullName;
}

void CGranSim2DObject::SetGivenName(const char *pszName)
{
	if (m_GivenName == pszName)
		return;
	m_GivenName = pszName;
	g_pGui->DesignChanged();
}


