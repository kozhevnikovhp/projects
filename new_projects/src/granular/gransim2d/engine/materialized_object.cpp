#include "materialized_object.h"
#include "material.h"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
// CMaterializedObject

CMaterializedObject::CMaterializedObject()
{
	m_pMaterial = NULL;
}

//virtual
CMaterializedObject::~CMaterializedObject()
{
}

void CMaterializedObject::SetMaterial(CMaterial *pMaterial)
{
	m_pMaterial = pMaterial;
	if (m_pMaterial)
		m_MaterialName = m_pMaterial->GetGivenName();
	else
		m_MaterialName.Empty();
}


