#ifndef __MEASURE_DEVICE_H_INCLUDED__
#define __MEASURE_DEVICE_H_INCLUDED__

#include "Device.h"

class CMeasureDevice : public CDevice
{
// Construction/destruction
public:
	CMeasureDevice();
	virtual ~CMeasureDevice();

// Common device methods
	virtual BOOL IsMeasureDevice() { return TRUE; }
	virtual BOOL IsPowerMeter() { return FALSE; }
	virtual BOOL IsFrequencyCounter() { return FALSE; }
	virtual MC_ErrorCode ResetMeasurements() { return MC_COMMAND_NOT_SUPPORTED; }
};

#endif // __MEASURE_DEVICE_H_INCLUDED__