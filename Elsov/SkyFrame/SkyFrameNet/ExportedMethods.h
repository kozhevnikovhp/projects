#ifndef __EXPORTED_METHODS_H_INCLUDED__
#define __EXPORTED_METHODS_H_INCLUDED__

// Forward declarations
class IControlTargetAPI;
class IControlSystemAPI;

IControlTargetAPI *CreateControlTarget(IControlSystemAPI *pControlSystemAPI);
void DeleteControlTarget(IControlTargetAPI *pControlTargetAPI);
char *GetControlTargetDescription();

#endif //__EXPORTED_METHODS_H_INCLUDED__