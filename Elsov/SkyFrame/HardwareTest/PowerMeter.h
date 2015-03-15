#ifndef __POWER_METER_H_INCLUDED__
#define __POWER_METER_H_INCLUDED__

#include "MeasureDevice.h"

class CPowerMeter : public CMeasureDevice
{
// Construction/destruction
public:
	CPowerMeter();
	virtual ~CPowerMeter();

// Common device methods
	virtual BOOL IsPowerMeter() { return TRUE; }

// Power measurement
	virtual int GetChannelsNumber() { return 0; }
	virtual MC_ErrorCode MeasurePower(double &Power, int Channel) { return MC_COMMAND_NOT_SUPPORTED; }
};

#endif // __POWER_METER_H_INCLUDED__