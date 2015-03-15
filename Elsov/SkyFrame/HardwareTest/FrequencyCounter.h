#ifndef __FREQUENCY_COUNTER_H_INCLUDED__
#define __FREQUENCY_COUNTER_H_INCLUDED__

#include "MeasureDevice.h"

class CFrequencyCounter : public CMeasureDevice
{
// Construction/destruction
public:
	CFrequencyCounter();
	virtual ~CFrequencyCounter();

// Common device methods
	virtual BOOL IsFrequencyCounter() { return TRUE; }

// Power measurement
	virtual int GetChannelsNumber() { return 0; }
	virtual MC_ErrorCode MeasureFrequency(double &Frequency, int Channel) { return MC_COMMAND_NOT_SUPPORTED; }
};

#endif // __FREQUENCY_COUNTER_H_INCLUDED__