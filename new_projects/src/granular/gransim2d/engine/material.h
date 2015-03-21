#ifndef MATERIAL_H
#define MATERIAL_H

#include "gransim2d_object.h"
#include <vector>

class CMaterial : public CGranSim2DObject
{
//Constuctors/destructors
public:
	CMaterial();
	virtual ~CMaterial();

// Public methods
public:

// Public overridables
public:
	virtual const char *GetTypeDesc() { return "Material"; }
	virtual const char *GetKeyWord();
	virtual int GetFormCount();
	virtual SFormDesc *GetFormDesc(int iFormNumber);
	virtual void SaveToFile(FILE *fd);
	virtual void DeleteFromDesign();

// Public members
public:
	double m_Nu; // Poisson
	double m_E; // Young
	double m_Ro;

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
typedef std::vector<CMaterial *> cMaterials;



#endif // MATERIAL_H
