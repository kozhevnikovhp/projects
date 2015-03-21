#ifndef MATERIALIZED_OBJECT_H
#define MATERIALIZED_OBJECT_H

#include "portable/text_string.h"

// Forward declarations
class CMaterial;

class CMaterializedObject
{
//Constuctors/destructors
public:
	CMaterializedObject();
	virtual ~CMaterializedObject();

// Public methods
public:
	common::portable::CTextString &GetMaterialName() { return m_MaterialName; }
	CMaterial *GetMaterial() { return m_pMaterial; }
	void SetMaterial(CMaterial *pMaterial);

// Public overridables
public:

// Public members
public:
	CMaterial *m_pMaterial;
	common::portable::CTextString m_MaterialName; // At loading time we store material name, and find material itself later (probably material does not exist yet)

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:

// Private methods
private:

// Private overridables
private:

// Private members
private:

};


#endif // MATERIALIZED_OBJECT_H
