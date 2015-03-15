#include "stdafx.h"
#include "ControlSystemDatabase.h"
#include "ControlledObjectsRecordset.h"
#include "EventLogRecordset.h"
#include "ControlTargetAPI.h"
#include "ControlObject.h"

///////////////////////////////////////////////////////////////////////////
// CControlSystemDatabase class implementation

CControlSystemDatabase::CControlSystemDatabase()
{
	m_TargetInstanceID = -1;
}

//virtual
CControlSystemDatabase::~CControlSystemDatabase()
{
}

void CControlSystemDatabase::AfterOpening()
{
	m_TargetInstanceID = QueryTargetInstanceID();
	Event_OperationStarted();
}

//virtual
void CControlSystemDatabase::Close()
{
	if (IsOpen())
	{
		Event_OperationStopped();
	}
	CDatabase::Close();
}

BOOL CControlSystemDatabase::CreateTables()
{
	BOOL bSuccess = TRUE;
	TRY
	{
		CreateStringTable();
		CreateObjectsTable();
		CreateObjectConnectionTable();
		CreateEventLogTable();
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании таблиц", MB_ICONEXCLAMATION);
		bSuccess = FALSE;
	END_CATCH

	TRY
	{
		CreateTargetInstance();
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании записи в таблицу объектов", MB_ICONEXCLAMATION);
		bSuccess = FALSE;
	END_CATCH

	return bSuccess;
}

BOOL CControlSystemDatabase::QueryDbChildObjects(long ParentID, CDatabaseObjectDesc *&pDescs, int &nDescs)
{
	nDescs = 0;
	pDescs = NULL;
	if (ParentID < 0)
		ParentID = m_TargetInstanceID;

	CControlledObjectsRecordset ObjectsSet(this);
	ObjectsSet.m_strFilter = "PARENT_ID = ?";
	ObjectsSet.m_strSort = "TYPE, NAME";
	ObjectsSet.m_Param1 = ParentID;
	ObjectsSet.Open();
	while (!ObjectsSet.IsEOF())
	{
		++nDescs;
		ObjectsSet.MoveNext();
	}
	
	if (nDescs)
	{
		pDescs = new CDatabaseObjectDesc[nDescs];
		ObjectsSet.MoveFirst();
		int iDesc = 0;
		while (!ObjectsSet.IsEOF())
		{
			CDatabaseObjectDesc *pDesc = pDescs + iDesc;
			pDesc->m_ID = ObjectsSet.m_ID;
			pDesc->m_PARENT_ID = ObjectsSet.m_PARENT_ID;
			pDesc->m_NAME = ObjectsSet.m_NAME;
			pDesc->m_TYPE = ObjectsSet.m_TYPE;
			pDesc->m_bDeleted = ObjectsSet.m_bDeleted;
			ObjectsSet.MoveNext();
			++iDesc;
		}
	}
	return TRUE;
}

BOOL CControlSystemDatabase::DeleteObject(CControlObject *pObject)
{
	CControlledObjectsRecordset ObjectsSet(this);
	BOOL bDeleted = ObjectsSet.DeleteObject(pObject->GetObjectID());
	if (bDeleted)
	{
		Event_ObjectDeleted(pObject->GetObjectID());
	}
	return bDeleted;
}

BOOL CControlSystemDatabase::DestroyObject(CControlObject *pObject)
{
	CControlledObjectsRecordset ObjectsSet(this);
	return ObjectsSet.DestroyObject(pObject->GetObjectID());
}

BOOL CControlSystemDatabase::SetObjectName(CControlObject *pObject, CString &NewName)
{
	CControlledObjectsRecordset ObjectsSet(this);
	return ObjectsSet.SetObjectName(pObject->GetObjectID(), NewName);
}

BOOL CControlSystemDatabase::SetObjectType(CControlObject *pObject, long type)
{
	CControlledObjectsRecordset ObjectsSet(this);
	return ObjectsSet.SetObjectType(pObject->GetObjectID(), type);
}

BOOL CControlSystemDatabase::SetObjectNameAndType(CControlObject *pObject, CString &NewName, long type)
{
	CControlledObjectsRecordset ObjectsSet(this);
	return ObjectsSet.SetObjectNameAndType(pObject->GetObjectID(), NewName, type);
}

BOOL CControlSystemDatabase::SetObjectParent(CControlObject *pChild, CControlObject *pParent)
{
	CControlledObjectsRecordset ObjectsSet(this);
	return ObjectsSet.SetObjectParent(pChild->GetObjectID(), pParent->GetObjectID());
}

long CControlSystemDatabase::AddNewObject(long ParentID, long Type, const char *pszName)
{
	if (ParentID < 0)
		ParentID = m_TargetInstanceID;
	CControlledObjectsRecordset ObjectsSet(this);
	long ID = ObjectsSet.AddNewObject(ParentID, Type, pszName);
	Event_ObjectCreated(ID);
	return ID;
}

long CControlSystemDatabase::QueryTargetInstanceID()
{
	CControlledObjectsRecordset ObjectsSet(this);
	return ObjectsSet.QueryTargetInstanceID();
}

BOOL CControlSystemDatabase::CreateTargetInstance()
{
	m_TargetInstanceID = QueryTargetInstanceID();
	if (m_TargetInstanceID > 0)
		return TRUE;
	CControlledObjectsRecordset ObjectsSet(this);
	m_TargetInstanceID = ObjectsSet.CreateTargetInstance();
	return ((m_TargetInstanceID > 0));
}

void CControlSystemDatabase::CreateObjectsTable()
{
	if (!IsOpen())
		return;
	ExecuteSQL("CREATE TABLE ControlObjects	(			\
				ID			COUNTER  				,	\
				PARENT_ID	INTEGER			NOT NULL,	\
				NAME		VARCHAR(100)			,	\
				TYPE		INTEGER			NOT NULL,	\
				DELETED		INTEGER			NOT NULL,	\
				PRIMARY KEY(ID))");
}

void CControlSystemDatabase::CreateObjectConnectionTable()
{
	if (!IsOpen())
		return;
	ExecuteSQL("CREATE TABLE ObjectConnections	(			\
				ID				INTEGER			NOT NULL,	\
				USE_PARENT		INTEGER					,	\
				TYPE			INTEGER					,	\
				SERIAL_PORT_NO	INTEGER					,	\
				CONNECTED		INTEGER					,	\
				TelnetOptions	VARCHAR(100)			,	\
				OPTIONS2		VARCHAR(100)			,	\
				PRIMARY KEY(ID)							,	\
				FOREIGN KEY (ID) REFERENCES ControlObjects (ID) ON DELETE CASCADE			\
				)											\
				");													  
}

void CControlSystemDatabase::CreateEventLogTable()
{
	if (!IsOpen())
		return;
	ExecuteSQL("CREATE TABLE EventLog					\
					(									\
					EventID COUNTER,					\
					EventType INTEGER,					\
					ObjectID INTEGER NOT NULL,			\
					EventTime DATETIME,					\
					CommentID INTEGER,					\
					PRIMARY KEY(EventType, EventTime),	\
					FOREIGN KEY (ObjectID) REFERENCES ControlObjects (ID) ON DELETE CASCADE	\
					)									\
				");													  
}

void CControlSystemDatabase::CreateStringTable()
{
	if (!IsOpen())
		return;
	ExecuteSQL("CREATE TABLE Strings	(				\
				ID				COUNTER				,	\
				Content			VARCHAR(128)		,	\
				NextChunkID		INTEGER					\
				)										\
				");													  
}

void CControlSystemDatabase::Event_OperationStarted()
{
	TRY
	{
		CEventLogRecordset set(this);
		set.OperationStarted();
	}
	CATCH(CDBException, e)
		MessageBox(NULL, e->m_strError, "Проблемы при создании записи в таблицу событий", MB_ICONEXCLAMATION);
	END_CATCH
}

void CControlSystemDatabase::Event_OperationStopped()
{
	CEventLogRecordset set(this);
	set.OperationStopped();
}

void CControlSystemDatabase::Event_ObjectCreated(long ObjectID)
{
	CEventLogRecordset set(this);
	set.ObjectCreated(ObjectID);
}

void CControlSystemDatabase::Event_ObjectDeleted(long ObjectID)
{
	CEventLogRecordset set(this);
	set.ObjectDeleted(ObjectID);
}

///////////////////////////////////////////////////////////////////////////
// CControlSystemRecordset class implementation

IMPLEMENT_DYNAMIC(CControlSystemRecordset, CRecordset)

CControlSystemRecordset::CControlSystemRecordset(CDatabase *pDB)
 : CRecordset(pDB)
{
	m_pDB = (CControlSystemDatabase *)pDB;
}

//virtual
/*CString CControlSystemRecordset::GetDefaultConnect()
{
	CString RetValue = "ODBC;";
	CString ConnectionStr = m_pDatabase->GetConnect();
	
	char *pszDsn = strstr(LPCTSTR(ConnectionStr), "DSN");
	if (pszDsn)
	{
		do
		{
			RetValue += *pszDsn;
		} while (*pszDsn && *pszDsn != ';');
	}
	return RetValue;
}*/

long CControlSystemRecordset::GetTargetInstanceID()
{
	return GetDB()->GetTargetInstanceID();
}

