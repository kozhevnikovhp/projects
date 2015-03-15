#ifndef __CONTROL_TARGET_API_H_INCLUDED__
#define __CONTROL_TARGET_API_H_INCLUDED__

// Forward declarations

// Interface to all control targets
class IControlTargetAPI
{
// Public methods
public:
	virtual ~IControlTargetAPI() {}

// Pure virtual methods (MUST BE OVERLOADED)
public:
	virtual char *GetTargetDescription() = 0;
	virtual int GetTargetPictureID() = 0;
	virtual int GetObjectPictureID(long ObjectType) = 0;

	// Object types
	virtual long GetNumberOfSupportedObjectTypes() = 0;
	virtual char *GetObjectTypeDescription(long ObjectType) = 0;
	
	// Visualisation
	virtual long HasEventLog(long ObjectType) = 0;
	virtual long HasMonitorAndControl(long ObjectType) = 0;
	virtual long HasTMI(long ObjectType) = 0;
	
	// Monitor and Control
	virtual long CanUseParentConnection(long ObjectType) = 0;
	virtual long CanObjectBeControlledBySerialPort(long ObjectType) { return 0; }
	virtual long CanObjectBeControlledByTelnet(long ObjectType) { return 0; }
	virtual long CanObjectBeControlledBySNMP(long ObjectType) { return 0; }
	virtual long CanObjectBeControlledByGPIB(long ObjectType) { return 0; }
	virtual long MC_HasSerialPortBusAddress(long ObjectType) = 0;
	virtual long MC_CanConfigureSerialPort(long ObjectType) = 0;
	virtual long MC_GetDefaultTelnetPortNo(long ObjectType) { return 23; }
	virtual long MC_CanChangeTelnetPortNo(long ObjectType) { return 1; }

// Protected virtual methods (can be overloaded)
protected:

//Protected members
};


#endif //__CONTROL_TARGET_API_H_INCLUDED__