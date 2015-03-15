#ifndef __TRANCEIVER_H_INCLUDED__
#define __TRANCEIVER_H_INCLUDED__

#include "Device.h"

class CTranceiver : public CDevice
{
// Construction/destruction
public:
	CTranceiver();
	virtual ~CTranceiver();
// Common device commands
	virtual BOOL IsTranceiver() { return TRUE; }
// Non Volatile RAM
// Frequency (carrier)

// Temperature
	virtual BOOL CanGetTemperature() { return FALSE; }
	virtual MC_ErrorCode IsTemperatureAlarm(BOOL &bAlarm) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetTemperature(double &T) { T = -273.15; return MC_COMMAND_NOT_SUPPORTED; }
// Output power and mute
	virtual BOOL CanOutputOnOff() { return FALSE; }
	virtual BOOL CanGetPower() { return FALSE; }
	virtual MC_ErrorCode IsOutputOn(BOOL &bOn) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode TurnOutputOn(BOOL &bOn) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode GetPower(double &Power) { Power = 0.; return MC_COMMAND_NOT_SUPPORTED; }
// Frequency
	virtual unsigned int GetBandShift() { ASSERT(FALSE); return 0; }
	virtual BOOL CanGetFrequency() { return FALSE; }
	virtual MC_ErrorCode GetFrequency(unsigned int &Frequency) { Frequency = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanSetFrequency() { return FALSE; }
	virtual MC_ErrorCode SetFrequency(unsigned int &Frequency) { Frequency = 0; return MC_COMMAND_NOT_SUPPORTED; }
// Gain
	virtual BOOL CanGetGain() { return FALSE; }
	virtual MC_ErrorCode GetGain(double &Gain) { Gain = 0.; return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanSetGain() { return FALSE; }
	virtual MC_ErrorCode SetGain(double &Gain) { Gain = 0.; return MC_COMMAND_NOT_SUPPORTED; }
};

#endif // __TRANCEIVER_H_INCLUDED__