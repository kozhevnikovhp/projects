#ifndef __CONTROL_OBJECT_H_INCLUDED__
#define __CONTROL_OBJECT_H_INCLUDED__

// Forward declarations
class CTargetInstance;

typedef enum
{
	MCT_Unknown,
	MCT_SerialPort,
	MCT_Telnet,
	MCT_SNMP,
	MCT_GPIB
} MC_Type;

class CMCParams
{
// constructors/destructors
public:
	CMCParams();
	~CMCParams();

// public methods
public:
	void Initialize();
	
// Public members
public:
	long	m_bUSE_PARENT;
	MC_Type	m_TYPE;
	long	m_SerialPortNo;
	CString	m_SerialPortOptions;
	long	m_SerialPortBusAddress;
	long	m_CONNECTED;
	CString	m_TelnetOptions;
};


// Interface to objects under control
class CControlObject
{
// Construction/destruction
public:
	CControlObject(CTargetInstance *pTargetInstance, CControlObject *pParent, long ID, long Type, CString &Name, long bDeleted);
	virtual ~CControlObject();
// Public methods
public:
	long GetObjectID() { return m_ID; }
	long GetType() { return m_Type; }
	void SetType(long type);
	int IsDeleted() { return m_bDeleted; }
	int GetPictureID();
	CTargetInstance *GetTargetInstance() { return m_pTargetInstance; }
	CString &GetName() { return m_Name; }
	void SetName(CString &NewName);
	void SetNameAndType(CString NewName, long type);
	void AddChild(CControlObject *pChild);
	void RemoveItselfFromParent();
	BOOL MC_HasMonitorAndControl();
	BOOL MC_CanUseParentConnection();
	BOOL MC_CanBeControlledBySerialPort();
	BOOL MC_CanBeControlledByTelnet();
	BOOL MC_CanBeControlledBySNMP();
	BOOL MC_CanBeControlledByGPIB();
	BOOL MC_HasSerialPortBusAddress();
	BOOL MC_CanConfigureSerialPort();
	BOOL MC_GetDefaultTelnetPortNo();
	BOOL MC_CanChangeTelnetPortNo();
	BOOL MC_LoadMonitorAndControlParams();
	BOOL MC_SaveMonitorAndControlParams();
	CMCParams *GetMCParams() { return &m_MCParams; }

	BOOL HasEventLog();

// Pure virtual methods (MUST BE OVERLOADED)
public:
	virtual char *GetFullName();
	void RemoveChild(CControlObject *pChild);
	void KillChildren();

// Protected virtual methods (can be overloaded)
protected:

//Protected members
protected:
	long m_ID;
	long m_Type;
	CTargetInstance *m_pTargetInstance;
	CControlObject *m_pParent;
	CString m_Name;
	int m_bDeleted;
	char m_szFullName[512];
	CControlObject *m_pNext, *m_pPrev; // next and prev objects in the list of parent's children
	CControlObject *m_pFirstChild;

	CMCParams m_MCParams;
};

#endif //__CONTROL_OBJECT_H_INCLUDED__

