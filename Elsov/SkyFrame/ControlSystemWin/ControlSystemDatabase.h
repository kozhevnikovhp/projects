#ifndef __CONTROL_SYSTEM_DATABASE_H_INCLUDED__
#define __CONTROL_SYSTEM_DATABASE_H_INCLUDED__

#include "ControlObject.h"

const long UNDEFINED_PARENT = -1;
const long UNDEFINED_TYPE = 0;
#define pszUndefinedName ""

// Events
// Event levels
const long EVENT_LEVEL_ERROR		= 1;
const long EVENT_LEVEL_WARNING		= 2;

// System event
const long EVENT_START				= 1;
const long EVENT_STOP				= 2;
const long EVENT_NEW_OBJECT_CREATED	= 3;
const long EVENT_OBJECT_DELETED		= 4;
const long EVENT_STRUCTURE_CHANGED	= 5;

// Forward declarations

class CDatabaseObjectDesc
{
// Construction/destruction
public:
	CDatabaseObjectDesc() {}
	~CDatabaseObjectDesc() {}
// Public members
public:	
	long	m_ID;
	long	m_PARENT_ID;
	CString	m_NAME;
	long	m_TYPE;
	long	m_bDeleted;
};

class CControlSystemDatabase : public CDatabase
{
// Constuction/destruction
public:
	CControlSystemDatabase();
	virtual ~CControlSystemDatabase();

// Public overridables
	virtual void Close();

// Public methods
public:
	BOOL CreateTables();
	void AfterOpening();
	long GetTargetInstanceID() { return m_TargetInstanceID; }
	BOOL QueryDbChildObjects(long ParentID, CDatabaseObjectDesc *&pDescs, int &nDescs);
	BOOL DeleteObject(CControlObject *pObject);
	BOOL DestroyObject(CControlObject *pObject);
	BOOL SetObjectName(CControlObject *pObject, CString &NewName);
	BOOL SetObjectType(CControlObject *pObject, long type);
	BOOL SetObjectNameAndType(CControlObject *pObject, CString &NewName, long type);
	BOOL SetObjectParent(CControlObject *pChild, CControlObject *pParent);
	long AddNewObject(long ParentID, long Type, const char *pszName);
	// Events
	void Event_OperationStarted();
	void Event_OperationStopped();
	void Event_ObjectCreated(long ObjectID);
	void Event_ObjectDeleted(long ObjectID);

// Protected methods
protected:

	// TargetInstance
	long QueryTargetInstanceID();
	BOOL CreateTargetInstance();
	
	// Tables
	void CreateObjectsTable();
	void CreateObjectConnectionTable();
	void CreateEventLogTable();
	void CreateStringTable();

// Protected members
protected:
	long m_TargetInstanceID;
};


class CControlSystemRecordset : public CRecordset
{
public:
	CControlSystemRecordset(CDatabase *pDB = NULL);
	DECLARE_DYNAMIC(CControlSystemRecordset)
//	virtual CString GetDefaultConnect();    // Default connection string
	long GetTargetInstanceID();
protected:
	CControlSystemDatabase *GetDB() { return m_pDB; }

//protected members
protected:
	CControlSystemDatabase *m_pDB;
};

#endif // __CONTROL_SYSTEM_DATABASE_H_INCLUDED__