#include "stdafx.h"
#include "ControlObject.h"
#include "ControlTargetAPI.h"
#include "TargetInstance.h"
#include "ObjectConnectionsRecordset.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// CMCParams

CMCParams::CMCParams()
{
	Initialize();
}

CMCParams::~CMCParams()
{
}

void CMCParams::Initialize()
{
	m_TYPE = MCT_Unknown;
	m_bUSE_PARENT = FALSE;
	m_CONNECTED = FALSE;
	m_SerialPortNo = 0;
	m_SerialPortBusAddress = 0;
	m_SerialPortOptions = &afxChNil;
	m_TelnetOptions = &afxChNil;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// CControlObject

// Object is constructed from database so all fields are known
CControlObject::CControlObject(CTargetInstance *pTargetInstance, CControlObject *pParent, long ID, long Type, CString &Name, long bDeleted)
{
	m_pNext = m_pPrev = m_pFirstChild = NULL;
	m_pTargetInstance = pTargetInstance;
	m_pParent = pParent;
	m_ID = ID;
	m_Type = Type;
	m_Name = Name;
	m_bDeleted = bDeleted;
	if (pParent)
		pParent->AddChild(this);

	// Load connection params
	if (m_pTargetInstance->GetTarget()->HasMonitorAndControl(Type))
		MC_LoadMonitorAndControlParams();
}

//virtual
CControlObject::~CControlObject()
{
	TRACE("Deleting object %s\n", m_szFullName);
	CControlObject *pNextChild = NULL;
	for (CControlObject *pChild = m_pFirstChild; pChild; pChild = pNextChild)
	{
		pNextChild = pChild->m_pNext;
		delete pChild;
	}
	m_pTargetInstance->RemoveObject(this);
}

void CControlObject::RemoveItselfFromParent()
{
	if (m_pParent)
	{
		m_pParent->RemoveChild(this);
	}
}

//virtual
char *CControlObject::GetFullName()
{
	if (strlen(GetName()) == 0)
		strcpy(m_szFullName, m_pTargetInstance->GetTarget()->GetObjectTypeDescription(m_Type));
	else
		sprintf(m_szFullName, "%s - %s", GetName(), m_pTargetInstance->GetTarget()->GetObjectTypeDescription(m_Type));
	return m_szFullName;
}

// Removes child from the list of children, not deletes child itself
void CControlObject::RemoveChild(CControlObject *pChild)
{
	for (CControlObject *pObject = m_pFirstChild; pObject; pObject = pObject->m_pNext)
	{
		if (pObject == pChild)
		{
			if (pObject->m_pNext)
				pObject->m_pNext->m_pPrev = pObject->m_pPrev;
			if (pObject->m_pPrev)
				pObject->m_pPrev->m_pNext = pObject->m_pNext;
			if (pObject == m_pFirstChild)
			{
				m_pFirstChild = pObject->m_pNext;
			}
			break;
		}
	}
}

void CControlObject::KillChildren()
{
}

void CControlObject::SetName(CString &NewName)
{
	m_Name = NewName;
	m_pTargetInstance->GetDatabase()->SetObjectName(this, NewName);
}

void CControlObject::SetType(long type)
{
	m_Type = type;
	m_pTargetInstance->GetDatabase()->SetObjectType(this, type);
}

void CControlObject::SetNameAndType(CString NewName, long type)
{
	m_Name = NewName;
	m_Type = type;
	m_pTargetInstance->GetDatabase()->SetObjectNameAndType(this, NewName, type);
}

BOOL CControlObject::MC_CanUseParentConnection()
{
	return GetTargetInstance()->GetTarget()->CanUseParentConnection(m_Type);
}

BOOL CControlObject::MC_CanBeControlledBySerialPort()
{
	return GetTargetInstance()->GetTarget()->CanObjectBeControlledBySerialPort(m_Type);
}

BOOL CControlObject::MC_CanConfigureSerialPort()
{
	return GetTargetInstance()->GetTarget()->MC_CanConfigureSerialPort(m_Type);
}

BOOL CControlObject::MC_CanBeControlledByTelnet()
{
	return GetTargetInstance()->GetTarget()->CanObjectBeControlledByTelnet(m_Type);
}

BOOL CControlObject::MC_CanBeControlledBySNMP()
{
	return GetTargetInstance()->GetTarget()->CanObjectBeControlledBySNMP(m_Type);
}

BOOL CControlObject::MC_CanBeControlledByGPIB()
{
	return GetTargetInstance()->GetTarget()->CanObjectBeControlledByGPIB(m_Type);
}

int CControlObject::GetPictureID()
{
	return GetTargetInstance()->GetTarget()->GetObjectPictureID(m_Type);
}

void CControlObject::AddChild(CControlObject *pChild)
{
	// Add to the head of the list
	pChild->m_pNext = m_pFirstChild;
	pChild->m_pPrev = NULL;
	m_pFirstChild = pChild;
}

BOOL CControlObject::HasEventLog()
{
	return m_pTargetInstance->GetTarget()->HasEventLog(GetType());
}

BOOL CControlObject::MC_HasMonitorAndControl()
{
	return m_pTargetInstance->GetTarget()->HasMonitorAndControl(GetType());
}

BOOL CControlObject::MC_GetDefaultTelnetPortNo()
{
	return m_pTargetInstance->GetTarget()->MC_GetDefaultTelnetPortNo(GetType());
}

BOOL CControlObject::MC_CanChangeTelnetPortNo()
{
	return m_pTargetInstance->GetTarget()->MC_CanChangeTelnetPortNo(GetType());
}

BOOL CControlObject::MC_HasSerialPortBusAddress()
{
	return m_pTargetInstance->GetTarget()->MC_HasSerialPortBusAddress(GetType());
}

BOOL CControlObject::MC_LoadMonitorAndControlParams()
{
	CObjectConnectionsRecordset set(m_pTargetInstance->GetDatabase());
	if (set.LoadConnectionParams(m_MCParams, m_ID))
		return TRUE;

	set.AddDefaultParams(m_ID);
	return set.LoadConnectionParams(m_MCParams, m_ID);
}

BOOL CControlObject::MC_SaveMonitorAndControlParams()
{
	CObjectConnectionsRecordset set(m_pTargetInstance->GetDatabase());
	if (set.SaveConnectionParams(m_MCParams, m_ID))
		return TRUE;

	set.AddDefaultParams(m_ID);
	return set.SaveConnectionParams(m_MCParams, m_ID);
}
