/* UpDownConverter.h - header file for Up/Down converter interface

	Author: Evgeny A. Kozhevnikov, Zelenograd, Moscow, Russia evgeny@kozhevnikov.org

Revision history:
	May 16 2003 - initial creation
*/

#ifndef __UPDOWN_CONVERTER_H_INCLUDED__
#define __UPDOWN_CONVERTER_H_INCLUDED__

#include "RadioDevice.h"

class CUpDownConverter : public CRadioDevice
{
public:
	// Construction-destruction
	CUpDownConverter();
	virtual ~CUpDownConverter();
	
// Public methods

// Overridable methods
	
// Common device commands

// Up converter methods
// Output
	virtual BOOL CanUpOnOff() { return FALSE; }
	virtual MC_ErrorCode IsUpTurnedOn(BOOL &bOn) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode TurnUpOn(BOOL &bOn) { return MC_COMMAND_NOT_SUPPORTED; }
// Frequency
	virtual unsigned int GetMinUpOutputFrequency() = 0;
	virtual unsigned int GetMaxUpOutputFrequency() = 0;
	virtual BOOL CanSetUpOutputFrequency() { return FALSE; }
	virtual MC_ErrorCode SetUpOutputFrequency(unsigned int &Frequency) { Frequency = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanGetUpOutputFrequency() { return FALSE; }
	virtual MC_ErrorCode GetUpOutputFrequency(unsigned int &Frequency) { Frequency = 0; return MC_COMMAND_NOT_SUPPORTED; }
// 
// Input attenuator
	virtual CanSetUpInputAtt() { return FALSE; }
	virtual MC_ErrorCode SetUpInputAtt(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual CanGetUpInputAtt() { return FALSE; }
	virtual MC_ErrorCode GetUpInputAtt(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
// Output attenuator
	virtual CanSetUpOutputAtt() { return FALSE; }
	virtual MC_ErrorCode SetUpOutputAtt(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual CanGetUpOutputAtt() { return FALSE; }
	virtual MC_ErrorCode GetUpOutputAtt(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
// Gain/loss
	virtual CanSetUpGainLoss() { return FALSE; }
	virtual MC_ErrorCode SetUpGainLoss(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual CanGetUpGainLoss() { return FALSE; }
	virtual MC_ErrorCode GetUpGainLoss(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
// Spectrum inversion
	virtual BOOL CanUpSpectralInv() { return FALSE; }
	virtual MC_ErrorCode EnableUpSpectralInv(BOOL &bEnabled) { bEnabled = FALSE; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode IsUpSpectralInvEnabled(BOOL &bEnabled) { bEnabled = FALSE; return MC_COMMAND_NOT_SUPPORTED; }
// Temperature
	virtual BOOL CanGetUpTemperature() { return FALSE; }
	virtual MC_ErrorCode GetUpTemperature(double &T) { T = -273.15; return MC_COMMAND_NOT_SUPPORTED; }

// Down converter methods
// Receiver
	virtual BOOL CanDownOnOff() { return FALSE; }
	virtual MC_ErrorCode IsDownTurnedOn(BOOL &bOn) { return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode TurnDownOn(BOOL &bOn) { return MC_COMMAND_NOT_SUPPORTED; }
// Frequency
	virtual unsigned int GetMinDownInputFrequency() = 0;
	virtual unsigned int GetMaxDownInputFrequency() = 0;
	virtual BOOL CanSetDownInputFrequency() { return FALSE; }
	virtual MC_ErrorCode SetDownInputFrequency(unsigned int &Frequency) { Frequency = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual BOOL CanGetDownInputFrequency() { return FALSE; }
	virtual MC_ErrorCode GetDownInputFrequency(unsigned int &Frequency) { Frequency = 0; return MC_COMMAND_NOT_SUPPORTED; }
// Input attenuator
	virtual CanSetDownInputAtt() { return FALSE; }
	virtual MC_ErrorCode SetDownInputAtt(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual CanGetDownInputAtt() { return FALSE; }
	virtual MC_ErrorCode GetDownInputAtt(double &AttValue) { AttValue = 0.; return MC_COMMAND_NOT_SUPPORTED; }
// Output attenuator
	virtual CanSetDownOutputAtt() { return FALSE; }
	virtual MC_ErrorCode SetDownOutputAtt(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual CanGetDownOutputAtt() { return FALSE; }
	virtual MC_ErrorCode GetDownOutputAtt(double &AttValue) { AttValue = 0.; return MC_COMMAND_NOT_SUPPORTED; }
// Gain/loss
	virtual CanSetDownGainLoss() { return FALSE; }
	virtual MC_ErrorCode SetDownGainLoss(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
	virtual CanGetDownGainLoss() { return FALSE; }
	virtual MC_ErrorCode GetDownGainLoss(double &AttValue) { AttValue = 0; return MC_COMMAND_NOT_SUPPORTED; }
// Spectrum inversion
	virtual BOOL CanDownSpectralInv() { return FALSE; }
	virtual MC_ErrorCode EnableDownSpectralInv(BOOL &bEnabled) { bEnabled = FALSE; return MC_COMMAND_NOT_SUPPORTED; }
	virtual MC_ErrorCode IsDownSpectralInvEnabled(BOOL &bEnabled) { bEnabled = FALSE; return MC_COMMAND_NOT_SUPPORTED; }
// Temperature
	virtual BOOL CanGetDownTemperature() { return FALSE; }
	virtual MC_ErrorCode GetDownTemperature(double &T) { T = -273.15; return MC_COMMAND_NOT_SUPPORTED; }

// Protected methods
protected:
};

#endif //__UPDOWN_CONVERTER_H_INCLUDED__
