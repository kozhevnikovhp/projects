#include "stdafx.h"
#include "TargetInstance.h"
#include "ControlTargetAPI.h"
#include "ControlObject.h"


CTargetInstance::CTargetInstance()
{
	Initialize();
}

CTargetInstance::CTargetInstance(CString &DllFileName)
{
	Create(DllFileName);
}

//virtual
CTargetInstance::~CTargetInstance()
{
	m_DB.Close();
	Free();
}

void CTargetInstance::Initialize()
{
	m_pTarget = NULL;
	m_DllFileName = &afxChNil;
	m_TargetName = &afxChNil;
	m_DbConnectionString = &afxChNil;
}

void CTargetInstance::Create(CString &DllFileName)
{
	typedef IControlTargetAPI *(*PFN_CREATE_TARGET) ();
	
	Initialize();
	m_DllFileName = DllFileName;

	if (m_DLL.Load(DllFileName))
	{
		PFN_CREATE_TARGET pfnCreateTarget = (PFN_CREATE_TARGET)GetProcAddress(m_DLL.GetHandle(), "CreateControlTarget");
		if (pfnCreateTarget)
		{
			m_pTarget = pfnCreateTarget();
		}
	}
	if (!m_pTarget)
		Free();
}

void CTargetInstance::Free()
{
	typedef void(*PFN_DELETE_TARGET) (IControlTargetAPI *);

	if (m_DLL.IsLoaded())
	{
		PFN_DELETE_TARGET pfnDeleteTarget = (PFN_DELETE_TARGET)GetProcAddress(m_DLL.GetHandle(), "DeleteControlTarget");
		if (pfnDeleteTarget)
		{
			if (m_pTarget)
				pfnDeleteTarget(m_pTarget);
		}
		m_DLL.Unload();
	}
	for (int i = 0; i < m_Objects.size(); i++)
	{
		CControlObject *pObject = m_Objects[i];
		pObject->RemoveItselfFromParent();
		delete pObject;
	}
	Initialize();
}

void CTargetInstance::GetFullName(CString &FullName)
{
	FullName.Format("%s - %s", m_TargetName, m_pTarget->GetTargetDescription());
}

void CTargetInstance::GetObjectFullName(CString &ObjectName, int ObjectType, CString &FullName)
{
	FullName.Format("%s - %s", ObjectName, m_pTarget->GetObjectTypeDescription(ObjectType));
}

void CTargetInstance::WriteToRegistry(CString &SectionName)
{
	AfxGetApp()->WriteProfileString(SectionName, "TargetName", m_TargetName);
	AfxGetApp()->WriteProfileString(SectionName, "FileName", m_DllFileName);
	AfxGetApp()->WriteProfileString(SectionName, "DBConnectionString", m_DbConnectionString);
}

void CTargetInstance::LoadFromRegistry(CString &SectionName)
{
	CString DllFileName = AfxGetApp()->GetProfileString(SectionName, "FileName", "");
	Create(DllFileName);
	
	m_TargetName = AfxGetApp()->GetProfileString(SectionName, "TargetName", "");
	m_DbConnectionString = AfxGetApp()->GetProfileString(SectionName, "DBConnectionString", "");
	if (!m_DbConnectionString.IsEmpty())
	{
		BOOL bOpen = m_DB.Open(m_DbConnectionString);
		m_DB.AfterOpening();
	}
}

BOOL CTargetInstance::CreateDB()
{
	m_DB.Close();
	BOOL bOpened = m_DB.Open(NULL);
	if (!bOpened)
		return FALSE;
	m_DB.CreateTables();
	m_DB.AfterOpening();
	m_DbConnectionString = m_DB.GetConnect();
	return TRUE;
}

CControlObject *CTargetInstance::CreateControlObject(CControlObject *pParent)
{
	long ParentID = UNDEFINED_PARENT;
	if (pParent)
		ParentID = pParent->GetObjectID();
	CString Name = pszUndefinedName;
	long ID = GetDatabase()->AddNewObject(ParentID, UNDEFINED_TYPE, Name);
	CControlObject *pNewObject = new CControlObject(this, pParent, ID, UNDEFINED_TYPE, Name, FALSE);
	m_Objects.push_back(pNewObject);
	return pNewObject;
}

CControlObject *CTargetInstance::CreateControlObject(CControlObject *pParent, long ID, long Type, CString Name, long bDeleted)
{
	CControlObject *pNewObject = new CControlObject(this, pParent, ID, Type, Name, bDeleted);
	m_Objects.push_back(pNewObject);
	return pNewObject;
}

void CTargetInstance::DeleteControlObject(CControlObject *pObjectToDelete)
{
	if (!pObjectToDelete)
		return;
	GetDatabase()->DeleteObject(pObjectToDelete);
}
	
void CTargetInstance::DestroyControlObject(CControlObject *pObjectToDelete)
{
	if (!pObjectToDelete)
		return;
	GetDatabase()->DestroyObject(pObjectToDelete);
	pObjectToDelete->RemoveItselfFromParent();
	delete pObjectToDelete;
}
	
void CTargetInstance::RemoveObject(CControlObject *pObjectToDelete)
{
	// remove from the list of objects
	for (int i = 0; i < m_Objects.size(); i++)
	{
		CControlObject *pObject = m_Objects[i];
		if (pObject == pObjectToDelete)
		{
			m_Objects.erase(m_Objects.begin() + i);
			break;
		}
	}
}

void CTargetInstance::SetObjectParent(CControlObject *pChild, CControlObject *pParent)
{
	GetDatabase()->SetObjectParent(pChild, pParent);
}
