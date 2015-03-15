#include "ExportedMethods.h"
#include "SkyFrameNet.h"

//////////////////////////////////////////////////////////////////////////////////
// DLL exported methods

IControlTargetAPI *CreateControlTarget(IControlSystemAPI *pControlSystemAPI)
{
	return new CSkyFrameNetRussian(pControlSystemAPI);
}

void DeleteControlTarget(IControlTargetAPI *pControlTargetAPI)
{
	delete pControlTargetAPI;
}

char *GetControlTargetDescription()
{
	return "SkyFrame Net";
}
