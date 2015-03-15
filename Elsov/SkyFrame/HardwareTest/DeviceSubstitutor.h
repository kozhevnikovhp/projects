#ifndef __DEVICE_SUBSTITUTOR_H_INCLUDED__
#define __DEVICE_SUBSTITUTOR_H_INCLUDED__

#include "Device.h"

class CDeviceSubstitutor : public CDevice
{
public:
// Construction-destruction
	CDeviceSubstitutor();
	virtual ~CDeviceSubstitutor();
	
// Monitor and Control
// Overridable methods
// Substitutor methods
	virtual BOOL Start() = 0;		// pure virtual - must be overriden
	virtual BOOL Stop() = 0;		// pure virtual - must be overriden
	virtual BOOL IsStarted() = 0;	// pure virtual - must be overriden
// Common device commands
	virtual BOOL IsDeviceSubstitutor() { return TRUE; }

// Public methods

// protected methods

// protected members
protected:
};

#endif //__DEVICE_SUBSTITUTOR_H_INCLUDED__