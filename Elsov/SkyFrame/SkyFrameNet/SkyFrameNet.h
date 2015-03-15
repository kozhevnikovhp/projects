#ifndef __SKY_FRAME_NET_H_INCLUDED__
#define __SKY_FRAME_NET_H_INCLUDED__

#include "ControlTargetAPI.h"
#include "ControlSystemAPI.h"

//using namespace std;

// Object types
const long OBJECT_TYPE_UNKNOWN				= 0;
const long OBJECT_TYPE_CZS					= 1;
const long OBJECT_TYPE_UZS					= 2;
const long OBJECT_TYPE_FRAME_UZS			= 3;
const long OBJECT_TYPE_AZS					= 4;
const long OBJECT_TYPE_FRAME_AZS			= 5;
const long OBJECT_TYPE_MODEM_DATUM			= 6;
const long OBJECT_TYPE_MODEM_RADYNE			= 7;
const long OBJECT_TYPE_TRANCEIVER_WAVESAT	= 8;
const long OBJECT_TYPE_SDMS					= 9;
const long OBJECT_TYPE_SDMS_VSAT			= 10;
const long MAX_OBJECT_TYPE = OBJECT_TYPE_SDMS_VSAT+1;

class CSkyFrameNet : public IControlTargetAPI
{
public:
	CSkyFrameNet();
	CSkyFrameNet(IControlSystemAPI *pControlSystem);
	virtual ~CSkyFrameNet();

	// Common methods
	virtual int GetTargetPictureID();
	virtual int GetObjectPictureID(long ObjectType);

	// Object types
	virtual long GetNumberOfSupportedObjectTypes();
	virtual char *GetObjectTypeDescription(long ObjectType);

	// Visualisation
	virtual long HasEventLog(long ObjectType);
	virtual long HasMonitorAndControl(long ObjectType);
	virtual long HasTMI(long ObjectType);

	// Monitor and Control
	virtual long CanUseParentConnection(long ObjectType);
	virtual long CanObjectBeControlledBySerialPort(long ObjectType);
	virtual long CanObjectBeControlledByTelnet(long ObjectType);
	virtual long CanObjectBeControlledBySNMP(long ObjectType);
	virtual long CanObjectBeControlledByGPIB(long ObjectType);
	virtual long MC_HasSerialPortBusAddress(long ObjectType);
	virtual long MC_CanConfigureSerialPort(long ObjectType);

// Protected members
protected:
	IControlSystemAPI *m_pControlSystem;
	char *m_pszObjectTypeDescs[MAX_OBJECT_TYPE];
};

class CSkyFrameNetRussian : public CSkyFrameNet
{
// Construction/destruction
public:
	CSkyFrameNetRussian(IControlSystemAPI *pControlSystem);
// Common methods
public:
	virtual char *GetTargetDescription();
};

class CSkyFrameNetEnglish : public CSkyFrameNet
{
// Construction/destruction
public:
	CSkyFrameNetEnglish(IControlSystemAPI *pControlSystem);

// Common methods
public:
	virtual char *GetTargetDescription();
};

#endif //__SKY_FRAME_NET_H_INCLUDED__